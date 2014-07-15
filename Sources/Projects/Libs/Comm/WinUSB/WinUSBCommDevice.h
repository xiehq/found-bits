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

#pragma once

#include <string>
#include <vector>
#include "WinUSBWrapper.h"

class CWinUSBCommDevice
{
public:
  CWinUSBCommDevice(void);
  ~CWinUSBCommDevice(void);

// #ifdef UNICODE
//   typedef std::wstring string;
// #else
  typedef std::string string;
// #endif

  typedef std::vector<string> TStringList;
  static BOOL ListDevices(TStringList &rNamesList, WORD wVID = 0, WORD wPID = 0);
  BOOL IsConnected() const;
  BOOL Connect(LPCSTR pcszDevice);  ///< Device name from ListDevices
  void Disconnect();

  BOOL SendData(BYTE *pbyBuffer, DWORD dwBufferSizeInBytes);
  BOOL CanReceive(DWORD &rdwNumBytes);
  BOOL DoReceive(unsigned char *pbyData, DWORD dwNumBytes);

  DWORD GetMaxBuffer() const { return m_dwCommBufferSizeInBytes; }
private:
  static BOOL listDevices(TStringList &rNamesList, TStringList &rPathsList, WORD wVID = 0, WORD wPID = 0);
  static BOOL getPath(LPCSTR pcszDevice, string &rstrPath);

  BOOL queryDeviceEndpoints();
  BOOL controlWrite(BYTE byWinUSBCommControl, BYTE *pbyData = NULL, WORD wNumBytesCount = 0);
  BOOL controlRead(BYTE byWinUSBCommControl, BYTE *pbyData, WORD wNumBytesCount);

  BOOL bulkWrite(BYTE *pbyData, DWORD dwNumBytesCount);
  BOOL bulkRead(BYTE *pbyData, DWORD dwNumBytesCount);

  BOOL reset();
  BYTE readStatus();
  BOOL getResponseLength(DWORD &rdwNumBytes);

  BOOL doSend(BYTE *pbyData, DWORD dwNumBytesCount);

private:
  static GUID sm_WinUSBCommInterfaceGUID;

  HANDLE m_hDeviceHandle;
  WINUSB_INTERFACE_HANDLE m_hWinUSBHandle;
  UCHAR  m_byPipeInId;
  UCHAR  m_byPipeOutId;
  DWORD m_dwCommBufferSizeInBytes;

  BYTE m_byDeviceVersion;

  BYTE m_bycommandReset;
  BYTE m_bycommandGetState;
  BYTE m_bycommandGetBufferSize;
  BYTE m_bycommandGetReturnSize;
};

