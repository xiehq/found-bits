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

#ifndef __COMM_BASE_H__
#define __COMM_BASE_H__

#include "CommCore.h"

///
/// Base class for comm layer implementation.
///
/// Defines static function called from CommCore C code.
/// In each CommCore static function the provided void pointer
/// from parameters is casted to CCommLayerBase. Then corresponding
/// virtual method is invoked. Specific implementation can/must
/// override these as needed/required.
class CCommLayerBase
{
public:
  CCommLayerBase(SCommLayer *psCommLayer);
  virtual ~CCommLayerBase();
public:
  // Static functions - prototypes as defined in CommCore.h
  static void ClientInit(SCommLayer *psCommLayer);
  static void HostInit(SCommLayer *psCommLayer);
  static COMMCOUNT CommGetBufferSize(SCommLayer *psCommLayer, COMMCOUNT cntLowerLayerBufferSize);
  static void PacketStart(SCommLayer *psCommLayer);
  static void Send(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount);
  static void PacketEnd(SCommLayer *psCommLayer);
  static ECommStatus TransmitProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount, unsigned char **ppbyData);
  static ECommStatus ReceiveProcess(SCommLayer *psCommLayer, COMMCOUNT *pcntByteCount, unsigned char **ppbyReceivedData);
  static void OnNewPacket(SCommLayer *psCommLayer);
  static void OnData(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount);
  static void Store(SCommLayer *psCommLayer, const unsigned char *pbyData, COMMCOUNT cntByteCount);
  static void OnPacketEnd(SCommLayer *psCommLayer);
  static void Disconnect(SCommLayer *psCommLayer);

private:
  static ICommLayer sm_iCommLayer;  ///< Collection of static functions. Provided to CommCore during initialization.

protected:
  // Set of comm buffer maintenance methods.
  // For when this class is used to hold communication data.
  void Configure(BYTE *pbyBuff, COMMCOUNT cntBufferSize);
  BYTE *GetBuffer() const { return m_pbyBuff; }
  DWORD GetBufferSize() const { return m_cntBufferSize; }
  DWORD GetReceivedSize() const { return m_cntStored; }
  DWORD GetSendSize() const { return m_cntToSend; }
protected:
  // Overridable methods called from CommCore through static functions.
  virtual void ClientInit();
  virtual void HostInit();
  virtual COMMCOUNT CommGetBufferSize(COMMCOUNT cntLowerLayerBufferSize);
  virtual void PacketStart();
  virtual void Send(const unsigned char *pbyData, COMMCOUNT cntByteCount);
  virtual void PacketEnd();
  virtual ECommStatus TransmitProcess();
  virtual ECommStatus ReceiveProcess(COMMCOUNT *pcntByteCount, unsigned char **ppbyReceivedData);
  virtual void OnNewPacket();
  virtual void OnData(const unsigned char *pbyData, COMMCOUNT cntByteCount);
  virtual void Store(const unsigned char *pbyData, COMMCOUNT cntByteCount);
  virtual void OnPacketEnd();
  virtual void Disconnect();

  // Notifiers for CommCore events that need to be dispatched over an array of comm layers.
  void OnNewPacketNotify();
  void OnDataNotify(const unsigned char *pbyData, COMMCOUNT cntByteCount);
  void OnPacketEndNotify();

protected:
  SCommLayer *m_psCommLayer;  ///< Own comm layer instance with data connecting CommCore and layer instanace.

private:
  // Buffer maintenance data.
  // For when this class is used to hold communication data.
  BYTE *m_pbyBuff;            ///< Pointer to buffer as provided by application.
  COMMCOUNT m_cntBufferSize;  ///< Buffer size as given by application
  COMMCOUNT m_cntToSend;      ///< Internal counter of bytes to send.
  COMMCOUNT m_cntStored;      ///< internal counter of received bytes.
};

#endif  //  __COMM_BASE_H__
