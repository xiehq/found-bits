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

#ifndef __MONITOR_H__
#define __MONITOR_H__

enum EState
{
  stateIDLE,
  stateACTIVE,
  stateERROR
};

enum EMonResult
{
  monOK,
  monERROR = stateERROR,
};

enum EMonitorHeaderField
{
  monhMagic,
  monhLoadAddress,
  monhEntry,
  monhBufferSize,
  monhBufferAddress,
  monhMaxFileNameLength,
  monhFileNameAddressA,
  monhFileNameAddressB,
  monhStatusA,
  monhStatusB,
  monhOpA,
  monhOpB,
  monhParamA0,
  monhParamB0,
  monhParamA1,
  monhParamB1,
  monhParamA2,
  monhParamB2,
  monhParamA3,
  monhParamB3,
};

enum EMonitorOperation
{
  opInit,
  opScan,
  opStartSL,
  opCreateFile,
  opCloseFile,
  opWriteFile,
  opStopSL,
  opDeinit
};

enum EMonOpFlags
{
  scanCSEnableBefore  = 0x00000001,  ///< CS Low before scan
  scanCSDisableAfter  = 0x00000002,  ///< CS High after scan
};
void MonitorLoop();

#endif  //  __MONITOR_H__
