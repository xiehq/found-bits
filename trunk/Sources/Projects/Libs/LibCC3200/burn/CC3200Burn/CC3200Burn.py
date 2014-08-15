#####################################################################################
## Copyright (c) 2014, pa.eeapai@gmail.com                                         ##
## All rights reserved.                                                            ##
##                                                                                 ##
## Redistribution and use in source and binary forms, with or without              ##
## modification, are permitted provided that the following conditions are met:     ##
##     * Redistributions of source code must retain the above copyright            ##
##       notice, this list of conditions and the following disclaimer.             ##
##     * Redistributions in binary form must reproduce the above copyright         ##
##       notice, this list of conditions and the following disclaimer in the       ##
##       documentation and/or other materials provided with the distribution.      ##
##     * Neither the name of the pa.eeapai@gmail.com nor the                       ##
##       names of its contributors may be used to endorse or promote products      ##
##       derived from this software without specific prior written permission.     ##
##                                                                                 ##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ##
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   ##
## WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          ##
## DISCLAIMED. IN NO EVENT SHALL pa.eeapai@gmail.com BE LIABLE FOR ANY             ##
## DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES      ##
## (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;    ##
## LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND     ##
## ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT      ##
## (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS   ##
## SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    ##
#####################################################################################
"""
Writes files in specified folder (and subfolders recursively) to CC3200 SPI FLASH with debugger.
Needs "empty" flash image and compiled monitor image.
Debug object provides basic debug operations, e.g. run, read memory, write memory.
Monitor object uses debug object to run different operations on CPU.
Monitor image is loaded to RAM and ran. 
Predefined locations and buffers in monitor image are used for monitor object to monitor
application communication. Two sets of these locations are used to speed up download.
While some chunk is being written to flash next chunk is written to CC3200 buffer.
This is possible because CortexM4 on CC3200 allows memory access with debugger while running.
Operation in CC3200 is done by writing operation index, parameters and buffer to 
predefined locations and last triggering the operation with setting state to active in
predefined location. In next cycle nothing is written until state location is not indicating
idle state.
Flash is formated with operations that call low level SPI functions in peripheral library before
writing files with operations that call SimpleLink library functions for flash file access.
Formatting includes writing "empty" flash image which is read out of flash once after
formatting with Uniflash tool.
"""
import os
import isystem.connect as ic
import time

class Debugger:
  """
  Provides basic debug functions.
  """
  def __init__(self):
    """
    Initializes debug object.

    Connects to winIDEA.
    """
    self.cmgr = ic.ConnectionMgr()
    self.cmgr.connectMRU()
    self.debug = ic.CDebugFacade(self.cmgr)

  def reset(self):
    """
    Resets target device.
    """
    self.debug.reset()

  def run(self):
    """
    Puts target device in running.
    """
    self.debug.run()

  def writeMemory(self, address, buff, length):
    """
    Writes a buffer to target device memory.

    @param address: Where to start writing.
    @param buff: Data to be written.
    @param length: Number of bytes to write.
    """
    icBuff = ic.VectorBYTE(buff)
    self.debug.writeMemory(ic.IConnectDebug.fMonitor | ic.IConnectDebug.fRealTime, # access mode
                           ic.maPhysicalARM,          # memory area
                           address,                   # memory address
                           length,                    # num of MAUs to write
                           1,                         # bytes per MAU
                           icBuff)                    # data to write

  def readMemory(self, address, length):
    """
    Reads memory from target device.

    @param address: Where to start reading from.
    @param length: Number of bytes to read.
    @return: Buffer with read data.
    """
    mem = self.debug.readMemory(ic.IConnectDebug.fMonitor | ic.IConnectDebug.fRealTime, ic.maPhysicalARM, address, length, 1)
    return mem

  def read32(self, address):
    """
    Reads 32bit value from target memory.

    @param address: Location of value to read.
    @return: Read value.
    """
    value = self.debug.evaluate(ic.IConnectDebug.fMonitor | ic.IConnectDebug.fRealTime, "*(unsigned long *){0}".format(address)).getInt()
    return value

  def write32(self, address, val):
    """
    Writes 32bit value to target memory.

    @param address: Location to write the value.
    @param val: Value to write.
    """
    valueType = ic.SType()
    valueType.m_byType = ic.SType.tUnsigned
    valueType.m_byBitSize = 32
    value = ic.CValueType(valueType, val)
    self.debug.writeValue(ic.IConnectDebug.fMonitor | ic.IConnectDebug.fRealTime,
                          ic.maPhysicalARM,
                          address,
                          value)

  def gotoAddress(self, address):
    """
    Presets execution address.

    @param: Address of new execution address.
    """
    self.debug.gotoAddress(ic.maPhysicalARM, address)

