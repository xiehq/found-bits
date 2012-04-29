#####################################################################################
## Copyright (c) 2012, pa.eeapai@gmail.com                                         ##
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

import tkFileDialog
import Tkinter
import sys
import os
import shutil
import distutils.dir_util
import datetime


root = Tkinter.Tk()
root.withdraw()

WDKPath = "C:\\WinDDK\\7600.16385.1"

now = datetime.datetime.now()
date = now.strftime("%m/%d/%Y")

name = raw_input("Enter the name (e.g. Awesome Embedded Developer). Don't use characters that are not valid in file names!: ")
certPrepend = name.replace(' ', '')
usbVID = raw_input("Enter your USB VID (4 HEX characters, without 0x or h or hex):")
usbPID = raw_input("Enter USB PID for the device (4 HEX characters, without 0x or h or hex):")

if not os.path.isdir(WDKPath):
  WDKPath = tkFileDialog.askdirectory()

if not os.path.isdir(WDKPath):
  print "WDK path is needed. You must have Windows Driver Kit (WDK) installed."
  sys.exit (-1)

driverPath = "Driver"
driverPath = os.path.abspath(driverPath)
caPVK = certPrepend + "CA.pvk"
caCER = certPrepend + "CA.cer"
spcPVK = certPrepend + "SPC.pvk"
spcCER = certPrepend + "SPC.cer"
spcPFX = certPrepend + "SPC.pfx"

bitVersion = "x86"

MakeCert = os.path.join(WDKPath, "bin\\" + bitVersion + "\\MakeCert.exe")
certutil = "certutil.exe"
pvk2pfx = os.path.join(WDKPath, "bin\\" + bitVersion + "\\pvk2pfx.exe")
chkinf = os.path.join(WDKPath, "Tools\\Chkinf\\chkinf.bat")
Inf2Cat = os.path.join(WDKPath, "bin\\selfsign\\Inf2Cat.exe")
SignTool = os.path.join(WDKPath, "bin\\" + bitVersion + "\\SignTool.exe")

def execute(command, args):
  fullCommand = command + " " + args
  print "Executing " + fullCommand
  return os.system(fullCommand)

# Create certificates
if (not os.path.isfile(caPVK)) or (not os.path.isfile(caCER)):
   execute(MakeCert, "-r -pe -n \"CN=" + name +" CA\" -ss CA -sr CurrentUser -a sha1 -sky signature -sv " + caPVK + " " + caCER)
if (not os.path.isfile(caPVK)) or (not os.path.isfile(caCER)):
   print "Could not create CA certificate."
   sys.exit(-1)

if (not os.path.isfile(spcPVK)) or (not os.path.isfile(spcCER)):
   execute(MakeCert, "-pe -n \"CN=" + name +" SPC\" -a sha1 -sky signature -ic " + caCER + " -iv " + caPVK + " -sv " + spcPVK + " " + spcCER)
if (not os.path.isfile(spcPVK)) or (not os.path.isfile(spcCER)):
   print "Could not create SPC certificate."
   sys.exit(-1)

if not os.path.isfile(spcPFX):
   execute(pvk2pfx, "-pvk " + spcPVK + " -spc " + spcCER + " -pfx " + spcPFX) #-po for pfx password
if not os.path.isfile(spcPFX):
   print "Could not create PFX."
   sys.exit(-1)

# Clean Driver folder
if os.path.isdir(driverPath):
  shutil.rmtree(driverPath)

# Copy redistributable files
wdfCoInstPath = os.path.join(WDKPath, "redist\\wdf")
winusbCoInstPath = os.path.join(WDKPath, "redist\\winusb")
dpinstPath = os.path.join(WDKPath, "redist\\DIFx\\dpinst\\MultiLin")

distutils.dir_util.copy_tree(wdfCoInstPath, driverPath)
distutils.dir_util.copy_tree(winusbCoInstPath, driverPath)
distutils.dir_util.copy_tree(dpinstPath, driverPath)

wdfCoInstVer = ""
wdfCoInstName = "WdfCoInstaller"
for filename in os.listdir(os.path.join(wdfCoInstPath, "x86")):
  if -1 == filename.find(wdfCoInstName):
    continue
  filename = filename.replace("_", "")
  filename = filename.replace("WdfCoInstaller", "")
  filename = filename.replace(".dll", "")
  filename = filename.replace("chk", "")
  wdfCoInstVer = filename
  break

if "" == wdfCoInstVer:
   print "Could not determine WdfCoInstaller version."
   sys.exit(-1)

wdfMM = int(wdfCoInstVer[:2])
wdfmmm = int(wdfCoInstVer[2:])
kmdfLibVer = "{0}.{1}".format(wdfMM, wdfmmm)
i = int("09")

# Create INF file
infTemplatePath =("WinUSBCommTemplate.inf")
infPath = os.path.join(driverPath, "WinUSBComm.inf")

with file(infTemplatePath, 'r') as infTemplate:
  data = infTemplate.read()

data = data.replace("<DATE>", date)
data = data.replace("<USBVID>", usbVID)
data = data.replace("<USBPID>", usbPID)
data = data.replace("<KMDFLIBVER>", kmdfLibVer)
data = data.replace("<WDFCOINSTVER>", wdfCoInstVer)
data = data.replace("<NAME>", name)
  
with file(infPath, 'w') as inf:
  inf.write(data)

# Create usbID.h - ask first
if len(raw_input("Create usbID.h with entered USB IDs (if file exists it will be deleted)?  [ENTER for NO, anything for YES]:")):
  with file(".\\..\\usbIDTemplate.h", 'r') as usbIDhTemplate:
    data = usbIDhTemplate.read()
  
  data = data.replace("<USBVID>", "0x" + usbVID)
  data = data.replace("<USBPID>", "0x" + usbPID)
  data = data.replace("<NAME>", name)
    
  with file(".\\..\\usbID.h", 'w') as usbIDh:
    usbIDh.write(data)

# Check INF file
if os.path.isdir("htm"):
  shutil.rmtree("htm")
if 0 != execute(chkinf, infPath):
  print "INF file check failed. See .\\htm\\"
  sys.exit(-1)

# Create CAT file
catPath = os.path.join(driverPath, "WinUSBComm.cat")
execute(Inf2Cat, "/driver:" + driverPath + " /os:XP_X86,XP_X64,Vista_X86,Server2008_X86,Vista_X64,Server2008_X64,Server2008_IA64,7_X86,7_X64,Server2008R2_X64,Server2008R2_IA64")
if not os.path.isfile(catPath):
   print "Could not create catalog file."
   sys.exit(-1)

# Sign catalog file
execute(SignTool, "sign /f " + spcPFX + " /t http://timestamp.verisign.com/scripts/timstamp.dll " + catPath)

print "Driver package ready in " + driverPath

# Install driver - ask first 
if len(raw_input("Install driver? [ENTER for NO, anything for YES]:")):
  if 'PROGRAMFILES(X86)' in os.environ:
    bitVersion = "amd64"
  
  dpinstPath = os.path.join(driverPath, bitVersion + "\\dpinst.exe")
  execute(dpinstPath, "/q /se /sw /path " + driverPath)  

