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

#include <stdlib.h>
#include <string.h>

#include "defs.h"
#include "simplelink.h"
#include "netcfg.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "utils.h"
#include "pin.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "uart.h"

#include "monitor.h"

#include <stdio.h>

typedef void (*pfnVect)();
extern pfnVect vectors[];
#define EXC_VECTOR(f) f,

extern unsigned long _estack;
extern unsigned long _sidata;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long RAM_USED;
extern unsigned long RAM_SIZE;
extern unsigned long RAM_USAGE;

int main()
{
  MAP_IntVTableBaseSet((unsigned long)vectors);
  MAP_IntMasterEnable();
  MAP_IntEnable(FAULT_SYSTICK);
  PRCMCC3200MCUInit();
#ifdef UART_LOG
  MAP_PRCMPeripheralClkEnable(PRCM_UART_TERM, PRCM_RUN_MODE_CLK);
  MAP_PinTypeUART(PIN_TERM_TX, PIN_TERM_TX_MODE);
  MAP_PinTypeUART(PIN_TERM_RX, PIN_TERM_RX_MODE);
  MAP_UARTConfigSetExpClk(UART_TERM,
                          MAP_PRCMPeripheralClockGet(PRCM_UART_TERM),
                          115200,
                          (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));
#endif

  MonitorLoop();

  asm(" BKPT");
  while ( 1 );

  return 0;
}


__attribute__( ( naked ) )
void Reset_Handler()
{
  register unsigned long *pulSP __asm("sp") = &_estack;
  register unsigned long *pulSrc = &_sidata;
  register unsigned long *pulDest = &_sdata;

  pulSP = pulSP;
  while( pulDest < &_edata )
  {
    *pulDest++ = *pulSrc++;
  }

  pulDest = &_sbss;
  while ( pulDest < &_ebss )
  {
    *pulDest++ = 0;
  }

  main();

  while( 1 );
}

typedef struct
{
  unsigned long m_dwExR0;
  unsigned long m_dwExR1;
  unsigned long m_dwExR2;
  unsigned long m_dwExR3;
  unsigned long m_dwExR12;
  unsigned long m_dwExLR;
  unsigned long m_dwExPC;
  unsigned long m_dwExPSR;
}SCortexM3ExContext;

__attribute__( ( naked ) )
void NMIHandler()
{
  asm(" BKPT");
  while( 1 );
}

__attribute__( ( naked ) )
void HardFaultHandler()
{
  register SCortexM3ExContext * psCortexM3ExContext __asm("r0");
  register unsigned long dwLR __asm("lr");

  psCortexM3ExContext = psCortexM3ExContext;
  if ( dwLR & 4 )
    asm(" mrs r0, psp");
  else
    asm(" mrs r0, msp");

  asm(" BKPT");
  while ( 1 );
}

__attribute__( ( naked ) )
void MemManageHandler()
{
  asm(" BKPT");
  while( 1 );
}

__attribute__( ( naked ) )
void BusFaultHandler()
{
  asm(" BKPT");
  while( 1 );
}

__attribute__( ( naked ) )
void UsageFaultHandler()
{
  asm(" BKPT");
  while( 1 );
}

__attribute__( ( naked ) )
void defaultHandler()
{
  asm(" BKPT");
  while( 1 );
}

__attribute__ ((section(".isr_vector")))
pfnVect vectors[NUM_INTERRUPTS] =
{
  (pfnVect)&_estack,
  EXC_VECTOR(Reset_Handler)
  EXC_VECTOR(NMIHandler)
  EXC_VECTOR(HardFaultHandler)
  EXC_VECTOR(MemManageHandler)
  EXC_VECTOR(BusFaultHandler)
  EXC_VECTOR(UsageFaultHandler)
  EXC_VECTOR(defaultHandler)
  EXC_VECTOR(defaultHandler)
};

#ifdef UART_LOG
#define ITM_STIM0_ADDR 0xE0000000
int PutChar(int ch)
{
  while ( !*(unsigned long *)ITM_STIM0_ADDR );
  *(unsigned char *)ITM_STIM0_ADDR = (unsigned char)ch;
  UARTCharPut(UART_TERM, ch);
  return ch;
}

#include <sys/stat.h>

int _close(int file)
{
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode =  S_IFSOCK;//S_IFIFO;// S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  return 1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _read(int file, char *ptr, int len)
{
  return 0;
}

caddr_t _sbrk(int incr)
{
  extern char _end;
  static char *heap_end;
  char *prev_heap_end;
  if (heap_end == 0)
  {
    heap_end = &_end;
  }
  prev_heap_end = heap_end;
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len)
{
  int i;
  for ( i=0; i < len; i++ )
  {
    PutChar(*ptr++);
  }
  return len;
}
#endif