class SPIMon:
  """
  Executes operations with monitor application running on target.
  """
  def __init__(self, pathToMon, dbg):
    """
    Initializes constants.

    @param pathToMon: File with binary monitor application image.
    @param dbg: Debugger object.
    """
    self.dbg = dbg
    self.monitorPath = pathToMon

    self.monhMagic               = 0
    self.monhLoadAddress         = 1
    self.monhEntry               = 2
    self.monhBufferSize          = 3
    self.monhBufferAddress       = 4
    self.monhMaxFileNameLength   = 5
    self.monhFileNameAddressA    = 6
    self.monhFileNameAddressB    = 7
    self.monhStatusA             = 8
    self.monhStatusB             = 9
    self.monhOpA                 = 10
    self.monhOpB                 = 11
    self.monhParamA0             = 12
    self.monhParamB0             = 13
    self.monhParamA1             = 14
    self.monhParamB1             = 15
    self.monhParamA2             = 16
    self.monhParamB2             = 17
    self.monhParamA3             = 18
    self.monhParamB3             = 19

    self.opInit                  = 0
    self.opScan                  = 1
    self.opStartSL               = 2
    self.opCreateFile            = 3
    self.opCloseFile             = 4
    self.opWriteFile             = 5
    self.opStopSL                = 6
    self.opDeinit                = 7

    self.stateIDLE   = 0
    self.stateACTIVE = 1
    self.stateERROR  = 2

    self.opsrcA = 0
    self.opsrcB = 1

    self.curOpSrc = self.opsrcA

    self.monitorMagic = 0
    self.monitorLoadAddress = 0
    self.monitorEntry = 0
    self.monitorOpBufferSize = 0
    self.monitorBufferAddressA = 0
    self.monitorBufferAddressB = 0
    self.monitorMaxFileNameLength = 0
    self.monitorFileNameAddressA = 0
    self.monitorFileNameAddressB = 0
    self.monitorSize = 0

    self.monStatusAAddress = 0 
    self.monStatusBAddress = 0
    self.monOpAAddress     = 0
    self.monOpBAddress     = 0
    self.monParamA0Address = 0
    self.monParamB0Address = 0
    self.monParamA1Address = 0
    self.monParamB1Address = 0
    self.monParamA2Address = 0
    self.monParamB2Address = 0
    self.monParamA3Address = 0
    self.monParamB3Address = 0

    self.outBuf = []

  def getHeaderDword(self, monhID):
    """
    Reads 32bit value from predefined address in monitor image (not from target).

    @param monhID: Predefined location index.
    @return: Value read from image.
    """
    return int.from_bytes(self.monitorImg[monhID * 4 : (monhID * 4) + 4], byteorder='little')

  def load(self):
    """
    Loads monitor application image from file to target memory and
    reads constants from predefined locations in image.
    """
    monitorFile = open(self.monitorPath, "rb")
    self.monitorImg = monitorFile.read()
    monitorFile.close()

    self.monitorMagic = self.getHeaderDword(self.monhMagic)
    self.monitorLoadAddress = self.getHeaderDword(self.monhLoadAddress)
    self.monitorEntry = (self.getHeaderDword(self.monhEntry) >> 1) << 1
    self.monitorOpBufferSize = self.getHeaderDword(self.monhBufferSize) >> 1
    self.monitorBufferAddressA = self.getHeaderDword(self.monhBufferAddress)
    self.monitorBufferAddressB = self.monitorBufferAddressA + self.monitorOpBufferSize
    self.monitorMaxFileNameLength = self.getHeaderDword(self.monhMaxFileNameLength)
    self.monitorFileNameAddressA = self.getHeaderDword(self.monhFileNameAddressA)
    self.monitorFileNameAddressB = self.getHeaderDword(self.monhFileNameAddressB)
    self.monitorSize = len(self.monitorImg)

    self.outBuf = bytearray(self.monitorSize)

    self.monStatusAAddress = self.monitorLoadAddress + (self.monhStatusA << 2) 
    self.monStatusBAddress = self.monitorLoadAddress + (self.monhStatusB << 2)
    self.monOpAAddress     = self.monitorLoadAddress + (self.monhOpA     << 2)
    self.monOpBAddress     = self.monitorLoadAddress + (self.monhOpB     << 2)
    self.monParamA0Address = self.monitorLoadAddress + (self.monhParamA0 << 2)
    self.monParamB0Address = self.monitorLoadAddress + (self.monhParamB0 << 2)
    self.monParamA1Address = self.monitorLoadAddress + (self.monhParamA1 << 2)
    self.monParamB1Address = self.monitorLoadAddress + (self.monhParamB1 << 2)
    self.monParamA2Address = self.monitorLoadAddress + (self.monhParamA2 << 2)
    self.monParamB2Address = self.monitorLoadAddress + (self.monhParamB2 << 2)
    self.monParamA3Address = self.monitorLoadAddress + (self.monhParamA3 << 2)
    self.monParamB3Address = self.monitorLoadAddress + (self.monhParamB3 << 2)

  def prepare(self):
    """
    Runs loaded monitor application from address read from image.
    """
    self.dbg.reset()
    self.dbg.writeMemory(self.monitorLoadAddress, self.monitorImg, self.monitorSize)
    self.dbg.gotoAddress(self.monitorEntry)
    self.dbg.run()
    self.curOpSrc = self.opsrcA

  def readParam(self, monhID):
    """
    Reads 32bit value from predefined address in target memory.

    @param monhID: Predefined location index.
    @return: Value read from target.
    """
    address = self.monitorLoadAddress + (monhID << 2)
    return self.dbg.read32(address)

  def writeParam(self, monhID, val):
    """
    Writes 32bit value to predefined address in target memory.

    @param monhID: Predefined location index.
    @param val: Value to write.
    """
    address = self.monitorLoadAddress + (monhID << 2)
    self.dbg.write32(address, val)


  def doOp(self, op, filename, p0, p1, p2, p3, inBuf, outCount):
    """
    Writes parameters to predefined locations in target memory and triggers operation.

    @param op: Operation index.
    @param filename: None or name of the file.
    @param p0: Parameter 0 associated with operation.
    @param p1: Parameter 1 associated with operation.
    @param p2: Parameter 2 associated with operation.
    @param p3: Parameter 3 associated with operation.
    @param inBuf: Input data associated with operation. [] if not needed.
    @param outCount: Expected number of bytes returned from operation. If specified call blocks until operation is done.
    """
    nextOpSrc = self.curOpSrc
    if 0 == outCount:
      if self.opsrcB == self.curOpSrc:
        nextOpSrc = self.opsrcA
      else:
        nextOpSrc = self.opsrcB

    monhStatus = self.monhStatusA
    monhOp = self.monhOpA
    monhP0 = self.monhParamA0
    monhP1 = self.monhParamA1
    monhP2 = self.monhParamA2
    monhP3 = self.monhParamA3
    monitorFileNameAddress = self.monitorFileNameAddressA
    bufAddress = self.monitorBufferAddressA
    
    if self.opsrcB == self.curOpSrc:
      monhStatus = self.monhStatusB
      monhOp = self.monhOpB
      monhP0 = self.monhParamB0
      monhP1 = self.monhParamB1
      monhP2 = self.monhParamB2
      monhP3 = self.monhParamB3
      monitorFileNameAddress = self.monitorFileNameAddressB
      bufAddress = self.monitorBufferAddressB

    self.curOpSrc = nextOpSrc

    status = self.readParam(monhStatus)
    while self.stateACTIVE == status:
      status = self.readParam(monhStatus)

    if not self.stateIDLE == status:
      raise Exception("Monitor error {0}".format(status))

    if not filename is None:
      fileNameBuff = bytearray(filename + "0", "ascii")
      filenameLen = len(fileNameBuff)
      if filenameLen >= self.monitorMaxFileNameLength:
        raise Exception("File name too long: " + filename)
      fileNameBuff[filenameLen - 1] = 0
      self.dbg.writeMemory(monitorFileNameAddress, fileNameBuff, filenameLen)
    if len(inBuf) > 0:
      self.dbg.writeMemory(bufAddress, inBuf, len(inBuf))
    self.writeParam(monhP0, p0)
    self.writeParam(monhP1, p1)
    self.writeParam(monhP2, p2)
    self.writeParam(monhP3, p3)
    self.writeParam(monhOp, op)
    self.writeParam(monhStatus, self.stateACTIVE)
    
    if 0 == outCount:
      return []

    status = self.readParam(monhStatus)
    while self.stateACTIVE == status:
      status = self.readParam(monhStatus)

    if not self.stateIDLE == status:
      raise Exception("Monitor error {0}".format(status))

    return self.dbg.readMemory(bufAddress, outCount)

  def init(self):
    """
    Executes init operation in monitor application running on target.
    """
    self.doOp(self.opInit, None, 0, 0, 0, 0, [], 0)

  def _scan(self, CSEnableBefore, inBuf, count, CSDisableAfter):
    """
    Outputs provided data on SPI and returns received SPI data.
    Optionally changes SPI CE signal at the beginning and at the end.

    @param CSEnableBefore: True to drive CE low on start.
    @param inBuf: Output data.
    @param count: Requested output byte count. Unpredictable extra values are written to SPI if this is more than input data byte count.
    @param CSDisableAfter: True to drive CE high when done.
    @return: SPI received data.
    """
    scanCount = len(inBuf)
    if count > scanCount:
      scanCount = count
    flags = 0
    if CSEnableBefore:
      flags = flags | 1
    if CSDisableAfter:
      flags = flags | 2
    return self.doOp(self.opScan, None, flags, scanCount, 0, 0, inBuf, scanCount)

  def readID(self):
    """
    Reads flash ID.

    @return: SPI ID.
    """
    outData = self._scan(True, bytes([0x9F]), 5, True)
    id = outData[1] << 24
    id = id | (outData[2] << 16)
    id = id | (outData[3] << 8)
    id = id | outData[4]
    return id

  def readStatus(self):
    """
    Reads flash status register.

    @return: Status register.
    """
    outData = self._scan(True, bytes([0x05]), 2, True)
    return outData[1]
    
  def writeEnable(self):
    """
    Sends SPI write enable command.
    """
    outData = self._scan(True, bytes([0x06]), 0, True)
    status = self.readStatus()
    while 0 == (status & 2): # WEL
      status = self.readStatus()
    
  def writeDisable(self):
    """
    Sends SPI write disable command.
    """
    outData = self._scan(True, bytes([0x04]), 0, True)

  def eraseAll(self):
    """
    Sends SPI mass erase command and pools status until completed.
    """
    self.writeEnable()
    outData = self._scan(True, bytes([0xC7]), 0, True)
    status = self.readStatus()
    loopCnt = 0
    while 1 == (status & 1): # WIP
      status = self.readStatus()
      print(".", end="", flush=True)
      loopCnt = loopCnt + 1
      if 64 == loopCnt:
        print("")
        loopCnt = 0
      time.sleep(0.1)

    print("")
    self.writeDisable()

  def _startRead(self, address):
    """
    Sends SPI read command.

    @param address: Address in flash where to start reading.
    """
    cmd = bytearray(4)
    cmd[0] = 0x03
    cmd[1] = (address >> 16) & 0xFF
    cmd[2] = (address >> 8) & 0xFF
    cmd[3] = address & 0xFF
    self._scan(True, cmd, 0, False)

  def _read(self, count):
    """
    Clocks data out of flash in chunks of max size of predefined buffer size.

    @param count: Total number of bytes to read.
    @return: Data read from flash.
    """
    outData = bytearray(0)
    read = 0
    loopCnt = 0
    while read < count:
      singleCount = count - read
      if singleCount > self.monitorOpBufferSize:
        singleCount = self.monitorOpBufferSize
      singleData = self._scan(False, [], singleCount, False)
      outData[read:read + singleCount] = singleData[0:singleCount]
      print(".", end="", flush=True)
      loopCnt = loopCnt + 1
      if 64 == loopCnt:
        print("")
        loopCnt = 0
      read = read + singleCount
    return outData

  def _endRead(self):
    """
    Disables SPI CS signal.
    """
    self._scan(False, [], 0, True)

  def read(self, address, count):
    """
    Reads raw data from flash.

    @param address: Address in flash where to start reading.
    @param count: Total number of bytes to read.
    @return: Data read from flash.
    """
    self._startRead(address)
    out = self._read(count)
    self._endRead()
    return out

  def writePage(self, address, data, maxCount):
    """
    Writes one SPI page of data in flash.

    @param address: Start address of the page in flash.
    @param data: Data to write.
    @param maxCount: Max number of bytes to write. Mustn't exceed page size.
    """
    empty = True
    for i in range(maxCount):
      if not (data[i] == 0xFF):
        empty = False
        break
    if empty:
      return

    self.writeEnable()
    cmd = bytearray(maxCount + 4)
    cmd[0] = 0x02
    cmd[1] = (address >> 16) & 0xFF
    cmd[2] = (address >> 8) & 0xFF
    cmd[3] = address & 0xFF
    cmd[4:] = data[0:]
    self._scan(True, cmd, 0, True)
    status = self.readStatus()
    while 1 == (status & 1): # WIP
      status = self.readStatus()
    self.writeDisable()
    pass


  def write(self, filePath):
    """
    Writes file to beginning of the flash.

    @param filePath: Path to flash image file.
    """
    file = open(filePath, "rb")
    img = file.read()
    file.close()

    pageSize = 256
    fullSize = len(img)
    address = 0
    loopCnt = 0
    while address < fullSize:
      size = fullSize - address
      if size > pageSize:
        size = pageSize
      
      buff = img[address:address + size]
      self.writePage(address, buff, size)

      address = address + size
      print(".", end="", flush=True)
      loopCnt = loopCnt + 1
      if 64 == loopCnt:
        print("")
        loopCnt = 0
    print("")

  def startSL(self):
    """
    Starts SimpleLink to enable calls to flash file functions.
    """
    self.doOp(self.opStartSL, None, 0, 0, 0, 0, [], 0)

  def stopSL(self):
    """
    Stops SimpleLink.
    """
    self.doOp(self.opStopSL, None, 0, 0, 0, 0, [], 0)

  def _createFile(self, name, size):
    """
    Creates new file with call to SimpleLink library. Handle to file is remembered in monitor.

    @param name: File name. Must start with '/'.
    @param size: File size.
    """
    self.doOp(self.opCreateFile, name, size, 0, 0, 0, [], 0)

  def _writeFile(self, buff, size):
    """
    Writes data to last created file.

    @param buff: Data to write.
    @param size: Size of data. Mustn't exceed monitor buffer size.
    """
    self.doOp(self.opWriteFile, None, size, 0, 0, 0, buff, 0)

  def _closeFile(self):
    """
    Closes last created file.
    """
    self.doOp(self.opCloseFile, None, 0, 0, 0, 0, [], 0)

  def writeFile(self, path, name):
    """
    Writes file to flash.

    @param path: Path to file on host.
    @param name: Name of file in flash.
    """
    print("Writing {0} to {1}".format(path, name))
    file = open(path, "rb")
    buff = file.read()
    file.close()
    size = len(buff)
    self._createFile(name, size)

    left = size
    while left > 0:
      chunkSize = left
      if chunkSize > self.monitorOpBufferSize:
        chunkSize = self.monitorOpBufferSize
    
      chunkBuff = buff[size - left:size - left + chunkSize]
      self.doOp(self.opWriteFile, None, chunkSize, 0, 0, 0, chunkBuff, 0)

      left = left - chunkSize

    self._closeFile()


