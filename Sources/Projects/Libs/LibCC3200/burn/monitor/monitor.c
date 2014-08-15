/////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014, pa.eeapai@gmail.com                                         //
// All rights reserved.                                                            //
//                                                                                 //
// Redistribution and use in source and binary forms, with or without              //
// modification, are permitted provided that the following conditions are met:     //
//     * Redistributions of source code must retain the above copyright            //
//       notice, this list of conditions and the following disclaimer.             //
//     * Redistributions in binary form must reproduce the above copyright         //
//       notice, this list of conditions and the following disclaimer in the       //
//       documentation and/or other materials provided with the distribution.      //
//     * Neither the name of the pa.eeapai@gmail.com nor the                       //
//       names of its contributors may be used to endorse or promote products      //
//       derived from this software without specific prior written permission.     //
//                                                                                 //
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND //
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   //
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          //
// DISCLAIMED. IN NO EVENT SHALL pa.eeapai@gmail.com BE LIABLE FOR ANY             //
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES      //
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;    //
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND     //
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT      //
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS   //
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                    //
/////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "simplelink.h"
#include "netcfg.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "utils.h"
#include "pin.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "uart.h"
#include "monitor.h"
#include "spi.h"

#include <stdio.h>

void Reset_Handler();

typedef enum _EOperationSource
{
  opsrcA,
  opsrcB,
}EOperationSource;

volatile unsigned char g_abyMonitorBuffer[MONITOR_BUFFER_SIZE];
volatile unsigned char g_abyFileNameA[MAX_FILE_NAME_LENGTH];
volatile unsigned char g_abyFileNameB[MAX_FILE_NAME_LENGTH];
__attribute__ ((section(".monitor_header")))
volatile unsigned int g_adwMonitorHeader[] =
{
  0xB007FACE,                         ///< Magic
  (unsigned int)g_adwMonitorHeader,   ///< Monitor load address
  (unsigned int)Reset_Handler,        ///< Entry
  MONITOR_BUFFER_SIZE,                ///< Max buffer size (use half for double buffering)
  (unsigned int)g_abyMonitorBuffer,   ///< Buffer address
  MAX_FILE_NAME_LENGTH,               ///< Max file name length
  (unsigned int)g_abyFileNameA,       ///< File name A
  (unsigned int)g_abyFileNameB,       ///< File name B
  0,                                  ///< Status A (of EState)
  0,                                  ///< Status B (of EState)
  0,                                  ///< Operation A
  0,                                  ///< Operation B
  0,                                  ///< Parameter A0
  0,                                  ///< Parameter B0
  0,                                  ///< Parameter A1
  0,                                  ///< Parameter B1
  0,                                  ///< Parameter A2
  0,                                  ///< Parameter B2
  0,                                  ///< Parameter A3
  0,                                  ///< Parameter B3
};

void spiScan(unsigned long dwIn, unsigned long *pdwOut)
{
  SPIDataPut(SSPI_BASE, dwIn);
  SPIDataGet(SSPI_BASE, pdwOut);
}

void spiCS(unsigned char bEnable)
{
  SPICSDisable(SSPI_BASE);
  if ( bEnable )
  {
    SPICSDisable(SSPI_BASE);
    SPICSEnable(SSPI_BASE);
  }
}

unsigned long spiReadID()
{
  unsigned long dwOut = 0;
  unsigned long dwID = 0;

  spiCS(1);

  spiScan(0x9F, &dwOut);

  spiScan(0, &dwOut);
  dwID <<= 8;
  dwID |= (0xFF & dwOut);
  spiScan(0, &dwOut);
  dwID <<= 8;
  dwID |= (0xFF & dwOut);
  spiScan(0, &dwOut);
  dwID <<= 8;
  dwID |= (0xFF & dwOut);
  spiScan(0, &dwOut);
  dwID <<= 8;
  dwID |= (0xFF & dwOut);

  spiCS(0);

  return dwID;
}

