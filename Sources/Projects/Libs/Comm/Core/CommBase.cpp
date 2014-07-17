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
#include "CommBase.h"

ICommLayer CCommLayerBase::sm_iCommLayer =
{
  CCommLayerBase::ClientInit,
  CCommLayerBase::HostInit,
  CCommLayerBase::CommGetBufferSize,
  CCommLayerBase::PacketStart,
  CCommLayerBase::Send,
  CCommLayerBase::PacketEnd,
  CCommLayerBase::TransmitProcess,
  CCommLayerBase::ReceiveProcess,
  CCommLayerBase::OnNewPacket,
  CCommLayerBase::OnData,
  CCommLayerBase::Store,
  CCommLayerBase::OnPacketEnd,
  CCommLayerBase::Disconnect
};

CCommLayerBase::CCommLayerBase(SCommLayer *psCommLayer) :
  m_pbyBuff(NULL),
  m_cntBufferSize(0),
  m_cntToSend(0),
  m_cntStored(0),
  m_psCommLayer(psCommLayer)
{
  psCommLayer->m_pLayerInstance = this;
  psCommLayer->m_piCommLayer = &sm_iCommLayer;
}
CCommLayerBase::~CCommLayerBase()
{
}

void CCommLayerBase::Configure(BYTE *pbyBuff, COMMCOUNT cntBufferSize)
{
  m_pbyBuff = pbyBuff;
  m_cntBufferSize = cntBufferSize;
}

void CCommLayerBase::ClientInit(SCommLayer *psCommLayer)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->ClientInit();
}
void CCommLayerBase::HostInit(SCommLayer *psCommLayer)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->HostInit();
}
COMMCOUNT CCommLayerBase::CommGetBufferSize(SCommLayer *psCommLayer, COMMCOUNT cntLowerLayerBufferSize)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  return pThis->CommGetBufferSize(cntLowerLayerBufferSize);
}
void CCommLayerBase::PacketStart(SCommLayer *psCommLayer)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->PacketStart();
}
void CCommLayerBase::Send(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->Send(pbyData, cntByteCount);
}
void CCommLayerBase::PacketEnd(SCommLayer *psCommLayer)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->PacketEnd();
}
ECommStatus CCommLayerBase::TransmitProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount, unsigned char **ppbyData)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  return pThis->TransmitProcess();
}
ECommStatus CCommLayerBase::ReceiveProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount, unsigned char **ppbyReceivedData)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  return pThis->ReceiveProcess(pcntByteCount, ppbyReceivedData);
}
void CCommLayerBase::OnNewPacket(SCommLayer *psCommLayer)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->OnNewPacket();
}
void CCommLayerBase::OnData(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->OnData(pbyData, cntByteCount);
}
void CCommLayerBase::Store(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->Store(pbyData, cntByteCount);
}
void CCommLayerBase::OnPacketEnd(SCommLayer *psCommLayer)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->OnPacketEnd();
}
void CCommLayerBase::Disconnect(SCommLayer *psCommLayer)
{
  CCommLayerBase *pThis = (CCommLayerBase *)psCommLayer->m_pLayerInstance;
  pThis->Disconnect();
}

// Default impl. Should be implemented in derived class.
void CCommLayerBase::ClientInit()
{
  m_cntToSend = 0;
  m_cntStored = 0;
}
void CCommLayerBase::HostInit()
{
}
COMMCOUNT CCommLayerBase::CommGetBufferSize(COMMCOUNT cntLowerLayerBufferSize)
{
  return m_cntBufferSize;
}
void CCommLayerBase::PacketStart()
{
  m_cntToSend = 0;
}
void CCommLayerBase::Send(const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  if ( (cntByteCount + m_cntToSend) > m_cntBufferSize )
  {
    m_psCommLayer->m_hCommStack->m_byLastError = commstatusOverrun;
    return;
  }
  memmove(&m_pbyBuff[m_cntToSend], pbyData, cntByteCount);
  m_cntToSend += cntByteCount;
}
void CCommLayerBase::PacketEnd()
{
}
ECommStatus CCommLayerBase::TransmitProcess()
{
  return commstatusIdle;
}
ECommStatus CCommLayerBase::ReceiveProcess(COMMCOUNT *pcntByteCount, unsigned char **ppbyReceivedData)
{
  return commstatusIdle;
}
void CCommLayerBase::OnNewPacket()
{
  m_cntStored = 0;
}
void CCommLayerBase::OnData(const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  if ( m_psCommLayer->m_psUpperLayer ) 
  {
    Comm_OnData(m_psCommLayer->m_psUpperLayer, pbyData, cntByteCount);
  }
  else
  {
    Store(pbyData, cntByteCount);
  }
}
void CCommLayerBase::Store(const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  if ( (cntByteCount + m_cntStored) > m_cntBufferSize )
  {
    m_psCommLayer->m_hCommStack->m_byLastError = commstatusOverrun;
    return;
  }
  memmove(&m_pbyBuff[m_cntStored], pbyData, cntByteCount);
  m_cntStored += cntByteCount;
}
void CCommLayerBase::OnPacketEnd()
{
}
void CCommLayerBase::Disconnect()
{
}

void CCommLayerBase::OnNewPacketNotify()
{
  Comm_OnNewPacket(m_psCommLayer);
}
void CCommLayerBase::OnDataNotify(const unsigned char *pbyData, COMMCOUNT cntByteCount)
{
  Comm_OnData(m_psCommLayer, pbyData, cntByteCount);
}
void CCommLayerBase::OnPacketEndNotify()
{
  Comm_OnPacketEnd(m_psCommLayer);
}

