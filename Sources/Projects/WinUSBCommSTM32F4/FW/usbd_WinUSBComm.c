/**
  ******************************************************************************
  * @file    USBD_WinUSBComm.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    18-February-2014
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *      
  *          ===================================================================      
  *                                WinUSBComm Class  Description
  *          ===================================================================
  *          
  *
  *
  *
  *           
  *      
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *           
  *      
  *  @endverbatim
  *
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
#include "usbd_WinUSBComm.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"

#include "WinUSBCommShared.h"
#include "WinUSBComm.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_WinUSBComm
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_WinUSBComm_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */ 


/** @defgroup USBD_WinUSBComm_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_WinUSBComm_Private_Macros
  * @{
  */ 
                                         
/**
  * @}
  */ 




/** @defgroup USBD_WinUSBComm_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_WinUSBComm_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx);

static uint8_t  USBD_WinUSBComm_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx);

static uint8_t  USBD_WinUSBComm_Setup (USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_WinUSBComm_GetCfgDesc (uint16_t *length);

static uint8_t  *USBD_WinUSBComm_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_WinUSBComm_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_WinUSBComm_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_WinUSBComm_EP0_RxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_WinUSBComm_EP0_TxReady (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_WinUSBComm_SOF (USBD_HandleTypeDef *pdev);

static uint8_t  USBD_WinUSBComm_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_WinUSBComm_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum);

/**
  * @}
  */ 

/** @defgroup USBD_WinUSBComm_Private_Variables
  * @{
  */

#if 1 // Descriptors
USBD_ClassTypeDef  USBD_WinUSBComm_ClassDriver =
{
  USBD_WinUSBComm_Init,
  USBD_WinUSBComm_DeInit,
  USBD_WinUSBComm_Setup,
  USBD_WinUSBComm_EP0_TxReady,
  USBD_WinUSBComm_EP0_RxReady,
  USBD_WinUSBComm_DataIn,
  USBD_WinUSBComm_DataOut,
  USBD_WinUSBComm_SOF,
  USBD_WinUSBComm_IsoINIncomplete,
  USBD_WinUSBComm_IsoOutIncomplete,
  USBD_WinUSBComm_GetCfgDesc,
  USBD_WinUSBComm_GetCfgDesc,
  USBD_WinUSBComm_GetCfgDesc,
  USBD_WinUSBComm_GetDeviceQualifierDesc,
};

/* USB WinUSBComm device Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
__ALIGN_BEGIN uint8_t USBD_WinUSBComm_CfgHSDesc[USB_WINUSBCOMM_CONFIG_DESC_SIZ]  __ALIGN_END =
{

  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_WINUSBCOMM_CONFIG_DESC_SIZ,

  0x00,
  0x01,   /* bNumInterfaces: 1 s_byCurrentInterface */
  0x01,   /* bConfigurationValue: */
  USBD_IDX_CONFIG_STR,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0xFA,   /* MaxPower 500 mA */

  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0xFF,   /* bInterfaceClass: Vendor  Class */
  0x00,   /* bInterfaceSubClass : none*/
  0x00,   /* nInterfaceProtocol */
  USBD_IDX_INTERFACE_STR,          /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  WINUSBCOMM_EPIN_ADDR,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(WINUSBCOMM_MAX_HS_PACKET),
  HIBYTE(WINUSBCOMM_MAX_HS_PACKET),
  0x00,   /*Polling interval in milliseconds */

  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  WINUSBCOMM_EPOUT_ADDR,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(WINUSBCOMM_MAX_HS_PACKET),
  HIBYTE(WINUSBCOMM_MAX_HS_PACKET),
  0x00     /*Polling interval in milliseconds*/
};

__ALIGN_BEGIN uint8_t USBD_WinUSBComm_CfgFSDesc[USB_WINUSBCOMM_CONFIG_DESC_SIZ]  __ALIGN_END =
{

  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_WINUSBCOMM_CONFIG_DESC_SIZ,

  0x00,
  0x01,   /* bNumInterfaces: 1 s_byCurrentInterface */
  0x01,   /* bConfigurationValue: */
  USBD_IDX_CONFIG_STR,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0xFA,   /* MaxPower 500 mA */

  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0xFF,   /* bInterfaceClass: Vendor  Class */
  0x00,   /* bInterfaceSubClass : none*/
  0x00,   /* nInterfaceProtocol */
  USBD_IDX_INTERFACE_STR,          /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  WINUSBCOMM_EPIN_ADDR,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(WINUSBCOMM_MAX_FS_PACKET),
  HIBYTE(WINUSBCOMM_MAX_FS_PACKET),
  0x00,   /*Polling interval in milliseconds */
  
  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  WINUSBCOMM_EPOUT_ADDR,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(WINUSBCOMM_MAX_FS_PACKET),
  HIBYTE(WINUSBCOMM_MAX_FS_PACKET),
  0x00     /*Polling interval in milliseconds*/
};

