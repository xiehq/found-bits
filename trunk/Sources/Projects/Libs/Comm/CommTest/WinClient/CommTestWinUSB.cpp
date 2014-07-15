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

#include "StdAfx.h"
#include "CommTestWinUSB.h"

#include "WinUSBCommDevice.h"

class CCommTestWinUSBImpl : public ICommTest
{
public:
  CCommTestWinUSBImpl(void) { };
  virtual ~CCommTestWinUSBImpl(void) { };

public:
  // ICommTest impl
  virtual HCOMMSTACK Connect(const string &rstrConnectionName);
  virtual void Disonnect(HCOMMSTACK hComm);
  virtual void List(vector<string> &rvstrAvailableConnections);

private:
  CWinUSBCommDevice m_USBDev;
};

HCOMMSTACK CCommTestWinUSBImpl::Connect(const string &rstrConnectionName)
{
  return NULL;
}
void CCommTestWinUSBImpl::Disonnect(HCOMMSTACK hComm)
{

}

void CCommTestWinUSBImpl::List(vector<string> &rvstrAvailableConnections)
{
  //CWinUSBCommDevice::ListDevices(rvstrAvailableConnections);
}

CCommTestWinUSB::CCommTestWinUSB(void) :
  m_pImpl(NULL)
{
  m_pImpl = new CCommTestWinUSBImpl();
  g_theCommManager.RegisterComm(this, "WinUSB");
}


CCommTestWinUSB::~CCommTestWinUSB(void)
{
  if ( m_pImpl )
  {
    delete m_pImpl;
  }
}

HCOMMSTACK CCommTestWinUSB::Connect(const string &rstrConnectionName)
{
  return m_pImpl->Connect(rstrConnectionName);
}
void CCommTestWinUSB::Disonnect(HCOMMSTACK hComm)
{
  m_pImpl->Disonnect(hComm);
}
void CCommTestWinUSB::List(vector<string> &rvstrAvailableConnections)
{
  m_pImpl->List(rvstrAvailableConnections);
}
