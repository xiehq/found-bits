#include "stdafx.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_WinUSBComm.h"

#include "CommCore.h"


extern unsigned char _sram;

uint32_t LibSTM32F4xx_GetHSEValue() { return HSE_VALUE; }
uint32_t LibSTM32F4xx_GetExternalClockValue() { return EXTERNAL_CLOCK_VALUE; }

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInit;
  RCC_ClkInitTypeDef RCC_ClkInit;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Configure RCC Oscillators: All parameters can be changed according to user’s needs */
  RCC_OscInit.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInit.HSEState = RCC_HSE_ON;
  RCC_OscInit.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInit.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInit.PLL.PLLM = 8;
  RCC_OscInit.PLL.PLLN = 336;
  RCC_OscInit.PLL.PLLP = 2;
  RCC_OscInit.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInit);

  /* RCC Clocks: All parameters can be changed according to user’s needs */
  RCC_ClkInit.ClockType = RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_HCLK |RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInit.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInit.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInit, FLASH_LATENCY_5);
}



int main()
{
  USBD_HandleTypeDef USBD_Device;
  SSTM32F4USB sSTM32F4USB;
  SCommLayer sCommLayerWinUSBComm;
  SCommLayer *apsCommLayers[] =
  {
    &sCommLayerWinUSBComm
  };

  SCommStack sCommStack;
  HCOMMSTACK hComm;
  COMMCOUNT cntNumBytes = 0;
  ECommStatus eCommStatus = commstatusIdle;

  sSTM32F4USB.m_sWinUSBCommSTM32F4.m_pCallbackParam = &USBD_Device;
  sSTM32F4USB.m_sWinUSBCommSTM32F4.m_pfnReceiveCallback = &USBD_WinUSBComm_Receive;
  sSTM32F4USB.m_sWinUSBCommSTM32F4.m_pfnSendCallback= &USBD_WinUSBComm_Send;

  USBD_Device.pUserData = &sSTM32F4USB;

  WinUSBCommSTM32F4_Init(&sCommLayerWinUSBComm, &sSTM32F4USB.m_sWinUSBCommSTM32F4, &_sram, 0x4000); // 16kB comm buffer

  hComm = CommStack_Init(commstackflSideHost, &sCommStack, apsCommLayers, _countof(apsCommLayers));

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
     */
  HAL_Init();

  /* Configure the system clock to 168 Mhz */
  SystemClock_Config();



  /* Init MSC Application */
  USBD_Init(&USBD_Device, &WinUSBComm_Desc, 0);

  /* Add Supported Class */
  USBD_RegisterClass(&USBD_Device, &USBD_WinUSBComm_ClassDriver);

  /* Start Device Process */
  USBD_Start(&USBD_Device);

  while ( 1 )
  {
    while ( ( commstatusIdle == eCommStatus ) || ( commstatusActive == eCommStatus ) )
    {
      eCommStatus = CommStack_ReceiveProcess(hComm, &cntNumBytes);
    }

    if ( commstatusNewPacket != eCommStatus )
    {
      CommStack_Disconnect(hComm);
      break;
    }

    CommStack_PacketStart(hComm);
    // TODO: process and send response packet
    CommStack_PacketEnd(hComm);

    while ( commstatusActive == eCommStatus )
    {
      eCommStatus = CommStack_TransmitProcess(hComm, &cntNumBytes);
    }
  }

  //USBD_DeInit(&USBD_Device);

  return -1;
}

