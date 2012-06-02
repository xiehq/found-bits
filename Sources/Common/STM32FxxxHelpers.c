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

#include "stm32f10x.h"
#include "STM32FxxxHelpers.h"

void STM32_GetSerial(uint32_t *pdwDevice_Serial2, uint32_t *pdwDevice_Serial1, uint32_t *pdwDevice_Serial0)
{
#ifdef STM32L1XX_MD
  *pdwDevice_Serial0 = *(uint32_t*)(0x1FF80050);
  *pdwDevice_Serial1 = *(uint32_t*)(0x1FF80054);
  *pdwDevice_Serial2 = *(uint32_t*)(0x1FF80064);
#else
  *pdwDevice_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  *pdwDevice_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  *pdwDevice_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);
#endif /* STM32L1XX_MD */
}

void STM32_RCCPeriphClockCmd(uint32_t RCC_Periph, FunctionalState NewState)
{
  if ( IS_RCC_APB2_PERIPH(RCC_Periph) )
  { RCC_APB2PeriphClockCmd(RCC_Periph, NewState); }
  else if ( IS_RCC_APB1_PERIPH(RCC_Periph) )
  { RCC_APB1PeriphClockCmd(RCC_Periph, NewState); }
  else if ( IS_RCC_AHB_PERIPH(RCC_Periph) )
  { RCC_AHBPeriphClockCmd(RCC_Periph, NewState); }
}
