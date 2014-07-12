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

#include "CommCore.h"

#ifndef NULL
#define NULL (void *)0
#endif

void Comm_PacketStart(SCommLayer *psCommLayer)
{
  if ( COMM_LOWER() )
  { Comm_PacketStart(COMM_LOWER()); }
  
  if ( psCommLayer->m_piCommLayer->m_pfnPacketStart )
  { psCommLayer->m_piCommLayer->m_pfnPacketStart(psCommLayer); }
}

void Comm_Send(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  if ( 0 == cntByteCount )
  { return; }
  if ( psCommLayer->m_piCommLayer->m_pfnSend )
  { psCommLayer->m_piCommLayer->m_pfnSend(psCommLayer, pbyData, cntByteCount); }
  else if ( COMM_LOWER() )
  { Comm_Send(COMM_LOWER(), pbyData, cntByteCount); }
}

void Comm_PacketEnd(SCommLayer *psCommLayer)
{
  if ( psCommLayer->m_piCommLayer->m_pfnPacketEnd )
  { psCommLayer->m_piCommLayer->m_pfnPacketEnd(psCommLayer); }

  if ( COMM_LOWER() )
  { Comm_PacketEnd(COMM_LOWER()); }
}


void Comm_OnNewPacket(SCommLayer *psCommLayer)
{
  if ( psCommLayer->m_piCommLayer->m_pfnOnNewPacket )
  { psCommLayer->m_piCommLayer->m_pfnOnNewPacket(psCommLayer); }

  if ( COMM_UPPER() )
  { Comm_OnNewPacket(COMM_UPPER()); }
}

void Comm_OnData(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  if ( 0 == cntByteCount )
  { return; }
  if ( psCommLayer->m_piCommLayer->m_pfnOnData )
  { psCommLayer->m_piCommLayer->m_pfnOnData(psCommLayer, pbyData, cntByteCount); }
  else if ( COMM_UPPER() )
  { Comm_OnData(COMM_UPPER(), pbyData, cntByteCount); }
  else if ( psCommLayer->m_piCommLayer->m_pfnStore )
  { psCommLayer->m_piCommLayer->m_pfnStore(psCommLayer, pbyData, cntByteCount); }
}

void Comm_Store(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  if ( 0 == cntByteCount )
  { return; }
  if ( psCommLayer->m_piCommLayer->m_pfnStore )
  { psCommLayer->m_piCommLayer->m_pfnStore(psCommLayer, pbyData, cntByteCount); }
  else if ( COMM_LOWER() )
  { Comm_Store(COMM_LOWER(), pbyData, cntByteCount); }
}

void Comm_OnPacketEnd(SCommLayer *psCommLayer)
{
  if ( psCommLayer->m_piCommLayer->m_pfnOnPacketEnd )
  { psCommLayer->m_piCommLayer->m_pfnOnPacketEnd(psCommLayer); }

  if ( COMM_UPPER() )
  { Comm_OnPacketEnd(COMM_UPPER()); }
}

ECommStatus Comm_TransmitProcess(SCommLayer *psCommLayer)
{
  unsigned char byStatus = commstatusIdle;
  if ( psCommLayer->m_piCommLayer->m_pfnTransmitProcess)
  {
    byStatus = psCommLayer->m_piCommLayer->m_pfnTransmitProcess(psCommLayer, (COMMCOUNT *)NULL, (unsigned char **)NULL);
  }
  if ( commstatusError == byStatus )
  {
    return commstatusError;
  }
  if ( COMM_LOWER() )
  {
    byStatus = Comm_TransmitProcess(COMM_LOWER());
  }
  return (ECommStatus)byStatus;
}

ECommStatus Comm_ReceiveProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntNumBytes, unsigned char **ppbyReceivedData)
{
  unsigned char byStatus = commstatusIdle;
  *pcntNumBytes = 0;
  if ( COMM_LOWER() )
  {
    byStatus = Comm_ReceiveProcess(COMM_LOWER(), pcntNumBytes, ppbyReceivedData);
  }
  if ( commstatusError == byStatus )
  {
    return commstatusError;
  }
  if ( psCommLayer->m_piCommLayer->m_pfnReceiveProcess)
  {
    byStatus = psCommLayer->m_piCommLayer->m_pfnReceiveProcess(psCommLayer, pcntNumBytes, ppbyReceivedData);
  }
  return (ECommStatus)byStatus;
}

