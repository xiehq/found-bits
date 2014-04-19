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

#include "stdafx.h"
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
  psThis->m_byPendingCommand = winusbcomm2commandNone;
  psThis->m_byState = winusbcomm2stateIdle;
  psThis->m_byFlags = wucstmflNone;
  psThis->m_dwExpectedByteCount = 0;
  psThis->m_dwReceivedByteCount = 0;
  psThis->m_dwSendByteCount = 0;
  psThis->m_dwSentByteCount = 0;
  psThis->m_pbyReceivePtr = psThis->m_pbyBuffer;
  psThis->m_pbySendPtr = psThis->m_pbyBuffer;
}
static COMMCOUNT winusbcommstm32f4_CommGetBufferSize(SCommLayer *psCommLayer, COMMCOUNT cntLowerLayerBufferSize)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  return psThis->m_dwBufferSizeInBytes - 4;
}
static void winusbcommstm32f4_PacketStart(SCommLayer *psCommLayer)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  psThis->m_dwSendByteCount = 0;
  psThis->m_dwSentByteCount = 0;
  psThis->m_pbySendPtr = psThis->m_pbyBuffer;
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
  if ( (psThis->m_dwSendByteCount + cntByteCount) > cntBufferSize )
  {
    psThis->m_byFlags |= wucstmflTxOverflow;
    return;
  }
  memmove(psThis->m_pbySendPtr, pbyData, cntByteCount);
  psThis->m_dwSendByteCount += cntByteCount;
}
static void winusbcommstm32f4_PacketEnd(SCommLayer *psCommLayer)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  if ( psThis->m_byFlags & wucstmflTxOverflow )
  {
    return;
  }
  DWORD2LSB(psThis->m_dwSendByteCount, psThis->m_pbyBuffer);
  psThis->m_dwSentByteCount = 0;
  psThis->m_byState = winusbcomm2stateSending;
  //USBD_LL_Transmit(&USBD_Device, WINUSBCOMM_EPIN_ADDR, USBD_Device.pClassData, WINUSBCOMM_MAX_FS_PACKET);
}
static ECommStatus winusbcommstm32f4_TransmitProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  if ( winusbcomm2stateSending != psThis->m_byState )
  {
    return commstatusIdle;
  }
  if ( psThis->m_dwSentByteCount < psThis->m_dwSendByteCount )
  {
    return commstatusActive;
  }
  psThis->m_byState = winusbcomm2stateIdle;
  return commstatusIdle;
}
static ECommStatus winusbcommstm32f4_ReceiveProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  if ( winusbcomm2stateReceiving != psThis->m_byState )
  {
    return commstatusIdle;
  }
  if ( psThis->m_dwExpectedByteCount == psThis->m_dwReceivedByteCount )
  {
    psThis->m_byState = winusbcomm2stateProcessing;
    return commstatusNewPacket;
  }
  return commstatusActive;
}
static void winusbcommstm32f4_OnData(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  if ( !psThis->m_dwExpectedByteCount )
  {
    psThis->m_dwExpectedByteCount = *(unsigned long *)pbyData;
    Comm_OnNewPacket(psCommLayer);
    COMM_ONDATA(pbyData + 4, cntByteCount - 4);
  }
  else
  {
    COMM_ONDATA(pbyData, cntByteCount);
  }
}
static void winusbcommstm32f4_Store(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  SWinUSBCommSTM32F4 *psThis = (SWinUSBCommSTM32F4 *)psCommLayer->m_pLayerInstance;
  memmove(psThis->m_pbyReceivePtr, pbyData, cntByteCount);
  psThis->m_pbyReceivePtr += cntByteCount;
  psThis->m_dwReceivedByteCount += cntByteCount;
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
  COMM_EVENT_NULL,                         // OnNewPacket
  winusbcommstm32f4_OnData,                // OnData
  winusbcommstm32f4_Store,                 // Store
  COMM_EVENT_NULL,                         // OnPacketEnd
  winusbcommstm32f4_Disconnect             // Disconnect
};

void WinUSBCommSTM32F4_Init(SCommLayer *psCommLayer, SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4, void * pBuffer, unsigned long dwBufferSizeInBytes)
{
  psCommLayer->m_pLayerInstance = psWinUSBCommSTM32F4;
  psCommLayer->m_piCommLayer = &sc_iCommLayerWinUSBCommSTM32F4;
}

//USBD_LL_PrepareReceive(&USBD_Device, WINUSBCOMM_EPOUT_ADDR, USBD_Device.pClassData, 0);
//USBD_LL_Transmit(&USBD_Device, WINUSBCOMM_EPIN_ADDR, USBD_Device.pClassData, WINUSBCOMM_MAX_FS_PACKET);