def writeFile(mon, fsRoot, file):
  if "\\sys\\" in file:
    return
  name = file[len(fsRoot):]
  mon.writeFile(file, name)

def writeFilesIn(mon, path):
  for root, dirs, files in os.walk(path):
    if "_session" in root:
      continue
    for file in files:
      if file.endswith(".ucf"):
        continue
      writeFile(mon, path, os.path.join(root, file))

def doMon(monPath, dumpPath, dumpSize, erase, formatPath, programPath, appPath):
  """
  Performs selected SPI flash operations in following order:
  Reads flash contents and dumps it in specified file.
  Performs raw SPI mass erase.
  Writes "empty" image from specified path.
  Downloads files from specified folder path and subfolders recursively.

  @param monPath: Monitor binary image to load to RAM and run.
                  <scriptFolder>\..\monitor\eclipse\Debug\CC3200Burn.bin is used if None.
  @param dumpPath: Where to dump the content of flash. Dump step is skipped when not given.
  @param dumpSize: How much to read from flash. Usually the size of flash in bytes.
                   Must be given when dumpPath is set.
  @param erase: True to erase flash. False to skip erase step.
  @param formatPath: Formated image to program in erased flash.
                     Writing "empty" image is skipped if not provided.
                     Erase step is implied when provided regardless of erase param.
  @param programPath: Path to folder which content (with subfolders recursively) should be programmed.
                      Programming step is skipped if not provided.
  @param appPath: Application binary to be programmed as "/sys/mcuimg.bin".
  """
  print("Connecting to winIDEA... ", end="", flush=True)
  debugger = Debugger()
  print("DONE")
  mon = SPIMon(monPath, debugger)
  print("Reading monitor image from {0} ... ".format(monPath), end="", flush=True)    
  mon.load()
  print("DONE")
  print("Starting debug and writing monitor image to RAM... ", end="", flush=True)
  mon.prepare()
  print("DONE")
  print("Monitor init call... ", end="", flush=True)
  mon.init()
  print("DONE")
  print("Reading flash ID... ", end="", flush=True)
  id = mon.readID()
  print("DONE. SPI FLASH ID: 0x{0:08X}".format(id))

  if dumpSize > 0:
    print("Creating {0} to dump raw flash image... ".format(dumpPath), end="", flush=True)    
    imgFile = open(dumpPath, "wb")
    print("DONE")
    print("Reading flash image... ")
    img = mon.read(0, dumpSize)
    count = len(img)
    print("Read {0} Bytes".format(count))
    print("Dumping data... ", end="", flush=True)
    imgFile.write(img)
    imgFile.close()
    print("DONE")

  format = not (formatPath == None)
  erase = erase or format
  program = not (programPath == None)

  if erase:
    print("Erasing SPI FLASH... ")
    mon.eraseAll()
    print("DONE")

  if format:
    print("Writing \"empty\" image... ")
    mon.write(formatPath)
    print("DONE")

  progApp = not ( appPath == None )

  if program or progApp:
    print("Starting SimpleLink... ", end="", flush=True)
    mon.startSL()
    print("DONE")

  if program:
    writeFilesIn(mon, programPath)

  if progApp:
    mon.writeFile(appPath, "/sys/mcuimg.bin")

  if program or progApp:
    print("Stopping SimpleLink... ", end="", flush=True)
    mon.stopSL()
    print("DONE")