void spiDumpSome()
{
  unsigned long dwOut = 0;

  unsigned long dwA0 = 0;
  unsigned long dwA1 = 0;
  unsigned long dwA2 = 0;

  unsigned int dwLn = 0;
  unsigned int dwCol = 0;

  spiCS(1);

  spiScan(0x03, &dwOut);
  spiScan(dwA0, &dwOut);
  spiScan(dwA1, &dwOut);
  spiScan(dwA2, &dwOut);
  for ( dwLn = 0; dwLn < 16; dwLn++ )
  {
    for ( dwCol = 0; dwCol < 8; dwCol++ )
    {
      spiScan(0x00, &dwOut);
      dwOut &= 0xFF;
      LOG_PRINT("%02X ", (unsigned int)dwOut);
    }
    LOG_PRINT("\n\r");
  }
  spiCS(0);
}

unsigned char spiReadSTATUS(unsigned char bFirstRead, unsigned char bLastRead)
{
  unsigned long dwOut = 0;
  if ( bFirstRead )
  {
    spiCS(1);
    spiScan(0x05, &dwOut);
  }

  spiScan(0x05, &dwOut);

  if ( bLastRead )
  {
    spiCS(0);
  }
  return (unsigned char)dwOut;
}

enum ESPIStatus
{
  spistatusWIP = (1 << 0),
  spistatusWEL = (1 << 1),
  spistatusBPM = (7 << 2),
  spistatusBP0 = (1 << 2),
  spistatusBP1 = (1 << 3),
  spistatusBP2 = (1 << 4),
  spistatusTBb = (1 << 5),
  spistatusWP = (1 << 7)
};

void spiEraseAll()
{
  unsigned long dwOut = 0;
  unsigned char byStatus = 0;

  byStatus = spiReadSTATUS(1, 1);

  spiCS(1);
  spiScan(0x06, &dwOut);
  spiCS(0);


  while ( !(spistatusWEL & byStatus) )
  {
    byStatus = spiReadSTATUS(1, 1);
  }

  spiCS(1);
  spiScan(0xC7, &dwOut);
  spiCS(0);

  byStatus = spiReadSTATUS(1, 0);
  while ( spistatusWIP & byStatus )
  {
    byStatus = spiReadSTATUS(0, 0);
  }
  byStatus = spiReadSTATUS(0, 1);

  spiCS(1);
  spiScan(0x04, &dwOut);
  spiCS(0);
}

int exMassErase()
{
  spiEraseAll();
  return monOK;
}

int exInit()
{
  spiCS(0);
  return monOK;
}

int exSPIScan(unsigned char byCSEnableBefore, unsigned long dwNumBytes, unsigned char *pBuff, unsigned char byCSDisableAfter)
{
  unsigned long dwOut = 0;
  if ( byCSEnableBefore )
  {
    spiCS(1); // enable CS
  }
  while ( dwNumBytes-- )
  {
    spiScan(*pBuff, &dwOut);
    *pBuff++ = (unsigned char)dwOut;
  }
  if ( byCSDisableAfter )
  {
    spiCS(0); // disable CS
  }
  return monOK;
}

int exStartSL()
{
  int nResult = sl_Start(0, 0, 0);
  switch ( nResult )
  {
  case ROLE_STA:
  case ROLE_AP:
  case ROLE_P2P:
    return monOK;
  default:
    break;
  }
  return monERROR;
}

int exStopSL()
{
  int nResult = sl_Stop(30);
  if ( nResult )
  {
    return monERROR;
  }
  return monOK;
}

long g_hFileHandle = 0;
unsigned long int g_nOffset = 0;

int exCreateFile(char *pszName, unsigned long dwMaxSize)
{
  long ret = sl_FsOpen((unsigned char *)pszName, FS_MODE_OPEN_CREATE(dwMaxSize, _FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE), NULL, &g_hFileHandle);
  g_nOffset = 0;
  if ( ret )
  {
    g_hFileHandle = 0;
    return monERROR;
  }
  return monOK;
}

