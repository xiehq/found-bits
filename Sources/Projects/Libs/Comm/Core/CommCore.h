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

#ifndef __COMM_CORE_H__
#define __COMM_CORE_H__

#include "Defs.h"

#ifdef __cplusplus
 extern "C" {
#endif

struct _SCommLayer;
typedef struct _SCommLayer SCommLayer;

struct _SCommStack;
typedef struct _SCommStack SCommStack;

#ifndef COMMCOUNT
typedef unsigned short COMMCOUNT;
#endif
#ifndef COMMINDEX
typedef unsigned short COMMINDEX;
#endif

typedef enum _ECommStatus
{
  commstatusIdle,       ///< No communication
  commstatusActive,     ///< Communication is ongoing
  commstatusNewPacket,  ///< New packet received
  commstatusErrorMask = 0x80,
  commstatusError,      ///< Internal error
  commstatusOverrun,    ///< Trying to send or receive too much data
  commstatusDisconnect, ///< Communication disconnected
}ECommStatus;

typedef void (*pfnCommEvent)(SCommLayer *psCommLayer);  ///< Layer event notification (init, packet start/end, disconnect etc.)
typedef void (*pfnCommTransfer)(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount); ///< Data transfer (data received, store, send)
typedef ECommStatus (*pfnCommProcess)(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount, unsigned char **ppbyReceivedData); ///< Comm process (receive, transmit)

typedef COMMCOUNT (*pfnCommGetBufferSize)(SCommLayer *psCommLayer, COMMCOUNT cntLowerLayerBufferSize);

typedef struct _ICommLayer
{
  pfnCommEvent m_pfnClientInit;                 ///< Client mode layer initialization and connect. Communication allowed.
  pfnCommEvent m_pfnHostInit;                   ///< Host mode layer initialization and start listening.

  pfnCommGetBufferSize m_pfnCommGetBufferSize;  ///< Called from application or upper layer to get max comm buffer length in bytes.
                                                ///< First same call to lower layer is made.
                                                ///< Then size from lower layers is available in this call where it can be adjusted and returned.

  pfnCommEvent m_pfnPacketStart;                ///< Called from application or upper layer on new packet transmission after same call to lower layer.
                                                ///< Same call to lower layer is made by framework regardless if layer implements this call.
  pfnCommTransfer m_pfnSend;                    ///< Called from application or upper layer to send data.
                                                ///< Nothing should be sent on physical layer until PacketEnd call.
                                                ///< If layer implements this call it must also make this call to lower layer.
                                                ///< Layer can optionally modify, append, encode etc. data here before forwarding call to lower layer.
                                                ///< In lowest layer the data is stored in communication buffer as it should be sent on PacketEnd.
  pfnCommEvent m_pfnPacketEnd;                  ///< Called from application or upper layer after all data for current packet was provided and before same call to lower layer.
                                                ///< Same call to lower layer is made by framework regardless if layer implements this call.

  pfnCommProcess m_pfnTransmitProcess;          ///< Repeatedly called from application or upper layer to determine when and if packet is successfully sent.
                                                ///< Layer should do transmit work in this call. This shouldn't be a blocking call.
                                                ///< Return count holds sent byte count until this call.
                                                ///< Total bytes send in return count can differ from actual packet size.

  pfnCommProcess m_pfnReceiveProcess;           ///< Repeatedly called from application or upper layer to determine if new packet was received.
                                                ///< Layer should do receiving work in this call. It must call OnNewPacket, OnData and OnPacketEnd.
                                                ///< This shouldn't be a blocking call.
                                                ///< Return count holds the byte count of data stored in receive buffer until this call.

  pfnCommEvent m_pfnOnNewPacket;                ///< Called from lower layer when the start of new packet received before same call to upper layer.
                                                ///< Same call to upper layer is made by framework regardless if layer implements this call.
  pfnCommTransfer m_pfnOnData;                  ///< Called from lower layer when new data received.
                                                ///< If layer implements this call it must also make this call to upper layer.
                                                ///< Layer can optionally modify, strip, decode etc. data here before forwarding call to upper layer.
                                                ///< If top most layer doesn't implement this call Store is called on stack.
  pfnCommTransfer m_pfnStore;                   ///< OnData callback to lower layer to store the data in receive buffer.
                                                ///< If layer implements this call it must also make this call to lower layer.
  pfnCommEvent m_pfnOnPacketEnd;                ///< Called from lower layer when complete packet received before same call to upper level.
                                                ///< Same call to upper layer is made by framework regardless if layer implements this call.
  
  pfnCommEvent m_pfnDisconnect;                 ///< Called from application or upper layer to disconnect before same call to lower level.
                                                ///< Same call to lower layer is made by framework regardless if layer implements this call.
}ICommLayer;

/*
layerImpl_ClientInit,            // ClientInit
layerImpl_HostInit,              // HostInit
layerImpl_CommGetBufferSize,     // CommGetBufferSize
layerImpl_PacketStart,           // PacketStart
layerImpl_Send,                  // Send
layerImpl_PacketEnd,             // PacketEnd
layerImpl_TransmitProcess,       // TransmitProcess
layerImpl_ReceiveProcess,        // ReceiveProcess
layerImpl_OnNewPacket,           // OnNewPacket
layerImpl_OnData,                // OnData
layerImpl_Store,                 // Store
layerImpl_OnPacketEnd,           // OnPacketEnd
layerImpl_Disconnect             // Disconnect
*/

#define COMM_EVENT_NULL         (pfnCommEvent)NULL
#define COMM_TRANSFER_NULL      (pfnCommTransfer)NULL
#define COMM_PROCESS_NULL       (pfnCommProcess)NULL
#define COMM_GETBUFFERSIZE_NULL (pfnCommGetBufferSize)NULL


typedef SCommStack * HCOMMSTACK;

struct _SCommLayer
{
  void *m_pLayerInstance;                     ///< "THIS" pointer.
  const ICommLayer *m_piCommLayer;            ///< Pointer to comm interface implementation.
  SCommLayer *m_psUpperLayer;                 ///< Pointer to upper comm layer.
  SCommLayer *m_psLowerLayer;                 ///< Pointer to lower comm layer.
  HCOMMSTACK m_hCommStack;                    ///< Comm stack handle to which layer belongs.
};

enum ECommStackFlags
{
  commstackflSideClient = 0x00, ///< Stack on client side.
  commstackflSideHost   = 0x01, ///< Stack on host side.
  commstackflSideMask   = 0x01,
};

struct _SCommStack
{
  SCommLayer *m_psCommLayer;      ///< Pointer to array of comm layers. Lowest layer (physical) at index 0.
  COMMCOUNT m_cntNumberOfLayers;  ///< Number of layers in array.
  unsigned char m_byFlags;        ///< Of ECommStackFlags.
  unsigned char m_byLastError;
};

void Comm_PacketStart(SCommLayer *psCommLayer);
void Comm_Send(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount);
void Comm_PacketEnd(SCommLayer *psCommLayer);

void Comm_OnNewPacket(SCommLayer *psCommLayer);
void Comm_OnData(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount);
void Comm_Store(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount);
void Comm_OnPacketEnd(SCommLayer *psCommLayer);
COMMCOUNT Comm_GetBufferSize(SCommLayer *psCommLayer);

HCOMMSTACK CommStack_Init(unsigned char byFlags, SCommStack * psCommStack, SCommLayer *psCommLayers, COMMCOUNT cntLayerCount);
void CommStack_PacketStart(HCOMMSTACK hCommStack);
void CommStack_Send(HCOMMSTACK hCommStack, const unsigned char *pbyData, COMMCOUNT cntByteCount);
void CommStack_SendCallback(void *pCallbackObject, const unsigned char *pbyData, COMMCOUNT cntByteCount);
void CommStack_PacketEnd(HCOMMSTACK hCommStack);
ECommStatus CommStack_TransmitProcess(HCOMMSTACK hCommStack);
ECommStatus CommStack_ReceiveProcess(HCOMMSTACK hCommStack, COMMCOUNT *pcntNumBytes, unsigned char **ppbyReceivedData);
COMMCOUNT CommStack_GetBufferSize(HCOMMSTACK hCommStack);
void CommStack_Disconnect(HCOMMSTACK hCommStack);

#define COMM_THIS() psCommLayer->m_pLayerInstance
#define COMM_UPPER() psCommLayer->m_psUpperLayer
#define COMM_LOWER() psCommLayer->m_psLowerLayer
#define COMM_ONDATA(pbyData, cntByteCount) { if ( COMM_UPPER() ) { Comm_OnData(COMM_UPPER(), pbyData, cntByteCount); } else { Comm_Store(psCommLayer, pbyData, cntByteCount); } }

#ifdef __cplusplus
}
#endif


#endif  //  __COMM_CORE_H__
