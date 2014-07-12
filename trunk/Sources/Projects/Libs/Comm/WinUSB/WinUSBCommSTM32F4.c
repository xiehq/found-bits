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

#include "WinUSBCommSTM32F4.h"
#include <string.h>

enum EWinUSBCommSTM32F4Flags
{
  wucstmflNone        = 0x00,
  wucstmflTxOverflow  = 0x01,
  wucstmflRxOverflow  = 0x02,
};

static void winusbcommstm32f4_HostInit(SCommLayer *psCommLayer)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  psThis->m_byFlags = wucstmflNone;
  psThis->m_dwExpectedByteCountUSB = 0;
  psThis->m_dwSendByteCountUSB = 0;
  psThis->m_pbyStorePtr = psThis->m_pbyBuffer;
  psThis->m_pbyReceivePtrUSB = psThis->m_pbyBuffer;
  psThis->m_pbySendPtrUSB = psThis->m_pbyBuffer;
  psThis->m_byStateUSB = winusbcomm2stateIdle;
}
static COMMCOUNT winusbcommstm32f4_CommGetBufferSize(SCommLayer *psCommLayer, COMMCOUNT cntLowerLayerBufferSize)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  return psThis->m_dwBufferSizeInBytes;
}
static void winusbcommstm32f4_PacketStart(SCommLayer *psCommLayer)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  psThis->m_dwSendByteCountUSB = 0;
  psThis->m_pbySendPtrUSB = psThis->m_pbyBuffer;
  psThis->m_byFlags &= ~wucstmflTxOverflow;
}
static void winusbcommstm32f4_Send(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  COMMCOUNT cntBufferSize = winusbcommstm32f4_CommGetBufferSize(psCommLayer, 0);

  if ( psThis->m_byFlags & wucstmflTxOverflow )
  {
    return;
  }
  if ( (psThis->m_pbyWritePtr + cntByteCount) > (psThis->m_pbyBuffer + cntBufferSize) )
  {
    psThis->m_byFlags |= wucstmflTxOverflow;
    return;
  }
  if ( psThis->m_pbyWritePtr != pbyData ) // for speed test
  {
    memmove(psThis->m_pbyWritePtr, pbyData, cntByteCount);
  }
  psThis->m_pbyWritePtr += cntByteCount;
}
static void winusbcommstm32f4_PacketEnd(SCommLayer *psCommLayer)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  if ( psThis->m_byFlags & wucstmflTxOverflow )
  {
    return;
  }
  psThis->m_dwSendByteCountUSB = psThis->m_pbyWritePtr - psThis->m_pbyBuffer;
}

static ECommStatus winusbcommstm32f4_TransmitProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount, unsigned char **ppData)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  EWinUSBComm2State eWinUSBComm2State = psThis->m_byStateUSB;
  if ( pcntByteCount )
  {
    *pcntByteCount = (COMMCOUNT)(psThis->m_pbyWritePtr - psThis->m_pbyBuffer - psThis->m_dwSendByteCountUSB);
  }
  switch ( eWinUSBComm2State )
  {
  case winusbcomm2stateSending: return commstatusActive;
  case winusbcomm2stateProcessing: return commstatusActive;
  case winusbcomm2stateIdle: return commstatusIdle;
  default: break;
  }
  return commstatusError;
}
static ECommStatus winusbcommstm32f4_ReceiveProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount, unsigned char **ppData)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  EWinUSBComm2State eWinUSBComm2State = psThis->m_byStateUSB;
  unsigned long dwReceivedByteCountUSB = psThis->m_pbyReceivePtrUSB - psThis->m_pbyBuffer;
  if ( pcntByteCount )
  {
    *pcntByteCount = (COMMCOUNT)dwReceivedByteCountUSB;
  }
  switch ( eWinUSBComm2State )
  {
  case winusbcomm2stateReceiving: return commstatusActive;
  case winusbcomm2stateIdle: return commstatusIdle;
  case winusbcomm2stateProcessing:
    Comm_OnNewPacket(psCommLayer);
    COMM_ONDATA(psThis->m_pbyBuffer, (COMMCOUNT)dwReceivedByteCountUSB);
    *ppData = psThis->m_pbyBuffer;
    return commstatusNewPacket;
  default: break;
  }
  return commstatusError;
}

static void winusbcommstm32f4_OnNewPacket(SCommLayer *psCommLayer)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  psThis->m_pbyStorePtr = psThis->m_pbyBuffer;
  psThis->m_pbyWritePtr = psThis->m_pbyBuffer;
  psThis->m_byFlags &= ~wucstmflRxOverflow;
  psThis->m_dwSendByteCountUSB = 0;
}

static void winusbcommstm32f4_Store(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  if ( pbyData != psThis->m_pbyStorePtr )
  {
    memmove(psThis->m_pbyStorePtr, pbyData, cntByteCount);
  }
  psThis->m_pbyStorePtr += cntByteCount;
}
static void winusbcommstm32f4_Disconnect(SCommLayer *psCommLayer)
{
  //SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
}


static const ICommLayer sc_iCommLayerWinUSBCommSTM32F4 =
{
  COMM_EVENT_NULL,                         // ClientInit
  winusbcommstm32f4_HostInit,              // HostInit
  winusbcommstm32f4_CommGetBufferSize,     // CommGetBufferSize
  winusbcommstm32f4_PacketStart,           // PacketStart
  winusbcommstm32f4_Send,                  // Send
  winusbcommstm32f4_PacketEnd,             // PacketEnd
  winusbcommstm32f4_TransmitProcess,       // TransmitProcess
  winusbcommstm32f4_ReceiveProcess,        // ReceiveProcess
  winusbcommstm32f4_OnNewPacket,           // OnNewPacket
  COMM_TRANSFER_NULL,                      // OnData
  winusbcommstm32f4_Store,                 // Store
  COMM_EVENT_NULL,                         // OnPacketEnd
  winusbcommstm32f4_Disconnect             // Disconnect
};

void WinUSBCommSTM32F4_Init(SCommLayer *psCommLayer, SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4, void * pBuffer, unsigned long dwBufferSizeInBytes)
{
  psCommLayer->m_pLayerInstance = psWinUSBCommSTM32F4;
  psCommLayer->m_piCommLayer = &sc_iCommLayerWinUSBCommSTM32F4;
  psWinUSBCommSTM32F4->m_pbyBuffer = pBuffer;
  psWinUSBCommSTM32F4->m_dwBufferSizeInBytes = dwBufferSizeInBytes;
}