__ALIGN_BEGIN uint8_t USBD_WinUSBComm_OtherSpeedCfgDesc[USB_WINUSBCOMM_CONFIG_DESC_SIZ]   __ALIGN_END  =
{
  
  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION,
  USB_WINUSBCOMM_CONFIG_DESC_SIZ,

  0x00,
  0x01,   /* bNumInterfaces: 1 s_byCurrentInterface */
  0x01,   /* bConfigurationValue: */
  USBD_IDX_CONFIG_STR,   /* iConfiguration: */
  0xC0,   /* bmAttributes: */
  0xFA,   /* MaxPower 500 mA */

  /********************  Mass Storage interface ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0xFF,   /* bInterfaceClass: Vendor  Class */
  0x00,   /* bInterfaceSubClass : none*/
  0x00,   /* nInterfaceProtocol */
  USBD_IDX_INTERFACE_STR,          /* iInterface: */
  /********************  Mass Storage Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  WINUSBCOMM_EPIN_ADDR,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  0x40,
  0x00,
  0x00,   /*Polling interval in milliseconds */

  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  WINUSBCOMM_EPOUT_ADDR,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  0x40,
  0x00,
  0x00     /*Polling interval in milliseconds*/
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN  uint8_t USBD_WinUSBComm_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]  __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  WINUSBCOMM_MAX_FS_PACKET,
  0x01,
  0x00,
};
#endif  //  Descriptors

static uint8_t s_byWinUSBCommVersion = winusbcommversion2;

/**
  * @}
  */ 

/** @defgroup USBD_WinUSBComm_Private_Functions
  * @{
  */ 

/**
  * @brief  USBD_WinUSBComm_Init
  *         Initialize the WinUSBComm interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx)
{
  if(pdev->dev_speed == USBD_SPEED_HIGH  )
  {
    /* Open EP OUT */
    USBD_LL_OpenEP(pdev,
                   WINUSBCOMM_EPOUT_ADDR,
                   USBD_EP_TYPE_BULK,
                   WINUSBCOMM_MAX_HS_PACKET);

    /* Open EP IN */
    USBD_LL_OpenEP(pdev,
                   WINUSBCOMM_EPIN_ADDR,
                   USBD_EP_TYPE_BULK,
                   WINUSBCOMM_MAX_HS_PACKET);
  }
  else
  {
    /* Open EP OUT */
    USBD_LL_OpenEP(pdev,
                   WINUSBCOMM_EPOUT_ADDR,
                   USBD_EP_TYPE_BULK,
                   WINUSBCOMM_MAX_FS_PACKET);

    /* Open EP IN */
    USBD_LL_OpenEP(pdev,
                   WINUSBCOMM_EPIN_ADDR,
                   USBD_EP_TYPE_BULK,
                   WINUSBCOMM_MAX_FS_PACKET);
  }
  return USBD_OK;
}

/**
  * @brief  USBD_WinUSBComm_Init
  *         DeInitialize the WinUSBComm layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx)
{
  /* Close WinUSBComm EPs */
  USBD_LL_CloseEP(pdev, WINUSBCOMM_EPOUT_ADDR);
  USBD_LL_CloseEP(pdev, WINUSBCOMM_EPIN_ADDR);

  /* Free WinUSBComm Class Resources */
  pdev->pClassData  = NULL;

  return USBD_OK;
}

static void USBD_WinUSBComm_UpdateState(SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4)
{
  unsigned long dwReceivedByteCountUSB = psWinUSBCommSTM32F4->m_pbyReceivePtrUSB - psWinUSBCommSTM32F4->m_pbyBuffer;
  if ( psWinUSBCommSTM32F4->m_dwExpectedByteCountUSB )
  {
    if ( psWinUSBCommSTM32F4->m_dwSendByteCountUSB )
    {
      psWinUSBCommSTM32F4->m_byStateUSB = winusbcomm2stateError;
      return;
    }
    if ( dwReceivedByteCountUSB < psWinUSBCommSTM32F4->m_dwExpectedByteCountUSB )
    {
      psWinUSBCommSTM32F4->m_byStateUSB = winusbcomm2stateReceiving;
      return;
    }
    psWinUSBCommSTM32F4->m_byStateUSB = winusbcomm2stateProcessing;
    return;
  }

  if ( dwReceivedByteCountUSB )
  {
    if ( psWinUSBCommSTM32F4->m_dwSendByteCountUSB )
    {
      psWinUSBCommSTM32F4->m_byStateUSB = winusbcomm2stateError;
      return;
    }
    psWinUSBCommSTM32F4->m_byStateUSB = winusbcomm2stateProcessing;
    return;
  }

  if ( psWinUSBCommSTM32F4->m_dwSendByteCountUSB )
  {
    if ( psWinUSBCommSTM32F4->m_dwExpectedByteCountUSB || dwReceivedByteCountUSB )
    {
      psWinUSBCommSTM32F4->m_byStateUSB = winusbcomm2stateError;
      return;
    }
    psWinUSBCommSTM32F4->m_byStateUSB = winusbcomm2stateSending;
    return;
  }
  psWinUSBCommSTM32F4->m_byStateUSB = winusbcomm2stateIdle;
}


