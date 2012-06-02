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
#include "STM32GPIO.h"
#include "STM32FxxxHelpers.h"

static const SSTM32GPIO *s_pSSTM32GPIO = NULL;
static DWORD s_dwGPIOCount = 0;

#define PORT(i) s_pSSTM32GPIO[i].m_psGPIOPort
#define PIN(i) s_pSSTM32GPIO[i].m_sGPIOInit.GPIO_Pin
#define PORT_AND_PIN(i) PORT(i), PIN(i)
#define ACTIVE(i) s_pSSTM32GPIO[i].m_eActiveState
#define INACTIVE(i) s_pSSTM32GPIO[i].m_eInactiveState

void stm32gpio_SetState(DWORD dwGPIOIndex, EGPIOState eState)
{
  GPIO_InitTypeDef GPIOInit = s_pSSTM32GPIO[dwGPIOIndex].m_sGPIOInit;
  ASSERT ( dwGPIOIndex < s_dwGPIOCount );

  switch ( eState )
  {
  case gpioLow:
    GPIO_ResetBits(PORT_AND_PIN(dwGPIOIndex));
    if ( GPIO_Mode_Out_OD != GPIOInit.GPIO_Mode )
    { GPIOInit.GPIO_Mode = GPIO_Mode_Out_PP; }
    GPIO_Init(PORT(dwGPIOIndex), &GPIOInit);
    break;
  case gpioHigh:
    GPIO_SetBits(PORT_AND_PIN(dwGPIOIndex));
    if ( GPIO_Mode_Out_OD != GPIOInit.GPIO_Mode )
    { GPIOInit.GPIO_Mode = GPIO_Mode_Out_PP; }
    GPIO_Init(PORT(dwGPIOIndex), &GPIOInit);
    break;
  case gpioZ:
    if ( GPIO_Mode_Out_OD != GPIOInit.GPIO_Mode )
    { GPIOInit.GPIO_Mode = GPIO_Mode_IN_FLOATING; }
    GPIO_Init(PORT(dwGPIOIndex), &GPIOInit);
    break;
  case gpioPullDown:
    ASSERT ( GPIO_Mode_Out_OD != GPIOInit.GPIO_Mode ) //  really? you want this?
    GPIOInit.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(PORT(dwGPIOIndex), &GPIOInit);
    GPIO_ResetBits(PORT_AND_PIN(dwGPIOIndex));
    break;
  case gpioPullUp:
    if ( GPIO_Mode_Out_OD != GPIOInit.GPIO_Mode )
    { GPIOInit.GPIO_Mode = GPIO_Mode_IPU; }
    GPIO_Init(PORT(dwGPIOIndex), &GPIOInit);
    GPIO_SetBits(PORT_AND_PIN(dwGPIOIndex));
    break;
  default:
    ASSERT ( 0 ); break;
  }
}

void STM32GPIO_Init(const SSTM32GPIO *pSSTM32GPIO, DWORD dwCount)
{
  GPIO_InitTypeDef sGPIOInit;
  DWORD I = 0;
  s_pSSTM32GPIO = pSSTM32GPIO;
  s_dwGPIOCount = dwCount;
  for ( I = 0; I < s_dwGPIOCount; I++ )
  {
    STM32_RCCPeriphClockCmd(pSSTM32GPIO[I].m_dwGPIOClk, ENABLE);
    switch ( INACTIVE(I) )
    {
    case gpioLow: GPIO_ResetBits(PORT_AND_PIN(I)); break;
    case gpioHigh: GPIO_SetBits(PORT_AND_PIN(I)); break;
    case gpioZ: break;
    case gpioPullDown: GPIO_ResetBits(PORT_AND_PIN(I)); break;
    case gpioPullUp: GPIO_SetBits(PORT_AND_PIN(I)); break;
    default: ASSERT ( 0 ); break;
    }
    sGPIOInit = pSSTM32GPIO[I].m_sGPIOInit;
    GPIO_Init(PORT(I), &sGPIOInit);
  }
}

void STM32GPIO_Activate(DWORD dwGPIOIndex)
{
  stm32gpio_SetState(dwGPIOIndex, ACTIVE(dwGPIOIndex));
}

void STM32GPIO_Deactivate(DWORD dwGPIOIndex)
{
  stm32gpio_SetState(dwGPIOIndex, INACTIVE(dwGPIOIndex));
}

void STM32GPIO_SetActive(DWORD dwGPIOIndex, BOOL bActive)
{
  if ( bActive )
  { STM32GPIO_Activate(dwGPIOIndex); }
  else
  { STM32GPIO_Deactivate(dwGPIOIndex); }
}

BOOL STM32GPIO_IsActive(DWORD dwGPIOIndex)
{
  BitAction bitState;
  ASSERT ( dwGPIOIndex < s_dwGPIOCount );
  bitState = GPIO_ReadInputDataBit(PORT_AND_PIN(dwGPIOIndex));
  switch ( ACTIVE(dwGPIOIndex) )
  {
  case gpioLow:
  case gpioPullDown:
    return Bit_RESET == bitState;
  case gpioHigh:
  case gpioPullUp:
    return Bit_SET == bitState;
  case gpioZ:
  default:
    switch ( INACTIVE(dwGPIOIndex) )
    {
    case gpioLow:
    case gpioPullDown:
      return Bit_SET == bitState;
    case gpioHigh:
    case gpioPullUp:
      return Bit_RESET == bitState;
    case gpioZ:
    default:
      ASSERT(0); break; // both unknown or Hi-Z
    }
    break;
  }
  return FALSE;
}
