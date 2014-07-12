/////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, pa.eeapai@gmail.com                                         //
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

#include "Conf.h"
#include "WinUSBCommShared.h"
#include "WinUSBCommSTM32F103.h"
#include <string.h>

#define WINUSB_RX_BUFFER_SIZE 0x2000
#define WINUSB_TX_BUFFER_SIZE 0x2000

static uint8_t s_abyIncomingBuffer[WINUSB_RX_BUFFER_SIZE];
static uint8_t s_abyOutgoingBuffer[WINUSB_TX_BUFFER_SIZE];

static uint8_t * s_pIncomingData = s_abyIncomingBuffer; ///< pointer to write the data received from USB (USB to WinUSBComm)
static uint8_t * s_pOutgoingData = NULL;                ///< pointer to read the data to send to USB (WinUSBComm to USB)

static uint8_t * s_pReadData = NULL;                    ///< pointer to read the data received from USB (WinUSBComm to application)
static uint8_t * s_pWriteData = NULL;                   ///< pointer to write the data to send to USB (application to WinUSBComm)

static uint32_t s_dwMaxRxBufferSize = sizeof(s_abyIncomingBuffer);
static EWinUSBCommStatus s_eWinUSBCommStatus = winusbcommIDLE;
static uint8_t s_byError = 0;

static uint32_t s_dwOutgoingDataSize = 0;

static uint8_t s_abyControlDataTestBuff[4] = { 0 };

void WinUSBComm_Reset()
{
  s_pIncomingData = s_abyIncomingBuffer;
  s_pOutgoingData = NULL;
  s_pReadData = NULL;
  s_pWriteData = NULL;
  s_byError = 0;
  s_eWinUSBCommStatus = winusbcommIDLE;
  WinUSBComm_SetOutEndpointReady();
}

void WinUSBComm_AllDataSentToDevice()
{
  if ( winusbcommERROR != s_eWinUSBCommStatus )
  { s_eWinUSBCommStatus = winusbcommPROCESSING; }
}

void WinUSBComm_ProcessNewData()
{
  if ( winusbcommPROCESSING != s_eWinUSBCommStatus )
  { return; }

  s_dwOutgoingDataSize = 0;

  s_pReadData = s_abyIncomingBuffer;
  s_pWriteData = s_abyOutgoingBuffer;
  WinUSBComm_OnNewData();

  if ( s_byError )
  {
    s_byError = 0;
    s_eWinUSBCommStatus = winusbcommERROR;
    return;
  }

  s_pOutgoingData = s_abyOutgoingBuffer;
  s_dwOutgoingDataSize = s_pWriteData - s_pOutgoingData;
  s_eWinUSBCommStatus = winusbcommIDLE;
  WinUSBComm_In();//SetEPTxValid(EP1_IN); //
}

uint8_t * WinUSBComm_ReadData(uint32_t dwLength)
{
  ASSERT( !s_byError );
  if ( s_byError )
  { return NULL; }
  ASSERT( s_pReadData );
  if ( !s_pReadData )
  {
    s_byError = 1;
    return NULL;
  }
  ASSERT( (s_pReadData + dwLength) <= s_pIncomingData );
  if ( (s_pReadData + dwLength) > s_pIncomingData )
  {
    s_byError = 1;
    return NULL;
  }

  uint8_t *pData = s_pReadData;
  s_pReadData += dwLength;
  return pData;
}

void WinUSBComm_WriteData(uint8_t *pbyData, uint32_t dwLength)
{
  ASSERT( !s_byError );
  if ( s_byError )
  { return; }
  ASSERT( s_pWriteData );
  if ( !s_pWriteData )
  {
    s_byError = 1;
    return;
  }
  ASSERT( (s_pWriteData + dwLength) <= (s_abyOutgoingBuffer + sizeof(s_abyOutgoingBuffer)) );
  if ( (s_pWriteData + dwLength) > (s_abyOutgoingBuffer + sizeof(s_abyOutgoingBuffer)) )
  {
    s_byError = 1;
    return;
  }
  memcpy(s_pWriteData, pbyData, dwLength);
  s_pWriteData += dwLength;
}

