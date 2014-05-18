/**
  ******************************************************************************
  * @file    USBD_WinUSBComm_core.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   header file for the USBD_WinUSBComm_core.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#ifndef __USB_WINUSBCOMM_CORE_H_
#define __USB_WINUSBCOMM_CORE_H_

#include  "usbd_ioreq.h"
#include  "WinUSBCommSTM32F4.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_WinUSBComm
  * @brief This file is the Header file for USBD_msc.c
  * @{
  */ 


/** @defgroup USBD_WinUSBComm_Exported_Defines
  * @{
  */

//#define WINUSBCOMM_DUAL_INTERFACE_TEST  ///< Define to test behavior when device exposes more WinUSB associated drivers.

#define WINUSBCOMM_EPIN_ADDR                 0x81
#define WINUSBCOMM_EPOUT_ADDR                0x02

#ifdef WINUSBCOMM_DUAL_INTERFACE_TEST
#define WINUSBCOMM_EPIN_ADDR_2              0x83
#define WINUSBCOMM_EPOUT_ADDR_2             0x04
#endif

#define WINUSBCOMM_MAX_FS_PACKET            0x40
#define WINUSBCOMM_MAX_HS_PACKET            0x200

#ifdef WINUSBCOMM_DUAL_INTERFACE_TEST
#define USB_WINUSBCOMM_CONFIG_DESC_SIZ       (9 + 9+7+7 + 9+7+7)
#else
#define USB_WINUSBCOMM_CONFIG_DESC_SIZ       (9 + 9+7+7)
#endif

#ifdef WINUSBCOMM_DUAL_INTERFACE_TEST
#define USB_WINUSBCOMM_COMPAT_ID_OS_DESC_SIZ       (16 + 24 + 24)
#else
#define USB_WINUSBCOMM_COMPAT_ID_OS_DESC_SIZ       (16 + 24)
#endif

/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

typedef enum _ESTM32F4USBInterfaces
{
  stm32f4usbinterface_WinUSBComm,
#ifdef WINUSBCOMM_DUAL_INTERFACE_TEST
  stm32f4usbinterface_WinUSBComm2,
#endif

  stm32f4usbinterface_NumInterfaces
}ESTM32F4USBInterfaces;


///< Type for STM32F4xx USB data transfer context
///< Has one member with custom data
///< transfer context per USB interface
typedef struct _SSTM32F4USB
{
  ESTM32F4USBInterfaces m_eCurrentInterface;
  SWinUSBCommSTM32F4 m_sWinUSBCommSTM32F4;  ///< WinUSBComm interface data transfer context
}SSTM32F4USB;

/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_ClassTypeDef  USBD_WinUSBComm_ClassDriver;
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */

void USBD_WinUSBComm_Receive(void *pParam, unsigned char *pbyBuffer, unsigned long dwCount);
void USBD_WinUSBComm_Send(void *pParam, unsigned char *pbyBuffer, unsigned long dwCount);

/**
  * @}
  */ 

#endif  // __USB_WINUSBCOMM_CORE_H_
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