/**
  * @brief  USBD_WinUSBComm_Setup
  *         Handle the WinUSBComm specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  SSTM32F4USB *psSTM32F4USB = (SSTM32F4USB *)pdev->pUserData;
  SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4 = &psSTM32F4USB->m_sWinUSBCommSTM32F4;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {

  /* Class request */
  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest)
    {
    default:
       USBD_CtlError(pdev , req);
       return USBD_FAIL;
    }
    break;
  /* Interface & Endpoint request */
  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest)
    {
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev, (uint8_t *)&psSTM32F4USB->m_eCurrentInterface, 1);
      break;

    case USB_REQ_SET_INTERFACE :
      psSTM32F4USB->m_eCurrentInterface = (uint8_t)(req->wValue);
      break;
    
    case USB_REQ_CLEAR_FEATURE:

      /* Flush the FIFO and Clear the stall status */
      USBD_LL_FlushEP(pdev, (uint8_t)req->wIndex);

      /* Re-activate the EP */
      USBD_LL_CloseEP (pdev , (uint8_t)req->wIndex);
      if((((uint8_t)req->wIndex) & 0x80) == 0x80)
      {
        if(pdev->dev_speed == USBD_SPEED_HIGH  )
        {
          /* Open EP IN */
          USBD_LL_OpenEP(pdev, WINUSBCOMM_EPIN_ADDR, USBD_EP_TYPE_BULK, WINUSBCOMM_MAX_HS_PACKET);
        }
        else
        {
          /* Open EP IN */
          USBD_LL_OpenEP(pdev, WINUSBCOMM_EPIN_ADDR, USBD_EP_TYPE_BULK, WINUSBCOMM_MAX_FS_PACKET);
        }
      }
      else
      {
        if(pdev->dev_speed == USBD_SPEED_HIGH  )
        {
          /* Open EP IN */
          USBD_LL_OpenEP(pdev, WINUSBCOMM_EPOUT_ADDR, USBD_EP_TYPE_BULK, WINUSBCOMM_MAX_HS_PACKET);
        }
        else
        {
          /* Open EP IN */
          USBD_LL_OpenEP(pdev, WINUSBCOMM_EPOUT_ADDR, USBD_EP_TYPE_BULK, WINUSBCOMM_MAX_FS_PACKET);
        }
      }
      break;
    }
    break;
  case USB_REQ_TYPE_VENDOR:
    switch ( req->wIndex )
    {
    case stm32f4usbinterface_WinUSBComm:
    switch ( req->bRequest )
    {
    case winusbcomm2commandReset:
      psWinUSBCommSTM32F4->m_dwExpectedByteCountUSB = 0;
      psWinUSBCommSTM32F4->m_pbyReceivePtrUSB = psWinUSBCommSTM32F4->m_pbyBuffer;
      psWinUSBCommSTM32F4->m_dwSendByteCountUSB = 0;
      return USBD_OK;
    case winusbcomm2commandGetVersion:
      USBD_CtlSendData(pdev, &s_byWinUSBCommVersion, 1);
      return USBD_OK;
    case winusbcomm2commandGetState:
      USBD_CtlSendData(pdev, &psWinUSBCommSTM32F4->m_byStateUSB, 1);
      return USBD_OK;
    case winusbcomm2commandGetBufferSize:
      USBD_CtlSendData(pdev, (uint8_t*)(&psWinUSBCommSTM32F4->m_dwBufferSizeInBytes), 4);
      return USBD_OK;
    case winusbcomm2commandGetReturnSize:
      if ( psWinUSBCommSTM32F4->m_dwSendByteCountUSB )
      {
        if ( psWinUSBCommSTM32F4->m_dwExpectedByteCountUSB )
        {
          USBD_LL_Transmit(pdev, WINUSBCOMM_EPIN_ADDR, psWinUSBCommSTM32F4->m_pbySendPtrUSB, psWinUSBCommSTM32F4->m_dwSendByteCountUSB);
        }
        psWinUSBCommSTM32F4->m_pbyReceivePtrUSB = psWinUSBCommSTM32F4->m_pbyBuffer;
        psWinUSBCommSTM32F4->m_dwExpectedByteCountUSB = 0;
      }
      USBD_CtlSendData(pdev, (uint8_t*)(&psWinUSBCommSTM32F4->m_dwSendByteCountUSB), 4);
      return USBD_OK;
    case winusbcomm2commandFollowingPacketSize:
      psWinUSBCommSTM32F4->m_pbyReceivePtrUSB = psWinUSBCommSTM32F4->m_pbyBuffer;
      USBD_CtlPrepareRx(pdev, (uint8_t*)(&psWinUSBCommSTM32F4->m_dwExpectedByteCountUSB), 4);
      return USBD_OK;
    default:
      USBD_CtlError(pdev , req);
      return USBD_FAIL;
    }
      break;
    default:
      USBD_CtlError(pdev , req);
      return USBD_FAIL;
    }
    break;
  default:
    break;
  }
  return USBD_OK;
}


