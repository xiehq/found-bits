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

#include "Conf.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "WinUSBComm.h"

int main(void)
{
  SystemCoreClockUpdate();
  SysTick_Config(SystemCoreClock / 1000);

  Set_System();
  Set_USBClock();
  Led_Config();

  USB_Interrupts_Config();
  USB_Init();
  while (bDeviceState != CONFIGURED);

  USB_Configured_LED();

  while ( 1 )
  {
    WinUSBComm_ProcessNewData();
  }

  return 0;
}

void WinUSBComm_OnNewData()
{
  uint32_t I;
  uint32_t dwNumBytesReceived = WinUSBComm_GetAvailableByteCount();
  uint8_t * pbyBuff = WinUSBComm_ReadData(dwNumBytesReceived);
  WinUSBComm_WriteData(pbyBuff, dwNumBytesReceived);
  return;
  if ( NULL == pbyBuff )
    return;
  for ( I = 0; I < dwNumBytesReceived; I++ )
  {
    pbyBuff[I] = pbyBuff[I] ^ 0xFF;
  }
  WinUSBComm_WriteData(pbyBuff, dwNumBytesReceived);
}
