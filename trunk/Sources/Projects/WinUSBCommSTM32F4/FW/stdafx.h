#ifndef __STDAFX_H__
#define __STDAFX_H__

#include "stm32f4xx.h"

#ifndef NULL
#define NULL 0
#endif

#ifndef _countof
#define _countof(a) (sizeof(a)/sizeof(a[0]))
#endif

#ifndef DWORD2LSB
#define DWORD2LSB(dw, buff) *(unsigned long *)(buff) = dw
#endif

#ifndef LSB2DWORD
#define LSB2DWORD(dw, buff) dw = *(unsigned long *)(buff)
#endif

#endif  //  __STDAFX_H__
