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

#ifndef __USB_PROP_H__
#define __USB_PROP_H__

#define USB_GetConfiguration          NOP_Process
/* #define USB_SetConfiguration          NOP_Process*/
#define USB_GetInterface              NOP_Process
#define USB_SetInterface              NOP_Process
#define USB_GetStatus                 NOP_Process
#define USB_ClearFeature              NOP_Process
#define USB_SetEndPointFeature        NOP_Process
#define USB_SetDeviceFeature          NOP_Process
/*#define USB_SetDeviceAddress          NOP_Process*/

void USB_init(void);
void USB_Reset(void);
void USB_SetConfiguration(void);
void USB_SetDeviceAddress (void);
void USB_Status_In (void);
void USB_Status_Out (void);
RESULT USB_Data_Setup(uint8_t);
RESULT USB_NoData_Setup(uint8_t);
RESULT USB_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *USB_GetDeviceDescriptor(uint16_t );
uint8_t *USB_GetConfigDescriptor(uint16_t);
uint8_t *USB_GetStringDescriptor(uint16_t);

#endif  //  __USB_PROP_H__