int exWriteFile(unsigned char *pbyData, unsigned long dwByteCount)
{
  long ret = sl_FsWrite(g_hFileHandle, g_nOffset, pbyData, dwByteCount);
  if ( ret < 0 )
  {
    return monERROR;
  }
  if ( ret != dwByteCount )
  {
    return monERROR;
  }
  g_nOffset += dwByteCount;
  return monOK;
}

int exCloseFile()
{
  long ret = sl_FsClose(g_hFileHandle, 0, 0, 0);
  g_hFileHandle = 0;
  g_nOffset = 0;
  if ( ret )
  {
    return monERROR;
  }
  return monOK;
}

int Execute(EOperationSource eOpSrc)
{
  int nResult = 0;
  unsigned char *pbyBuff = (unsigned char *)g_adwMonitorHeader[monhBufferAddress];
  char *pszFileName = (char *)g_adwMonitorHeader[monhFileNameAddressA];
  unsigned int dwOp = g_adwMonitorHeader[monhOpA];
  unsigned int dwP0 = g_adwMonitorHeader[monhParamA0];
  unsigned int dwP1 = g_adwMonitorHeader[monhParamA1];
  unsigned int dwP2 = g_adwMonitorHeader[monhParamA2];
  unsigned int dwP3 = g_adwMonitorHeader[monhParamA3];
  if ( opsrcB == eOpSrc )
  {
    pbyBuff += MONITOR_BUFFER_SIZE >> 1;
    pszFileName = (char *)g_adwMonitorHeader[monhFileNameAddressB];
    dwOp = g_adwMonitorHeader[monhOpB];
    dwP0 = g_adwMonitorHeader[monhParamB0];
    dwP1 = g_adwMonitorHeader[monhParamB1];
    dwP2 = g_adwMonitorHeader[monhParamB2];
    dwP3 = g_adwMonitorHeader[monhParamB3];
  }
  
  switch ( dwOp )
  {
  case opInit: nResult = exInit(); break;
  case opScan: nResult = exSPIScan((unsigned char)(dwP0 & scanCSEnableBefore), dwP1, pbyBuff, (unsigned char)(dwP0 & scanCSDisableAfter)); break;
  case opStartSL: nResult = exStartSL(); break;
  case opStopSL: nResult = exStopSL(); break;
  case opCreateFile: nResult = exCreateFile(pszFileName, dwP0); break;
  case opWriteFile: nResult = exWriteFile(pbyBuff, dwP0); break;
  case opCloseFile: nResult = exCloseFile(); break;
  default: nResult = monERROR; break;
  }

  return nResult;
}

void MonitorLoop()
{
  int nResult;
  g_adwMonitorHeader[monhStatusA] = stateIDLE;
  g_adwMonitorHeader[monhStatusB] = stateIDLE;

  LOG_PRINT("\33[2J\n\r");    // clear screen command
  LOG_PRINT("\33[3J\n\r");    // clear scroll back
  LOG_PRINT("CC3200 flash programming monitor started\n\r");

  while ( 1 )
  {
    if ( stateACTIVE == g_adwMonitorHeader[monhStatusA] )
    {
      nResult = Execute(opsrcA);
      if ( nResult )
      {
        g_adwMonitorHeader[monhStatusA] = nResult;
        break;
      }
      g_adwMonitorHeader[monhStatusA] = stateIDLE;
    }

    if ( stateACTIVE == g_adwMonitorHeader[monhStatusB] )
    {
      nResult = Execute(opsrcB);
      if ( nResult )
      {
        g_adwMonitorHeader[monhStatusB] = nResult;
        break;
      }
      g_adwMonitorHeader[monhStatusB] = stateIDLE;
    }
  }
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
}

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pSlWlanEvent)
{
}


void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
}

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
}
