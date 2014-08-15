#ifndef __DEFS_H__
#define __DEFS_H__

//#define UART_LOG
#define MONITOR_BUFFER_SIZE 0x4000
#define MAX_FILE_NAME_LENGTH 128


#ifdef UART_LOG
#define LOG_PRINT printf

#define UART_TERM_NONE 0
#define UART_TERM_CC3200LAUNCHXL_FTDI 1
#define UART_TERM_CC3200_JTAG 2
#define UART_FOR_TERM UART_TERM_CC3200_JTAG


#ifdef UART_FOR_TERM
#if UART_FOR_TERM == UART_TERM_CC3200LAUNCHXL_FTDI
  #define PRCM_UART_TERM PRCM_UARTA0
  #define PIN_TERM_TX PIN_55
  #define PIN_TERM_TX_MODE PIN_MODE_3
  #define PIN_TERM_RX PIN_57
  #define PIN_TERM_RX_MODE PIN_MODE_3
  #define UART_TERM UARTA0_BASE
  #define PRCM_UART_TERM PRCM_UARTA0
#elif UART_FOR_TERM == UART_TERM_CC3200_JTAG
  #define PRCM_UART_TERM PRCM_UARTA1
  #define PIN_TERM_TX PIN_16
  #define PIN_TERM_TX_MODE PIN_MODE_2
  #define PIN_TERM_RX PIN_17
  #define PIN_TERM_RX_MODE PIN_MODE_2
  #define UART_TERM UARTA1_BASE
  #define PRCM_UART_TERM PRCM_UARTA1
#else
#define NO_TERM_UART
#endif
#else
#error Define which UART to use (or NONE)
#endif
#else
#define LOG_PRINT(...)
#endif


#endif  //  __DEFS_H__
