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

#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
#include "usb_prop.h"
#include "WinUSBCommSTM32F103.h"

static uint8_t * s_pbyControlRequestData = NULL;
static uint16_t s_wControlRequestDataSize = 0;

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


DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    USB_init,
    USB_Reset,
    USB_Status_In,
    USB_Status_Out,
    USB_Data_Setup,
    USB_NoData_Setup,
    USB_Get_Interface_Setting,
    USB_GetDeviceDescriptor,
    USB_GetConfigDescriptor,
    USB_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };

USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    USB_GetConfiguration,
    USB_SetConfiguration,
    USB_GetInterface,
    USB_SetInterface,
    USB_GetStatus,
    USB_ClearFeature,
    USB_SetEndPointFeature,
    USB_SetDeviceFeature,
    USB_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)DeviceDescriptor,
    SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
  {
    (uint8_t*)ConfigDescriptor,
    SIZ_CONFIG_DESC
  };


uint8_t StringVendor[64];
uint8_t StringProduct[64];
uint8_t StringSerial[144];
uint8_t StringInterface[32];

ONE_DESCRIPTOR String_Descriptor[] =
{
  {(uint8_t*)StringLangID,    SIZ_STRING_LANGID    },
  {(uint8_t*)StringVendor,    sizeof(StringVendor)    },
  {(uint8_t*)StringProduct,   sizeof(StringProduct)   },
  {(uint8_t*)StringSerial,    sizeof(StringSerial)    },
  {(uint8_t*)StringInterface, sizeof(StringInterface) },
};
COMPILETIME_ASSERT(usdsdNumUSBStringDescriptors == _countof(String_Descriptor));

void usb_StringToDescriptor(const char *pcszString, ONE_DESCRIPTOR *pDescriptor)
{
  uint32_t dwUNIStringLength = 0;
  uint32_t I = 0;
  while ( 0 != *pcszString )
  {
    pDescriptor->Descriptor[2 + I++] = *pcszString++;
    pDescriptor->Descriptor[2 + I++] = 0;
    dwUNIStringLength += 2;
  }
  dwUNIStringLength += 2; // for descriptor length and type
  ASSERT(pDescriptor->Descriptor_Size >= dwUNIStringLength);
  pDescriptor->Descriptor_Size = dwUNIStringLength;
  pDescriptor->Descriptor[0] = (uint8_t)dwUNIStringLength;  // length
  pDescriptor->Descriptor[1] = 0x03;  // type
}

void usb_InitDescriptors()
{
  const static uint8_t abyHEX[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  uint8_t abySerial[sizeof(uint32_t) * 3];
  char achSerial[4 + sizeof(uint32_t) * 3 * 2 + 1] = { 0 };
  uint32_t I = sizeof(abySerial);
  uint32_t S = 4;
  achSerial[0] = 'S';
  achSerial[1] = 'T';
  achSerial[2] = 'M';
  achSerial[3] = 'n';
  STM32_GetSerial((uint32_t *)(abySerial + 8), (uint32_t *)(abySerial + 4), (uint32_t *)abySerial);
  do
  {
    I--;
    achSerial[S++] = abyHEX[abySerial[I] >> 4];
    achSerial[S++] = abyHEX[abySerial[I] & 0xF];
  }while ( I != 0);


  usb_StringToDescriptor(g_pcszUSBVendorName, &String_Descriptor[usbsdVendor]);
  usb_StringToDescriptor(g_pcszUSBProductName, &String_Descriptor[usbsdProduct]);
  usb_StringToDescriptor(achSerial, &String_Descriptor[usbsdSerial]);
  usb_StringToDescriptor(g_pcszWinUSBUSBInterfaceName, &String_Descriptor[usbsdWinUSBInterface]);

}
/*******************************************************************************
* Function Name  : USB_init
* Description    : USB init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_init()
{
  usb_InitDescriptors();

  pInformation->Current_Configuration = 0;

  /* Connect the device */
  PowerOn();

  /* Perform basic device initialization operations */
  USB_SIL_Init();

  bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : USB_Reset
* Description    : USB reset routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Reset()
{
  /* Set the device as not configured */
  Device_Info.Current_Configuration = 0;

  /* Current Feature initialization */
  pInformation->Current_Feature = ConfigDescriptor[7];

#ifdef STM32F10X_CL   
  
  /* EP0 is already configured by USB_SIL_Init() function */

  /* Init EP1 IN as Bulk endpoint */
  OTG_DEV_EP_Init(EP1_IN, OTG_DEV_EP_TYPE_BULK, BULK_MAX_PACKET_SIZE);
  
  /* Init EP2 OUT as Bulk endpoint */
  OTG_DEV_EP_Init(EP2_OUT, OTG_DEV_EP_TYPE_BULK, BULK_MAX_PACKET_SIZE); 
  
#else 

  SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_NAK);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxValid(ENDP0);

  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  SetEPRxStatus(ENDP1, EP_RX_DIS);

  /* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_BULK);
  SetEPRxAddr(ENDP2, ENDP2_RXADDR);
  SetEPRxCount(ENDP2, Device_Property.MaxPacketSize);
  SetEPRxStatus(ENDP2, EP_RX_VALID);
  SetEPTxStatus(ENDP2, EP_TX_DIS);


  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);

  /* Set the device to response on default address */
  SetDeviceAddress(0);
#endif /* STM32F10X_CL */

  bDeviceState = ATTACHED;

  USB_NotConfigured_LED();
}