class args:
  def __init__(self, argv):
    self.monPath = os.path.abspath(os.path.dirname(__file__) + "\\..\\monitor\\eclipse\\Debug\\CC3200Burn.bin")
    self.dumpPath = None
    self.dumpSize = 0
    self.erase = False
    self.formatPath = None
    self.progPath = None
    self.appPath = None

    for arg in argv:
      if arg.startswith("-M"): self.monPath = os.path.abspath(arg[2:])
      elif arg.startswith("-D"): self.dumpPath = os.path.abspath(arg[2:])
      elif arg.startswith("-S"): self.dumpSize = int(arg[2:], 0)
      elif arg.startswith("-E"): self.erase = True
      elif arg.startswith("-F"): self.formatPath = os.path.abspath(arg[2:])
      elif arg.startswith("-P"): self.progPath = os.path.abspath(arg[2:])
      elif arg.startswith("-A"): self.appPath = os.path.abspath(arg[2:])
      else: print("Skipping unknown param: " + arg)

def main():
  """
  Usage: CC3200Burn.py [-M<path>] [-D<path> -S<size>] [-E] [-F<path>] [-P<path>] [-A<path>]
  
  -M<path> : Path to monitor image. <scriptFolder>\..\monitor\eclipse\Debug\CC3200Burn.bin is tried if None. 
  -D<path> : Where to dump raw image.
  -S<size> : Number of bytes to dump. Must be given if -D.
  -E       : Low level SPI flash erase.
  -F<path> : Formated image path. Dumped image after erase (-E) and format with Uniflash.
  -P<path> : Folder with files to program. /sys/mcuimg.bin is ignored.
  -A<path> : Path to application image. Programmed as "/sys/mcuimg.bin".
  """
  import sys
  if 2 > len(sys.argv):
    print(main.__doc__)
    return
  conf = args(sys.argv[1:])
  doMon(conf.monPath, conf.dumpPath, conf.dumpSize, conf.erase, conf.formatPath, conf.progPath, conf.appPath)

if __name__ == "__main__":
  main()

