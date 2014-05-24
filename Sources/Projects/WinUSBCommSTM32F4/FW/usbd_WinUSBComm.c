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

static uint8_t * USBD_WinUSBComm_GetUsrStrDescriptor(struct _USBD_HandleTypeDef *pdev, uint8_t index,  uint16_t *length);
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
  USBD_WinUSBComm_GetUsrStrDescriptor
};

__ALIGN_BEGIN uint8_t USBD_WinUSBComm_CfgFSDesc[USB_WINUSBCOMM_CONFIG_DESC_SIZ]  __ALIGN_END =
{

  0x09,   /* bLength: Configuation Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,   /* bDescriptorType: Configuration */
  USB_WINUSBCOMM_CONFIG_DESC_SIZ,

  0x00,
  stm32f4usbinterface_NumInterfaces,   /* bNumInterfaces */
  0x01,   /* bConfigurationValue: */
  USBD_IDX_CONFIG_STR,   /* iConfiguration: */
  0x80,   /* bmAttributes: */
  0xFA,   /* MaxPower 500 mA */

  /******************** Some interface over WinUSB ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0xFF,   /* bInterfaceClass: Vendor  Class */
  0x00,   /* bInterfaceSubClass : none*/
  0x00,   /* nInterfaceProtocol */
  USBD_IDX_INTERFACE_STR,          /* iInterface: */
  /******************** Some interface over WinUSB Endpoints ********************/
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
  0x00,    /*Polling interval in milliseconds*/

#ifdef WINUSBCOMM_DUAL_INTERFACE_TEST
  /******************** Some other interface over WinUSB ********************/
  0x09,   /* bLength: Interface Descriptor size */
  0x04,   /* bDescriptorType: */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints*/
  0xFF,   /* bInterfaceClass: Vendor  Class */
  0x00,   /* bInterfaceSubClass : none*/
  0x00,   /* nInterfaceProtocol */
  USBD_IDX_INTERFACE_STR,          /* iInterface: */
  /******************** Some interface over WinUSB Endpoints ********************/
  0x07,   /*Endpoint descriptor length = 7*/
  0x05,   /*Endpoint descriptor type */
  WINUSBCOMM_EPIN_ADDR_2,   /*Endpoint address (IN, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(WINUSBCOMM_MAX_FS_PACKET),
  HIBYTE(WINUSBCOMM_MAX_FS_PACKET),
  0x00,   /*Polling interval in milliseconds */

  0x07,   /*Endpoint descriptor length = 7 */
  0x05,   /*Endpoint descriptor type */
  WINUSBCOMM_EPOUT_ADDR_2,   /*Endpoint address (OUT, address 1) */
  0x02,   /*Bulk endpoint type */
  LOBYTE(WINUSBCOMM_MAX_FS_PACKET),
  HIBYTE(WINUSBCOMM_MAX_FS_PACKET),
  0x00     /*Polling interval in milliseconds*/
#endif
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

// MS OS String descriptor to tell Windows that it may query for other descriptors
// It's a standard string descriptor.
// Windows will only query for OS descriptors once!
// Delete the information about already queried devices in registry by deleting:
// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\usbflags\VVVVPPPPRRRR
__ALIGN_BEGIN uint8_t USBD_WinUSBComm_MS_OS_StringDescriptor[]  __ALIGN_END =
{
  0x12,           //  bLength           1 0x12  Length of the descriptor
  0x03,           //  bDescriptorType   1 0x03  Descriptor type
                  //  qwSignature      14 ‘MSFT100’ Signature field
  0x4D, 0x00,     //  'M'
  0x53, 0x00,     //  'S'
  0x46, 0x00,     //  'F'
  0x54, 0x00,     //  'T'
  0x31, 0x00,     //  '1'
  0x30, 0x00,     //  '0'
  0x30, 0x00,     //  '0'
  MS_VendorCode,  //  bMS_VendorCode    1 Vendor-specific Vendor code
  0x00            //  bPad              1 0x00  Pad field
};


// This associates winusb driver with the device
__ALIGN_BEGIN uint8_t USBD_WinUSBComm_Extended_Compat_ID_OS_Desc[USB_WINUSBCOMM_COMPAT_ID_OS_DESC_SIZ]  __ALIGN_END =
{
                                                    //    +-- Offset in descriptor
                                                    //    |             +-- Size
                                                    //    v             v
  USB_WINUSBCOMM_COMPAT_ID_OS_DESC_SIZ, 0, 0, 0,    //    0 dwLength    4 DWORD The length, in bytes, of the complete extended compat ID descriptor
  0x00, 0x01,                                       //    4 bcdVersion  2 BCD The descriptor’s version number, in binary coded decimal (BCD) format
  0x04, 0x00,                                       //    6 wIndex      2 WORD  An index that identifies the particular OS feature descriptor
  stm32f4usbinterface_NumInterfaces,                //    8 bCount      1 BYTE  The number of custom property sections
  0, 0, 0, 0, 0, 0, 0,                              //    9 RESERVED    7 BYTEs Reserved
                                                    //    =====================
                                                    //                 16

                                                    //   +-- Offset from function section start
                                                    //   |                        +-- Size
                                                    //   v                        v
  stm32f4usbinterface_WinUSBComm,                   //   0  bFirstInterfaceNumber 1 BYTE  The interface or function number
  0,                                                //   1  RESERVED              1 BYTE  Reserved
  0x57, 0x49, 0x4E, 0x55, 0x53, 0x42, 0x00, 0x00,   //   2  compatibleID          8 BYTEs The function’s compatible ID      ("WINUSB")
  0, 0, 0, 0, 0, 0, 0, 0,                           //  10  subCompatibleID       8 BYTEs The function’s subcompatible ID
  0, 0, 0, 0, 0, 0,                                 //  18  RESERVED              6 BYTEs Reserved
                                                    //  =================================
                                                    //                           24
#ifdef WINUSBCOMM_DUAL_INTERFACE_TEST
                                                    //   +-- Offset from function section start
                                                    //   |                        +-- Size
                                                    //   v                        v
  stm32f4usbinterface_WinUSBComm2,                  //   0  bFirstInterfaceNumber 1 BYTE  The interface or function number
  0,                                                //   1  RESERVED              1 BYTE  Reserved
  0x57, 0x49, 0x4E, 0x55, 0x53, 0x42, 0x00, 0x00,   //   2  compatibleID          8 BYTEs The function’s compatible ID      ("WINUSB")
  0, 0, 0, 0, 0, 0, 0, 0,                           //  10  subCompatibleID       8 BYTEs The function’s subcompatible ID
  0, 0, 0, 0, 0, 0,                                 //  18  RESERVED              6 BYTEs Reserved
                                                    //  =================================
                                                    //                           24
#endif
};

// Properties are added to:
// HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\USB\VID_xxxx&PID_xxxx\sssssssss\Device Parameters
// Use USBDeview or similar to uninstall

__ALIGN_BEGIN uint8_t USBD_WinUSBComm_Extended_Properties_OS_Desc[0xCC]  __ALIGN_END =
{
  0xCC, 0x00, 0x00, 0x00,   // 0 dwLength   4 DWORD The length, in bytes, of the complete extended properties descriptor
  0x00, 0x01,               // 4 bcdVersion 2 BCD   The descriptor’s version number, in binary coded decimal (BCD) format
  0x05, 0x00,               // 6 wIndex     2 WORD  The index for extended properties OS descriptors
  0x02, 0x00,               // 8 wCount     2 WORD  The number of custom property sections that follow the header section
                            // ====================
                            //             10
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  0x84, 0x00, 0x00, 0x00,   //  0       dwSize                  4 DWORD             The size of this custom properties section
  0x01, 0x00, 0x00, 0x00,   //  4       dwPropertyDataType      4 DWORD             Property data format
  0x28, 0x00,               //  8       wPropertyNameLength     2 DWORD             Property name length
                            // ========================================
                            //                                 10
                            // 10       bPropertyName         PNL WCHAR[]           The property name
  'D',0, 'e',0, 'v',0, 'i',0, 'c',0, 'e',0, 'I',0, 'n',0,
  't',0, 'e',0, 'r',0, 'f',0, 'a',0, 'c',0, 'e',0, 'G',0,
  'U',0, 'I',0, 'D',0, 0,0,
                            // ========================================
                            //                                 40 (0x28)

  0x4E, 0x00, 0x00, 0x00,   // 10 + PNL dwPropertyDataLength    4 DWORD             Length of the buffer holding the property data
                            // ========================================
                            //                                  4
    // 14 + PNL bPropertyData         PDL Format-dependent  Property data
  '{',0, 'E',0, 'A',0, '0',0, 'B',0, 'D',0, '5',0, 'C',0,
  '3',0, '-',0, '5',0, '0',0, 'F',0, '3',0, '-',0, '4',0,
  '8',0, '8',0, '8',0, '-',0, '8',0, '4',0, 'B',0, '4',0,
  '-',0, '7',0, '4',0, 'E',0, '5',0, '0',0, 'E',0, '1',0,
  '6',0, '4',0, '9',0, 'D',0, 'B',0, '}',0,  0 ,0,
                            // ========================================
                            //                                 78 (0x4E)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  0x3E, 0x00, 0x00, 0x00,   //  0 dwSize 0x00000030 (62 bytes)
  0x01, 0x00, 0x00, 0x00,   //  4 dwPropertyDataType 0x00000001 (Unicode string)
  0x0C, 0x00,               //  8 wPropertyNameLength 0x000C (12 bytes)
                            // ========================================
                            //                                  10
  'L',0, 'a',0, 'b',0, 'e',0, 'l',0, 0,0,
                            // 10 bPropertyName “Label”
                            // ========================================
                            //                                  12
  0x24, 0x00, 0x00, 0x00,   // 22 dwPropertyDataLength 0x00000016 (36 bytes)
                            // ========================================
                            //                                  4
  'W',0, 'i',0, 'n',0, 'U',0, 'S',0, 'B',0, 'C',0, 'o',0, 'm',0, 'm',0, ' ',0, 'd',0, 'e',0, 'v',0, 'i',0, 'c',0, 'e',0, 0,0
                            // 26 bPropertyData “WinUSBComm Device”
                            // ========================================
                            //                                  36

};

// Experiment with these to try to remove device from "Safely Remove Hardware" list:
// HKR,,"SurpriseRemovalOK",0x00010001,1
// HKR,,"Removable",0x00010001,1
// HKR,,"RemovalPolicy",0x00010001,3


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

static uint8_t  USBD_WinUSBComm_SetupStandrad(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  SSTM32F4USB *psSTM32F4USB = (SSTM32F4USB *)pdev->pUserData;

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
  default:
    USBD_CtlError(pdev , req);
    return USBD_FAIL;
  }
  return USBD_OK;
}

