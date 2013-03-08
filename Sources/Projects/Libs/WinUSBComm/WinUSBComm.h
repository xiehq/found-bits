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

#ifndef __WINUSB_COMM_H__
#define __WINUSB_COMM_H__

// WinUSBComm implements; called from USB implementation
void WinUSBComm_In();                       ///< Called to transfer the data to the USB endpoint.

void WinUSBComm_Out();                      ///< Called to transfer the data from the USB endpoint.
                                            ///< After the complete packet is received the
                                            ///< WinUSBComm_ProcessNewData calls the WinUSBComm_OnNewData callback.


uint16_t WinUSBComm_Control(uint8_t byRequest, uint8_t **ppData, uint16_t wControlDataSizeToBeReceived);   ///< Called for the communication control. Used for communication reset,
                                                                    ///< obtaining the response packet size and
                                                                    ///< signaling that the complete packet has been sent to device.




// WinUSBComm implements; called from user code
void WinUSBComm_ProcessNewData();                               ///< Poll. When new data packet available the WinUSBComm_OnNewData callback called.

uint32_t WinUSBComm_GetAvailableByteCount();                    ///< Returns the available byte count in the received packet.
                                                                ///< Available count is decreased for read size on each WinUSBComm_ReadData call.
                                                                ///< Call only in the WinUSBComm_OnNewData callback.

uint8_t * WinUSBComm_ReadData(uint32_t dwLength);               ///< Returns the copy of internal pointer to received packet.
                                                                ///< Advances the internal packet for dwLength.
                                                                ///< Call only in the WinUSBComm_OnNewData callback.

void WinUSBComm_WriteData(uint8_t *pbyData, uint32_t dwLength); ///< Writes (copies) the data to the transmit buffer.
                                                                ///< The data in buffer is sent after the WinUSBComm_OnNewData callback returns.
                                                                ///< Call only in the WinUSBComm_OnNewData callback.

// User implements; called from WinUSBComm
void WinUSBComm_OnNewData();  ///< Called from WinUSBComm_ProcessNewData after complete packet is transfered to device.
                              ///< On call read received data with WinUSBComm_ReadData and process accordingly.
                              ///< Return response data with calls to WinUSBComm_WriteData.

  // endpoint handling functions
uint32_t WinUSBComm_GetOutEndpointBufferSize();                 ///< Must return the number of bytes that are transfered in one USB packet. This should always be same value.
void WinUSBComm_SetOutEndpointReady();                          ///< USB implementation must allow data reception.
void WinUSBComm_ErrorOnOutEndpoint();                           ///< USB implementation must stall reception.
uint32_t WinUSBComm_GetNumBytesToReadFromOutEndpoint();         ///< Must return how many bytes were transfered in last USB packet.
uint32_t WinUSBComm_ReadFromOutEndpoint(uint8_t *pbyData);      ///< Must copy received data from endpoint buffer.

uint32_t WinUSBComm_GetInEndpointBufferSize();                  ///< Must return the number of bytes that are transfered in one USB packet. This should always be same value.
void WinUSBComm_SetInEndpointReady();                           ///< USB implementation can send data.
void WinUSBComm_ErrorOnInEndpoint();                            ///< USB implementation must stall transmission.
void WinUSBComm_WriteToInEndpoint(uint8_t *pbyData, uint32_t dwByteCount); ///< Must copy data to endpoint buffer.


#endif  //  __WINUSB_COMM_H__