/**
  * @brief  USBD_WinUSBComm_GetCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_WinUSBComm_GetCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_WinUSBComm_CfgFSDesc);
  return USBD_WinUSBComm_CfgFSDesc;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_WinUSBComm_DeviceQualifierDescriptor (uint16_t *length)
{
  *length = sizeof (USBD_WinUSBComm_DeviceQualifierDesc);
  return USBD_WinUSBComm_DeviceQualifierDesc;
}


/**
  * @brief  USBD_WinUSBComm_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  SSTM32F4USB *psSTM32F4USB = (SSTM32F4USB *)pdev->pUserData;
  SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4 = &psSTM32F4USB->m_sWinUSBCommSTM32F4;
  psWinUSBCommSTM32F4->m_dwSendByteCountUSB = 0; // is this OK?
  USBD_WinUSBComm_UpdateState(psWinUSBCommSTM32F4);
  return USBD_OK;
}

/**
  * @brief  USBD_WinUSBComm_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_EP0_RxReady (USBD_HandleTypeDef *pdev)
{
  SSTM32F4USB *psSTM32F4USB = (SSTM32F4USB *)pdev->pUserData;
  SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4 = &psSTM32F4USB->m_sWinUSBCommSTM32F4;

  USBD_SetupReqTypedef *req = &pdev->request;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_VENDOR:
    switch ( req->wIndex )
    {
    case stm32f4usbinterface_WinUSBComm:
      switch ( req->bRequest )
      {
      case winusbcomm2commandFollowingPacketSize:
        USBD_LL_PrepareReceive(pdev, WINUSBCOMM_EPOUT_ADDR, psWinUSBCommSTM32F4->m_pbyReceivePtrUSB, psWinUSBCommSTM32F4->m_dwExpectedByteCountUSB);
        break;
      default:
        break;
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }


  USBD_WinUSBComm_UpdateState(psWinUSBCommSTM32F4);
  return USBD_OK;
}
/**
  * @brief  USBD_WinUSBComm_EP0_TxReady
  *         handle EP0 TRx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_EP0_TxReady (USBD_HandleTypeDef *pdev)
{
  SSTM32F4USB *psSTM32F4USB = (SSTM32F4USB *)pdev->pUserData;
  SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4 = &psSTM32F4USB->m_sWinUSBCommSTM32F4;
  USBD_WinUSBComm_UpdateState(psWinUSBCommSTM32F4);
  return USBD_OK;
}
/**
  * @brief  USBD_WinUSBComm_SOF
  *         handle SOF event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_SOF (USBD_HandleTypeDef *pdev)
{

  return USBD_OK;
}
/**
  * @brief  USBD_WinUSBComm_IsoINIncomplete
  *         handle data ISO IN Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_IsoINIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_WinUSBComm_IsoOutIncomplete
  *         handle data ISO OUT Incomplete event
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_IsoOutIncomplete (USBD_HandleTypeDef *pdev, uint8_t epnum)
{

  return USBD_OK;
}
/**
  * @brief  USBD_WinUSBComm_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_WinUSBComm_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  SSTM32F4USB *psSTM32F4USB = (SSTM32F4USB *)pdev->pUserData;
  SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4 = &psSTM32F4USB->m_sWinUSBCommSTM32F4;
  uint32_t dwNumBytesReceived = USBD_LL_GetRxDataSize(pdev, epnum);
  psWinUSBCommSTM32F4->m_pbyReceivePtrUSB += dwNumBytesReceived;
  USBD_WinUSBComm_UpdateState(psWinUSBCommSTM32F4);
  return USBD_OK;
}

/**
* @brief  DeviceQualifierDescriptor 
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *USBD_WinUSBComm_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_WinUSBComm_DeviceQualifierDesc);
  return USBD_WinUSBComm_DeviceQualifierDesc;
}

/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
