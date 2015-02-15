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

// Most source in this file is copied from Microsoft MSDN site 
// http://msdn.microsoft.com/en-us/library/windows/hardware/ff540174(v=vs.85).aspx:
// Dev Center - Hardware > Learn > Drivers > Windows Driver Development >
// Device and Driver Technologies > Bus and Port Drivers >
// Universal Serial Bus (USB) Drivers > System-Supplied USB Drivers > 
// WinUSB > How to Access a USB Device by Using WinUSB Functions

#include "stdafx.h"
#include "WinUSBComm.h"

// Include Windows headers
#include <strsafe.h>

// Include WinUSB headers
#include <winusb.h>
#include <Usb100.h>
#include <Setupapi.h>

#include "WinUSBCommShared.h"
#include "WinUSBComm2Shared.h"

#define PTRACE(...)

//////////////////////////////////////////////////////////////////////////
BOOL GetDeviceHandle (GUID guidDeviceInterface, PHANDLE hDeviceHandle)
{
  if (guidDeviceInterface==GUID_NULL)
  {
    return FALSE;
  }

  BOOL bResult = TRUE;
  HDEVINFO hDeviceInfo;
  SP_DEVINFO_DATA DeviceInfoData;

  SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
  PSP_DEVICE_INTERFACE_DETAIL_DATA pInterfaceDetailData = NULL;

  ULONG requiredLength=0;

  LPTSTR lpDevicePath = NULL;

  DWORD index = 0;

  // Get information about all the installed devices for the specified
  // device interface class.
  hDeviceInfo = SetupDiGetClassDevs( 
    &guidDeviceInterface,
    NULL, 
    NULL,
    DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

  if (hDeviceInfo == INVALID_HANDLE_VALUE) 
  { 
    // ERROR 
    PTRACE("Error SetupDiGetClassDevs: %d.\n", GetLastError());
    goto done;
  }

  //Enumerate all the device interfaces in the device information set.
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

  for (index = 0; SetupDiEnumDeviceInfo(hDeviceInfo, index, &DeviceInfoData); index++)
  {
    //Reset for this iteration
    if (lpDevicePath)
    {
      LocalFree(lpDevicePath);
    }
    if (pInterfaceDetailData)
    {
      LocalFree(pInterfaceDetailData);
    }

    deviceInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

    //Get information about the device interface.
    bResult = SetupDiEnumDeviceInterfaces( 
      hDeviceInfo,
      &DeviceInfoData,
      &guidDeviceInterface,
      0, 
      &deviceInterfaceData);

    // Check if last item
    if (GetLastError () == ERROR_NO_MORE_ITEMS)
    {
      break;
    }

    //Check for some other error
    if (!bResult) 
    {
      printf("Error SetupDiEnumDeviceInterfaces: %d.\n", GetLastError());
      goto done;
    }

    //Interface data is returned in SP_DEVICE_INTERFACE_DETAIL_DATA
    //which we need to allocate, so we have to call this function twice.
    //First to get the size so that we know how much to allocate
    //Second, the actual call with the allocated buffer

    bResult = SetupDiGetDeviceInterfaceDetail(
      hDeviceInfo,
      &deviceInterfaceData,
      NULL, 0,
      &requiredLength,
      NULL);


    //Check for some other error
    if (!bResult) 
    {
      if ((ERROR_INSUFFICIENT_BUFFER==GetLastError()) && (requiredLength>0))
      {
        //we got the size, allocate buffer
        pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)LocalAlloc(LPTR, requiredLength);

        if (!pInterfaceDetailData) 
        { 
          // ERROR 
          printf("Error allocating memory for the device detail buffer.\n");
          goto done;
        }
      }
      else
      {
        printf("Error SetupDiEnumDeviceInterfaces: %d.\n", GetLastError());
        goto done;
      }
    }

    //get the interface detailed data
    pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    //Now call it with the correct size and allocated buffer
    bResult = SetupDiGetDeviceInterfaceDetail(
      hDeviceInfo,
      &deviceInterfaceData,
      pInterfaceDetailData,
      requiredLength,
      NULL,
      &DeviceInfoData);

    //Check for some other error
    if (!bResult) 
    {
      printf("Error SetupDiGetDeviceInterfaceDetail: %d.\n", GetLastError());
      goto done;
    }

    //copy device path

    size_t nLength = _tcslen(pInterfaceDetailData->DevicePath) + 1;  
    lpDevicePath = (TCHAR *) LocalAlloc (LPTR, nLength * sizeof(TCHAR));
    StringCchCopy(lpDevicePath, nLength, pInterfaceDetailData->DevicePath);
    lpDevicePath[nLength-1] = 0;

    printf("Device path:  %s\n", lpDevicePath);

  }

  if (!lpDevicePath)
  {
    //Error.
    printf("Error %d.", GetLastError());
    goto done;
  }

  //Open the device
  *hDeviceHandle = CreateFile (
    lpDevicePath,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,
    OPEN_EXISTING,
    FILE_FLAG_OVERLAPPED,
    NULL);

  if (*hDeviceHandle == INVALID_HANDLE_VALUE)
  {
    //Error.
    printf("Error %d.", GetLastError());
    goto done;
  }



done:
  LocalFree(lpDevicePath);
  LocalFree(pInterfaceDetailData);    
  bResult = SetupDiDestroyDeviceInfoList(hDeviceInfo);

  return bResult;
}
//////////////////////////////////////////////////////////////////////////
BOOL GetWinUSBHandle(HANDLE hDeviceHandle, PWINUSB_INTERFACE_HANDLE phWinUSBHandle)
{
  if (hDeviceHandle == INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  BOOL bResult = WinUsb_Initialize(hDeviceHandle, phWinUSBHandle);
  if(!bResult)
  {
    //Error.
    printf("WinUsb_Initialize Error %d.", GetLastError());
    return FALSE;
  }

  return bResult;
}
//////////////////////////////////////////////////////////////////////////
BOOL GetUSBDeviceSpeed(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pDeviceSpeed)
{
  if (!pDeviceSpeed || hDeviceHandle==INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  BOOL bResult = TRUE;

  ULONG length = sizeof(UCHAR);

  bResult = WinUsb_QueryDeviceInformation(hDeviceHandle, DEVICE_SPEED, &length, pDeviceSpeed);
  if(!bResult)
  {
    printf("Error getting device speed: %d.\n", GetLastError());
    goto done;
  }

  if(*pDeviceSpeed == 1)
  {
    printf("Device speed: %d - Full speed or lower (initial documentation on MSDN was wrong).\n", *pDeviceSpeed);
    goto done;
  }
  if(*pDeviceSpeed == HighSpeed)
  {
    printf("Device speed: %d - High speed.\n", *pDeviceSpeed);
    goto done;
  }

done:
  return bResult;
}
//////////////////////////////////////////////////////////////////////////
struct PIPE_ID
{
  UCHAR  PipeInId;
  UCHAR  PipeOutId;
};

BOOL QueryDeviceEndpoints (WINUSB_INTERFACE_HANDLE hDeviceHandle, PIPE_ID* pipeid)
{
  if (hDeviceHandle==INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  BOOL bResult = TRUE;

  USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
  ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

  WINUSB_PIPE_INFORMATION  Pipe;
  ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));


  bResult = WinUsb_QueryInterfaceSettings(hDeviceHandle, 0, &InterfaceDescriptor);

  if (bResult)
  {
    for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++)
    {
      bResult = WinUsb_QueryPipe(hDeviceHandle, 0, index, &Pipe);

      if (bResult)
      {
        if (Pipe.PipeType == UsbdPipeTypeControl)
        {
          printf("Endpoint index: %d Pipe type: Control Pipe ID: %d.\n", index, Pipe.PipeId);
        }
        if (Pipe.PipeType == UsbdPipeTypeIsochronous)
        {
          printf("Endpoint index: %d Pipe type: Isochronous Pipe ID: %d.\n", index, Pipe.PipeId);
        }
        if (Pipe.PipeType == UsbdPipeTypeBulk)
        {
          if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
          {
            printf("Endpoint index: %d Pipe type: Bulk Pipe ID: 0x%02X.\n", index, Pipe.PipeId);
            pipeid->PipeInId = Pipe.PipeId;
          }
          if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId))
          {
            printf("Endpoint index: %d Pipe type: Bulk Pipe ID: 0x%02X.\n", index, Pipe.PipeId);
            pipeid->PipeOutId = Pipe.PipeId;
          }

        }
        if (Pipe.PipeType == UsbdPipeTypeInterrupt)
        {
          printf("Endpoint index: %d Pipe type: Interrupt Pipe ID: %d.\n", index, Pipe.PipeId);
        }
      }
      else
      {
        continue;
      }
    }
  }
  return bResult;
}
//////////////////////////////////////////////////////////////////////////
BOOL SendDatatoDefaultEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, BYTE byWinUSBCommControl, BYTE *pbyData = NULL, WORD wNumBytesCount = 0)
{
  if (hDeviceHandle==INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  BOOL bResult = TRUE;
  WINUSB_SETUP_PACKET SetupPacket;
  ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
  ULONG cbSent = 0;

  //Create the setup packet
  SetupPacket.RequestType = (BMREQUEST_HOST_TO_DEVICE << 7) | (BMREQUEST_VENDOR << 5) | BMREQUEST_TO_INTERFACE;
  SetupPacket.Request = byWinUSBCommControl;
  SetupPacket.Value = 0;
  SetupPacket.Index = 0; // specify WinUSBComm interface
  SetupPacket.Length = wNumBytesCount;

  bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, pbyData, wNumBytesCount, &cbSent, 0);
  if(!bResult)
  {
    goto done;
  }

  PTRACE("Data sent: %d \nActual data transferred: %d.\n", wNumBytesCount, cbSent);


done:
  return bResult;

}
//////////////////////////////////////////////////////////////////////////
BOOL GetDataFromDefaultEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, BYTE byWinUSBCommControl, BYTE *pbyData, WORD wNumBytesCount)
{
  if ( 0 == wNumBytesCount )
  {
    return TRUE;
  }

  if ( NULL == pbyData )
  {
    return FALSE;
  }

  if (hDeviceHandle==INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  BOOL bResult = TRUE;
  WINUSB_SETUP_PACKET SetupPacket;
  ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
  ULONG cbSent = 0;

  //Create the setup packet
  SetupPacket.RequestType = (BMREQUEST_DEVICE_TO_HOST << 7) | (BMREQUEST_VENDOR << 5) | BMREQUEST_TO_INTERFACE;
  SetupPacket.Request = byWinUSBCommControl;
  SetupPacket.Value = 0;
  SetupPacket.Index = 0; // specify WinUSBComm interface
  SetupPacket.Length = wNumBytesCount;

  bResult = WinUsb_ControlTransfer(hDeviceHandle, SetupPacket, pbyData, wNumBytesCount, &cbSent, 0);
  if(!bResult)
  {
    goto done;
  }

  PTRACE("Data get : %d \nActual data transferred: %d.\n", wNumBytesCount, cbSent);


done:
  return bResult;

}
//////////////////////////////////////////////////////////////////////////
BOOL WriteToBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG* pcbWritten, BYTE *pbyData, DWORD dwNumBytesCount)
{
  if ( 0 == dwNumBytesCount )
  {
    return TRUE;
  }

  if ( NULL == pbyData )
  {
    return FALSE;
  }

  if (hDeviceHandle==INVALID_HANDLE_VALUE || !pID || !pcbWritten)
  {
    return FALSE;
  }

  BOOL bResult = TRUE;
  ULONG cbSent = 0;

  bResult = WinUsb_WritePipe(hDeviceHandle, *pID, pbyData, dwNumBytesCount, &cbSent, 0);
  if(!bResult)
  {
    goto done;
  }

  PTRACE("Wrote to pipe %d: %d bytes\nActual data transferred: %d.\n", *pID, dwNumBytesCount, cbSent);
  *pcbWritten = cbSent;


done:
  return bResult;

}
//////////////////////////////////////////////////////////////////////////
BOOL ReadFromBulkEndpoint(WINUSB_INTERFACE_HANDLE hDeviceHandle, UCHAR* pID, ULONG cbSize, BYTE *pbyData, DWORD dwNumBytesCount)
{
  if ( 0 == dwNumBytesCount )
  {
    return TRUE;
  }

  if ( NULL == pbyData )
  {
    return FALSE;
  }

  if (hDeviceHandle==INVALID_HANDLE_VALUE)
  {
    return FALSE;
  }

  BOOL bResult = TRUE;
  ULONG cbRead = 0;

  bResult = WinUsb_ReadPipe(hDeviceHandle, *pID, pbyData, dwNumBytesCount, &cbRead, 0);
  if(!bResult)
  {
    goto done;
  }

  PTRACE("Read from pipe %d: %d bytes\nActual data read: %d.\n", *pID, dwNumBytesCount, cbRead);


done:
  return bResult;

}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//// MAIN
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
  GUID guidDeviceInterface = CWinUSBComm::m_WinUSBCommInterfaceGUID; //in the INF file

  BOOL bResult = TRUE;

  PIPE_ID PipeID;

  HANDLE hDeviceHandle = INVALID_HANDLE_VALUE;
  WINUSB_INTERFACE_HANDLE hWinUSBHandle = INVALID_HANDLE_VALUE;

  UCHAR DeviceSpeed;
  ULONG cbSize = 0;

  BYTE * pbyDataToDevice = NULL;
  BYTE * pbyDataFromDevice = NULL;

  SetConsoleTitle(_T("WinUSBComm Test"));

  bResult = GetDeviceHandle(guidDeviceInterface, &hDeviceHandle);
  if(!bResult)
  {
    goto done;
  }

  bResult = GetWinUSBHandle(hDeviceHandle, &hWinUSBHandle);
  if(!bResult)
  {
    goto done;
  }

  bResult = GetUSBDeviceSpeed(hWinUSBHandle, &DeviceSpeed);
  if(!bResult)
  {
    goto done;
  }

  bResult = QueryDeviceEndpoints(hWinUSBHandle, &PipeID);
  if(!bResult)
  {
    goto done;
  }

  BOOL bRepeat = FALSE;

  BYTE byVersion = winusbcommversion1;

  bResult = GetDataFromDefaultEndpoint(hWinUSBHandle, winusbcomm2commandGetVersion, (BYTE *)&byVersion, sizeof(byVersion));
  
  EWinUSBCommVersion eVersion = ( byVersion <= winusbcommversion1 ) ? winusbcommversion1 : (EWinUSBCommVersion)byVersion;

  BYTE byGetCommBufferSizeCmd = ( winusbcommversion1 == eVersion ) ? winusbctrlGETBUFFSIZE : winusbcomm2commandGetBufferSize;
  BYTE byResetCmd = ( winusbcommversion1 == eVersion ) ? winusbctrlRESET : winusbcomm2commandReset;
  BYTE byGetReturnSizeCmd = ( winusbcommversion1 == eVersion ) ? winusbctrlGETDATASIZE : winusbcomm2commandGetReturnSize;

  DWORD dwMaxBufferSize = 0;
  bResult = GetDataFromDefaultEndpoint(hWinUSBHandle, byGetCommBufferSizeCmd, (BYTE *)&dwMaxBufferSize, sizeof(dwMaxBufferSize));
  if(!bResult)
  {
    goto done;
  }           

  pbyDataToDevice = new BYTE[dwMaxBufferSize];
  for ( DWORD I = 0; I < dwMaxBufferSize; I++ )
  {
    pbyDataToDevice[I] = (BYTE) I;
  }
  pbyDataFromDevice = new BYTE[dwMaxBufferSize];

  LARGE_INTEGER countStart;
  DWORD dwNumDataTransered = 0;
  DWORD dwNumLoops = 100;
  QueryPerformanceCounter(&countStart);

  if ( winusbcommversion1 == eVersion )
  {
    // send some data over control EP
    BYTE abyCtrlEPTestData[4] = { 4, 3, '2', '1' };
    bResult = SendDatatoDefaultEndpoint(hWinUSBHandle, winusbctrlEXAMPLEDATA4B, abyCtrlEPTestData, 4);
    if(!bResult)
    {
      goto done;
    }
  }
  do
  {
    // put device to receiving state
    bResult = SendDatatoDefaultEndpoint(hWinUSBHandle, byResetCmd);
    if(!bResult)
    {
      goto done;
    }

    if ( winusbcommversion1 != eVersion )
    {
      // notify device how much data will be sent next
      bResult = SendDatatoDefaultEndpoint(hWinUSBHandle, winusbcomm2commandFollowingPacketSize, (BYTE *)&dwMaxBufferSize, 4);
      if(!bResult)
      {
        goto done;
      }
    }

    // send the data
    bResult = WriteToBulkEndpoint(hWinUSBHandle, &PipeID.PipeOutId, &cbSize, pbyDataToDevice, dwMaxBufferSize);
    if(!bResult)
    {
      goto done;
    }

    dwNumDataTransered += dwMaxBufferSize;

    if ( winusbcommversion1 == eVersion )
    {
      // notify device that all is sent
      bResult = SendDatatoDefaultEndpoint(hWinUSBHandle, winusbctrlTXDONE);
      if(!bResult)
      {
        goto done;
      }

      // poll status and wait until idle
      BYTE byCommStatus = winusbcommPROCESSING;
      while ( winusbcommPROCESSING == byCommStatus )
      {
        bResult = GetDataFromDefaultEndpoint(hWinUSBHandle, winusbctrlGETSTATUS, &byCommStatus, sizeof(byCommStatus));
        if(!bResult)
        {
          goto done;
        }
        dwNumDataTransered += 1;
        // timeout here if needed
      }

      // check if idle
      if ( winusbcommIDLE != byCommStatus )
      {
        goto done;
      }
    }

    // get how much data is to read from device
    DWORD dwResponseByteCount = 0;
    do
    {
      bResult = GetDataFromDefaultEndpoint(hWinUSBHandle, byGetReturnSizeCmd, (BYTE *)&dwResponseByteCount, sizeof(dwResponseByteCount));
      if(!bResult)
      {
        goto done;
      }
      // timeout here if needed
    } while ( !dwResponseByteCount );
    dwNumDataTransered += sizeof(dwResponseByteCount);
    
    // response byte count must match sent count; application XORs the data and sends it back
    if ( dwMaxBufferSize != dwResponseByteCount )
    {
      goto done;
    }

    // read data back
    bResult = ReadFromBulkEndpoint(hWinUSBHandle, &PipeID.PipeInId, cbSize, pbyDataFromDevice, dwResponseByteCount);
    if(!bResult)
    {
      goto done;
    }

    dwNumDataTransered += dwResponseByteCount;

// At first I had data XORed in the device and sent back. I was later interested in transfer speed.
//     BOOL bMatch = TRUE;
//     for ( DWORD I = 0; I < dwResponseByteCount; I++ )
//     {
//       if ( pbyDataToDevice[I] != (pbyDataFromDevice[I]))// ^ 0xFF) )
//       {
//         bMatch = FALSE;
//         break;
//       }
//     }
// 
//     printf("Data %s\n", bMatch ? "matched" : "didn't match");
  } while ( --dwNumLoops );

  LARGE_INTEGER countEnd;
  QueryPerformanceCounter(&countEnd);

  LARGE_INTEGER f;
  QueryPerformanceFrequency( &f );

  LARGE_INTEGER elapsedCount;
  elapsedCount.QuadPart = countEnd.QuadPart - countStart.QuadPart;

  DOUBLE elapsed = (DOUBLE)elapsedCount.QuadPart / (DOUBLE)f.QuadPart;
  DOUBLE rate = (DOUBLE)dwNumDataTransered / elapsed;

  rate /= 1024;
  printf("Data speed: %f kbps = %f kBps\n", rate * 8, rate);
  system("PAUSE");
done:
  if ( INVALID_HANDLE_VALUE != hDeviceHandle )
  {
    CloseHandle(hDeviceHandle);
  }
  if ( INVALID_HANDLE_VALUE != hWinUSBHandle )
  {
    WinUsb_Free(hWinUSBHandle);
  }
  if ( pbyDataToDevice ) delete[] pbyDataToDevice;
  if ( pbyDataFromDevice ) delete[] pbyDataFromDevice;
}