/*******************************************************************************
* Function Name  : USB_SetConfiguration
* Description    : Handle the SetConfiguration request.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_SetConfiguration(void)
{
  if (pInformation->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;

#ifdef STM32F10X_CL 
    /* Init EP1 IN as Bulk endpoint */
    OTG_DEV_EP_Init(EP1_IN, OTG_DEV_EP_TYPE_BULK, BULK_MAX_PACKET_SIZE);
  
    /* Init EP2 OUT as Bulk endpoint */
    OTG_DEV_EP_Init(EP2_OUT, OTG_DEV_EP_TYPE_BULK, BULK_MAX_PACKET_SIZE);     
#else    
    ClearDTOG_TX(ENDP1);
    ClearDTOG_RX(ENDP2);
#endif /* STM32F10X_CL */
  }
}

/*******************************************************************************
* Function Name  : USB_SetConfiguration.
* Description    : Update the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}
/*******************************************************************************
* Function Name  : USB_Status_In
* Description    : USB Status IN routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Status_In(void)
{
  return;
}

/*******************************************************************************
* Function Name  : USB_Status_Out
* Description    : USB Status OUT routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Status_Out(void)
{
  return;
}


uint8_t *usb_ControlCopyData(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = s_wControlRequestDataSize;
    return NULL;
  }
  else
  {
    return s_pbyControlRequestData;
  }
}


/*******************************************************************************
* Function Name  : USB_Data_Setup.
* Description    : Handle the data class specific requests..
* Input          : RequestNo.
* Output         : None.
* Return         : RESULT.
*******************************************************************************/
RESULT USB_Data_Setup(uint8_t RequestNo)
{
  if ( ( VENDOR_REQUEST == (pInformation->USBbmRequestType & REQUEST_TYPE) ) &&
         ( INTERFACE_RECIPIENT == (pInformation->USBbmRequestType & RECIPIENT) ) &&
         ( usbifWinUSBComm == pInformation->USBwIndexs.bw.bb1 ) )
  {
    pInformation->Ctrl_Info.CopyData = usb_ControlCopyData;
    s_wControlRequestDataSize = WinUSBComm_Control(RequestNo, &s_pbyControlRequestData, pInformation->USBwLengths.w);
    pInformation->Ctrl_Info.Usb_wLength = s_wControlRequestDataSize;
  }

  return USB_SUCCESS; // USB_UNSUPPORT
}

/*******************************************************************************
* Function Name  : USB_NoData_Setup.
* Description    : Handle the no data class specific requests.
* Input          : RequestNo.
* Output         : None.
* Return         : RESULT.
*******************************************************************************/
RESULT USB_NoData_Setup(uint8_t RequestNo)
{
  if ( ( VENDOR_REQUEST == (pInformation->USBbmRequestType & REQUEST_TYPE) ) &&
       ( INTERFACE_RECIPIENT == (pInformation->USBbmRequestType & RECIPIENT) ) &&
       ( usbifWinUSBComm == pInformation->USBwIndexs.bw.bb1 ) )
  {
    WinUSBComm_Control(RequestNo, NULL, 0);
  }

  return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : USB_Get_Interface_Setting
* Description    : Test the interface and the alternate setting according to the
*                  supported one.
* Input          : uint8_t Interface, uint8_t AlternateSetting.
* Output         : None.
* Return         : RESULT.
*******************************************************************************/
RESULT USB_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
  if (AlternateSetting > 0)
  {
    return USB_UNSUPPORT; /* no AlternateSetting */
  }
  if (Interface < usbifNumInterfaces)
  {
    return USB_SUCCESS;
  }
  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : USB_GetDeviceDescriptor
* Description    : Get the device descriptor.
* Input          : uint16_t Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint8_t *USB_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor );
}

/*******************************************************************************
* Function Name  : USB_GetConfigDescriptor
* Description    : Get the configuration descriptor.
* Input          : uint16_t Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint8_t *USB_GetConfigDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Config_Descriptor );
}

/*******************************************************************************
* Function Name  : USB_GetStringDescriptor
* Description    : Get the string descriptors according to the needed index.
* Input          : uint16_t Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint8_t *USB_GetStringDescriptor(uint16_t Length)
{
  uint8_t wValue0 = pInformation->USBwValue0;

  if (wValue0 >= usdsdNumUSBStringDescriptors)
  {
    return NULL;
  }
  else
  {
    return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
  }
}
