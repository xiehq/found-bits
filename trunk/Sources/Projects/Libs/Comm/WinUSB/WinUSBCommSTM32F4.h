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

#ifndef __WINUSB_COMM_STM32F4_H__
#define __WINUSB_COMM_STM32F4_H__

#include "CommCore.h"
#include "WinUSBComm2Shared.h"

///< Type for WinUSBComm USB interface data transfer context
typedef struct _SWinUSBCommSTM32F4
{
  unsigned char *m_pbyBuffer;               ///< Pointer to comm buffer
  unsigned long m_dwBufferSizeInBytes;      ///< Size of comm buffer

  unsigned char *m_pbyStorePtr;             ///< Where to put incoming data in Store calls from upper layers while receiving
  unsigned char *m_pbyWritePtr;             ///< Where to put response data in Send calls from upper layers while processing

  unsigned long m_dwExpectedByteCountUSB;   ///< Current packet length
  unsigned char *m_pbyReceivePtrUSB;        ///< Current receive position

  unsigned long m_dwSendByteCountUSB;       ///< Size of packet to send
  unsigned char *m_pbySendPtrUSB;           ///< Current send position

  unsigned char m_byFlags;                  ///< Of EWinUSBCommSTM32F4Flags
  unsigned char m_byStateUSB;               ///< Of EWinUSBComm2State
}SWinUSBCommSTM32F4;

void WinUSBCommSTM32F4_Init(SCommLayer *psCommLayer, SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4, void * pBuffer, unsigned long dwBufferSizeInBytes);

#endif  //  __WINUSB_COMM_STM32F4_H__