COMMCOUNT Comm_GetBufferSize(SCommLayer *psCommLayer)
{
  COMMCOUNT cntBufferSize = 0;
  if ( COMM_LOWER() )
  { cntBufferSize = Comm_GetBufferSize(COMM_LOWER()); }
  
  if ( psCommLayer->m_piCommLayer->m_pfnCommGetBufferSize )
  { cntBufferSize = psCommLayer->m_piCommLayer->m_pfnCommGetBufferSize(psCommLayer, cntBufferSize); }
  return cntBufferSize;
}

void Comm_Disconnect(SCommLayer *psCommLayer)
{
  if ( psCommLayer->m_piCommLayer->m_pfnDisconnect )
  { psCommLayer->m_piCommLayer->m_pfnDisconnect(psCommLayer); }

  if ( COMM_LOWER() )
  { Comm_Disconnect(COMM_LOWER()); }
}

HCOMMSTACK CommStack_Init(unsigned char byFlags, SCommStack * psCommStack, SCommLayer *psCommLayers, COMMCOUNT cntLayerCount)
{
  COMMCOUNT cntLayerIndex;
  SCommLayer *psCurrentLayer = (SCommLayer *)NULL;
  pfnCommEvent pfnInit = COMM_EVENT_NULL;
  HCOMMSTACK hComm = psCommStack;

  hComm->m_cntNumberOfLayers = cntLayerCount;
  hComm->m_byFlags = byFlags;
  hComm->m_byLastError = 0;

  for ( cntLayerIndex = 0; cntLayerIndex < cntLayerCount; cntLayerIndex++ )
  {
    psCommLayers[cntLayerIndex].m_hCommStack = hComm;
    psCommLayers[cntLayerIndex].m_psLowerLayer = psCurrentLayer;
    psCommLayers[cntLayerIndex].m_psUpperLayer = &psCommLayers[cntLayerIndex + 1];
    psCurrentLayer = &psCommLayers[cntLayerIndex];
  }

  psCurrentLayer->m_psUpperLayer = (SCommLayer *)NULL;
  hComm->m_psCommLayer = psCurrentLayer;

  for ( cntLayerIndex = 0; cntLayerIndex < cntLayerCount; cntLayerIndex++ )
  {
    if ( commstackflSideClient == ( byFlags & commstackflSideMask ) )
    {
      pfnInit = psCurrentLayer->m_piCommLayer->m_pfnClientInit;
    }
    else
    {
      pfnInit = psCurrentLayer->m_piCommLayer->m_pfnHostInit;
    }
    if ( pfnInit )
    {
      pfnInit(psCurrentLayer);
    }
  }

  return hComm;
}



void CommStack_PacketStart(HCOMMSTACK hCommStack)
{
  Comm_PacketStart(hCommStack->m_psCommLayer);
}

void CommStack_Send(HCOMMSTACK hCommStack, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  Comm_Send(hCommStack->m_psCommLayer, pbyData, cntByteCount);
}
void CommStack_PacketEnd(HCOMMSTACK hCommStack)
{
  Comm_PacketEnd(hCommStack->m_psCommLayer);
}

void CommStack_Disconnect(HCOMMSTACK hCommStack)
{
  Comm_Disconnect(hCommStack->m_psCommLayer);
}

ECommStatus CommStack_TransmitProcess(HCOMMSTACK hCommStack)
{
  unsigned char byStatus = Comm_TransmitProcess(hCommStack->m_psCommLayer);
  return (ECommStatus)byStatus;
}

ECommStatus CommStack_ReceiveProcess(HCOMMSTACK hCommStack, COMMCOUNT *pcntNumBytes, unsigned char **ppbyReceivedData)
{
  unsigned char byStatus = Comm_ReceiveProcess(hCommStack->m_psCommLayer, pcntNumBytes, ppbyReceivedData);
  return (ECommStatus)byStatus;
}


COMMCOUNT CommStack_GetBufferSize(HCOMMSTACK hCommStack)
{
  return Comm_GetBufferSize(hCommStack->m_psCommLayer);
}

