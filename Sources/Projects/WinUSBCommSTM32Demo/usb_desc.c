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

#include "usb_desc.h"
#include "usb_conf.h"

const uint8_t DeviceDescriptor[] =
  {
    SIZ_DEVICE_DESC,   /* bLength  */
    0x01,   /* bDescriptorType */
    0x00,   /* bcdUSB, version 2.00 */
    0x02,
    0x00,   /* bDeviceClass : each interface define the device class */
    0x00,   /* bDeviceSubClass */
    0x00,   /* bDeviceProtocol */
    ENDP0_BUFFSIZE,   /* bMaxPacketSize0 */
    BYTE0(USB_VID),   /* idVendor */
    BYTE1(USB_VID),
    BYTE0(USB_PID),   /* idProduct */
    BYTE1(USB_PID),
    0x00,   /* bcdDevice 2.00*/
    0x02,
    usbsdVendor,  /* index of string Manufacturer  */
    usbsdProduct, /* index of string descriptor of product*/
    usbsdSerial,  /* Index of Serial Number String Descriptor */
    0x01    /*bNumConfigurations */
  };
COMPILETIME_ASSERT(sizeof(DeviceDescriptor) == SIZ_DEVICE_DESC);

const uint8_t ConfigDescriptor[] =
  {

    0x09,   /* bLength: Configuration Descriptor size */
    0x02,   /* bDescriptorType: Configuration */
    SIZ_CONFIG_DESC,

    0x00,
    usbifNumInterfaces,   /* bNumInterfaces */
    0x01,   /* bConfigurationValue: Configuration value */
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,   /* bmAttributes: bus powered */
    0xFA,   /* MaxPower 500 mA */

    /* 09 */
    0x09,   /* bLength: Interface Descriptor size */
    0x04,   /* bDescriptorType: Interface descriptor type */
    usbifWinUSBComm,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints*/
    0xFF,   /* bInterfaceClass: Vendor Class */
    0x00,   /* bInterfaceSubClass : none (WinUSB?)*/
    0x00,   /* nInterfaceProtocol */
    4,      /* iInterface: */
    0x07,   /* Endpoint descriptor length = 7*/
    0x05,   /* Endpoint descriptor type */
    0x81,   /* Endpoint address (IN, address 1) */
    0x02,   /* Bulk endpoint type */
    ENDP1_TXBUFFSIZE,   /* Maximum packet size */
    0x00,
    0x00,   /* Polling interval in milliseconds */
    0x07,   /* Endpoint descriptor length = 7 */
    0x05,   /* Endpoint descriptor type */
    0x02,   /* Endpoint address (OUT, address 2) */
    0x02,   /* Bulk endpoint type */
    ENDP2_RXBUFFSIZE,   /* Maximum packet size */
    0x00,
    0x00    /*Polling interval in milliseconds*/
  };
COMPILETIME_ASSERT(sizeof(ConfigDescriptor) == SIZ_CONFIG_DESC);

const uint8_t StringLangID[] =
{
    SIZ_STRING_LANGID,
    0x03,
    0x09,
    0x04
};      /* LangID = 0x0409: U.S. English */
COMPILETIME_ASSERT(sizeof(StringLangID) == SIZ_STRING_LANGID);

const char * g_pcszUSBVendorName = "Awesome Embedded Developer";
const char * g_pcszUSBProductName = "WinUSBComm Device";
const char * g_pcszWinUSBUSBInterfaceName = "WinUSBComm";