void WinUSBComm_In()  // on EP1_IN (from WinUSBComm to USB)
{
  uint32_t dwNumBytesToSend = s_pWriteData - s_pOutgoingData;
  uint32_t dwInEndpointBufferSize = WinUSBComm_GetInEndpointBufferSize();
  if ( dwNumBytesToSend > dwInEndpointBufferSize )
  { dwNumBytesToSend = dwInEndpointBufferSize; }

  ASSERT( s_pWriteData );
  if ( ( !s_pOutgoingData ) || ( !s_pWriteData ) )
  {
    s_eWinUSBCommStatus = winusbcommERROR;
    WinUSBComm_ErrorOnInEndpoint();
    return;
  }

  if ( !dwNumBytesToSend )
  {
    WinUSBComm_ErrorOnInEndpoint();
    return;
  }

  WinUSBComm_WriteToInEndpoint(s_pOutgoingData, dwNumBytesToSend);
  s_pOutgoingData += dwNumBytesToSend;
  WinUSBComm_SetInEndpointReady();
}

void WinUSBComm_Out() // on EP2_OUT (from USB to WinUSBComm)
{
  uint32_t dwNumBytesToReceive = WinUSBComm_GetNumBytesToReadFromOutEndpoint();
  ASSERT( (s_pIncomingData + dwNumBytesToReceive) <= (s_abyIncomingBuffer + sizeof(s_abyIncomingBuffer)) );
  if ( (s_pIncomingData + dwNumBytesToReceive) > (s_abyIncomingBuffer + sizeof(s_abyIncomingBuffer)) )
  {
    s_eWinUSBCommStatus = winusbcommERROR;
    WinUSBComm_ErrorOnOutEndpoint();
    return;
  }
  ASSERT( s_pIncomingData );
  if ( !s_pIncomingData )
  {
    s_eWinUSBCommStatus = winusbcommERROR;
    WinUSBComm_ErrorOnOutEndpoint();
    return;
  }

  dwNumBytesToReceive = WinUSBComm_ReadFromOutEndpoint(s_pIncomingData);
  s_pIncomingData += dwNumBytesToReceive;
  WinUSBComm_SetOutEndpointReady();
}

uint32_t WinUSBComm_GetAvailableByteCount()
{
  return (uint32_t)(s_pIncomingData - s_pReadData);
}

uint16_t WinUSBComm_Control(uint8_t byRequest, uint8_t **ppData, uint16_t wControlDataSizeToBeReceived)
{
  uint16_t wControlDataSize = 0;
  if ( ppData ) *ppData = NULL;
  switch ( byRequest )
  {
  case winusbctrlRESET: WinUSBComm_Reset(); break;
  case winusbctrlGETSTATUS: wControlDataSize = 1; if ( ppData ) *ppData = (uint8_t *)&s_eWinUSBCommStatus; break;
  case winusbctrlTXDONE: WinUSBComm_AllDataSentToDevice(); break;
  case winusbctrlGETDATASIZE: wControlDataSize = 4; if ( ppData ) *ppData = (uint8_t *)&s_dwOutgoingDataSize; break;
  case winusbctrlGETBUFFSIZE: wControlDataSize = 4; if ( ppData ) *ppData = (uint8_t *)&s_dwMaxRxBufferSize; break;

  case winusbctrlEXAMPLEDATA4B:
    ASSERT( 4 == wControlDataSizeToBeReceived );
    wControlDataSize = 4;
    if ( ppData )
    { *ppData = (uint8_t *)s_abyControlDataTestBuff; }  // tell where to copy received data
    break;
  default: break;
  }
  return wControlDataSize;
}

