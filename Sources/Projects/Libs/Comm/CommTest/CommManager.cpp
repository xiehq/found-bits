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
#include "CommManager.h"

/*
Layer implementations should have own sub folder in
Projects\Libs\Comm folder named by technology used.
Names of source files with ICommLayer implementation should have "Comm"
prefix.

New comm stacks for test can be added by implementing new instance of
ICommTest and registering it with the g_theCommManager.
Stack instance and registration should be implemented within CommTest
project in new source file. Prepend "CommTest" to source file(s)
registering comm stack to test.
Define new class derived from ICommTest and make static instance of it.
Register instance with g_theCommManager within the constructor.
*/

CCommManager g_theCommManager;


CCommManager::CCommManager(void)
{
}


CCommManager::~CCommManager(void)
{
}


void CCommManager::RegisterComm(ICommTest *piCommTest, LPCSTR pcszName)
{
  string strCommName = pcszName;
  m_RegisteredCommNames.push_back(strCommName);
  m_RegisteredCommCallbacks.push_back(piCommTest);
}

void CCommManager::List(TNames &rConnectionNames)
{
  rConnectionNames.clear();
  for ( DWORD dwRegisteredComm = 0; dwRegisteredComm < m_RegisteredCommNames.size(); ++dwRegisteredComm )
  {
    ICommTest *piCommTest = m_RegisteredCommCallbacks[dwRegisteredComm];
    piCommTest->List(rConnectionNames);
    for ( DWORD dwConnectionIndex = 0; dwConnectionIndex < rConnectionNames.size(); ++dwConnectionIndex )
    {
      string strConnection = m_RegisteredCommNames[dwRegisteredComm] + " : " + rConnectionNames[dwConnectionIndex];
      rConnectionNames.push_back(strConnection);
    }
  }
}

HCOMMSTACK CCommManager::Connect(LPCSTR pcszConnectionName)
{
  if ( !pcszConnectionName )
  {
    return NULL;
  }

  HCOMMSTACK hComm = NULL;
  string strConnectionName = pcszConnectionName;
  string strComm = strConnectionName.substr(0, strConnectionName.find(" : "));
  string strConnection = strConnectionName.substr(strComm.length() + 3);

  for ( DWORD dwRegisteredComm = 0; dwRegisteredComm < m_RegisteredCommNames.size(); ++dwRegisteredComm )
  {
    if ( m_RegisteredCommNames[dwRegisteredComm] == strComm )
    {
      hComm = m_RegisteredCommCallbacks[dwRegisteredComm]->Connect(strConnection.c_str());
      break;
    }
  }

  return hComm;
}