static uint8_t  USBD_WinUSBComm_SetupVendorDevice(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  USBD_CtlError(pdev , req);
  return USBD_FAIL;
}

static uint8_t  USBD_WinUSBComm_SetupVendorInterface(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  SSTM32F4USB *psSTM32F4USB = (SSTM32F4USB *)pdev->pUserData;
  SWinUSBCommSTM32F4 *psWinUSBCommSTM32F4 = &psSTM32F4USB->m_sWinUSBCommSTM32F4;

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
  return USBD_OK;
}
static uint8_t  USBD_WinUSBComm_GetMSExtendedCompatIDOSDescriptor (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  switch (req->wIndex)
  {
  case 0x04:
    USBD_CtlSendData (pdev, USBD_WinUSBComm_Extended_Compat_ID_OS_Desc, req->wLength);
    break;
  default:
   USBD_CtlError(pdev , req);
   return USBD_FAIL;
  }
  return USBD_OK;
}
static uint8_t  USBD_WinUSBComm_GetMSExtendedPropertiesOSDescriptor (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint8_t byInterfaceIndex = (uint8_t)req->wValue;
  if ( req->wIndex != 0x05 )
  {
    USBD_CtlError(pdev , req);
    return USBD_FAIL;
  }
  switch ( byInterfaceIndex )
  {
  case stm32f4usbinterface_WinUSBComm:
#ifdef WINUSBCOMM_DUAL_INTERFACE_TEST
  case stm32f4usbinterface_WinUSBComm2:
#endif
    USBD_CtlSendData (pdev, USBD_WinUSBComm_Extended_Properties_OS_Desc, req->wLength);
    break;
  default:
    USBD_CtlError(pdev , req);
    return USBD_FAIL;
  }
  return USBD_OK;
}
static uint8_t  USBD_WinUSBComm_SetupVendor(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  switch ( req->bmRequest & USB_REQ_RECIPIENT_MASK )
  {
  case USB_REQ_RECIPIENT_DEVICE:
    return ( MS_VendorCode == req->bRequest ) ? USBD_WinUSBComm_GetMSExtendedCompatIDOSDescriptor(pdev, req) : USBD_WinUSBComm_SetupVendorDevice(pdev, req);
  case USB_REQ_RECIPIENT_INTERFACE:
    return ( MS_VendorCode == req->bRequest ) ? USBD_WinUSBComm_GetMSExtendedPropertiesOSDescriptor(pdev, req) : USBD_WinUSBComm_SetupVendorInterface(pdev, req);
  case USB_REQ_RECIPIENT_ENDPOINT:
    // fall through
  default:
    break;
  }
  USBD_CtlError(pdev , req);
  return USBD_FAIL;
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
  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
  case USB_REQ_TYPE_CLASS:
    break;
  case USB_REQ_TYPE_STANDARD:
    return USBD_WinUSBComm_SetupStandrad(pdev, req);
  case USB_REQ_TYPE_VENDOR:
    return USBD_WinUSBComm_SetupVendor(pdev, req);
  default:
    break;
  }
  USBD_CtlError(pdev , req);
  return USBD_FAIL;
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
static uint8_t  *USBD_WinUSBComm_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_WinUSBComm_DeviceQualifierDesc);
  return USBD_WinUSBComm_DeviceQualifierDesc;
}

/**
* @brief  GetUsrStrDescriptor
*         return non standard string descriptor (OS String Descriptor)
* @param  pdev: device instance
* @param  index : descriptor index (0xEE for MS OS String Descriptor)
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t * USBD_WinUSBComm_GetUsrStrDescriptor(struct _USBD_HandleTypeDef *pdev, uint8_t index,  uint16_t *length)
{
  *length = 0;
  if ( 0xEE == index )
  {
    *length = sizeof (USBD_WinUSBComm_MS_OS_StringDescriptor);
    return USBD_WinUSBComm_MS_OS_StringDescriptor;
  }
  return NULL;
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
