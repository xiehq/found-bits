/////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, pa.eeapai@gmail.com                                         //
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

#ifndef __COMMON_DEFS_H__
#define __COMMON_DEFS_H__

#ifndef _countof
#define _countof(a) (sizeof(a)/sizeof(a[0]))
#endif

#ifndef _PtrAndCnt
#define _PtrAndCnt(a) &(a[0]), _countof(a)
#endif

#ifndef _CntAndPtr
#define _CntAndPtr(a)  _countof(a), &(a[0])
#endif

#ifndef COMPILETIME_ASSERT
#define COMPILETIME_ASSERT(e) extern char _cta[(e) ? 1 : -1]
#endif

#define ABS(X)  ((X) > 0 ? (X) : -(X))
#define MAX(X,Y)  ((X) > (Y) ? (X) : (Y))
#define MIN(X,Y)  ((X) > (Y) ? (Y) : (X))

#define BYTE0(v) ((BYTE)(v))
#define BYTE1(v) ((BYTE)((v) >> 8))
#define BYTE2(v) ((BYTE)((v) >> 16))
#define BYTE3(v) ((BYTE)((v) >> 24))

#endif  //  __COMMON_DEFS_H__
