/////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2014, pa.eeapai@gmail.com                                         //
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

#include "StdAfx.h"

#include "WinUSBCommDevice.h"

#include "SetupAPIWrapper.h"

#include "WinUSBCommShared.h"
#include "WinUSBComm2Shared.h"

#ifdef _DEBUG
#define PTRACE(...) //TRACE(__VA_ARGS__)
#else
#define PTRACE(...)
#endif

#define INVALID_PIPE_ID 0xFF

// {EA0BD5C3-50F3-4888-84B4-74E50E1649DB}
GUID CWinUSBCommDevice::sm_WinUSBCommInterfaceGUID = 
{ 0xEA0BD5C3, 0x50F3, 0x4888, { 0x84, 0xB4, 0x74, 0xE5, 0x0E, 0x16, 0x49, 0xDB } };

CWinUSBCommDevice::CWinUSBCommDevice(void) :
  m_hDeviceHandle(INVALID_HANDLE_VALUE),
  m_hWinUSBHandle(INVALID_HANDLE_VALUE),
  m_byPipeInId(INVALID_PIPE_ID),
  m_byPipeOutId(INVALID_PIPE_ID),
  m_dwCommBufferSizeInBytes(0)
{
}


CWinUSBCommDevice::~CWinUSBCommDevice(void)
{
  Disconnect();
}


static BOOL setupDiEnumDeviceInfo(HDEVINFO hDeviceInfo,
                                  SP_DEVINFO_DATA &DeviceInfoData,
                                  DWORD I,
                                  BOOL &rbListed)
{
  rbListed = FALSE;
  ZeroMemory(&DeviceInfoData, sizeof(SP_DEVINFO_DATA));
  DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
  BOOL bResult = CSetupAPIWrapper::SetupDiEnumDeviceInfo(hDeviceInfo, I, &DeviceInfoData);
  if ( bResult )
  {
    rbListed = TRUE;
    return TRUE;
  }
  if ( GetLastError() == ERROR_NO_MORE_ITEMS )
  {
    return TRUE;
  }
  PTRACE("Error SetupDiEnumDeviceInfo: %d.\n", GetLastError());
  return FALSE;
}

static BOOL getStringDescriptor(WINUSB_INTERFACE_HANDLE hWinUSBHandle, BYTE byIndex, CWinUSBCommDevice::string &rstrDescriptor)
{
  BYTE abyDescriptor[1024] = {0};
  DWORD dwDescriptorSize = 0;

  rstrDescriptor.clear();

  BOOL bResult = CWinUSBWrapper::GetDescriptor(hWinUSBHandle, USB_STRING_DESCRIPTOR_TYPE, byIndex, 0, abyDescriptor, sizeof(abyDescriptor), &dwDescriptorSize);

  PUSB_STRING_DESCRIPTOR pVD = (PUSB_STRING_DESCRIPTOR)abyDescriptor;

  if ( bResult )
  {
// #ifdef UNICODE
    rstrDescriptor = W2STR(pVD->bString);
// #else
//     int nLength = WideCharToMultiByte(CP_UTF8, 0, pVD->bString, (pVD->bLength - 2) >> 1, NULL, 0, NULL, NULL);
//     if ( !nLength )
//     {
//       return FALSE;
//     }
//     LPSTR pszDescriptor = new CHAR[nLength + 1];
//     ZeroMemory(pszDescriptor, (nLength + 1));
//     nLength = WideCharToMultiByte(CP_UTF8, 0, pVD->bString, (pVD->bLength - 2) >> 1, pszDescriptor, nLength, NULL, NULL);
//     bResult = 0 != nLength;
//     rstrDescriptor = pszDescriptor;
//     delete[] pszDescriptor;
// #endif
  }
  return bResult;
}

void makeName(const CWinUSBCommDevice::string &rstrVendor,
              const CWinUSBCommDevice::string &rstrProduct,
              const CWinUSBCommDevice::string &rstrSerial,
              BOOL bLong,                                   ///< Include vendor name
              CWinUSBCommDevice::string &rstrName)
{
  rstrName.clear();

  BOOL bSerial = FALSE;
  for ( DWORD I = 0; I < rstrSerial.length(); I++ )
  {
    if ( '0' != rstrSerial[I] )
    {
      bSerial = TRUE;
    }
  }

  if ( bLong )
  {
    rstrName = rstrVendor + " - ";
  }
  rstrName += rstrProduct;

  if ( bSerial )
  {
    rstrName += " (" + rstrSerial + ")";
  }
}

///
/// Gets device name and path.
///
static BOOL getDeviceInfo(HDEVINFO hDeviceInfo,
                          SP_DEVINFO_DATA &DeviceInfoData,
                          const GUID &rGUID,
                          DWORD I,
                          WORD &rwVID,
                          WORD &rwPID,
                          BOOL &rbListed,
                          CWinUSBCommDevice::string &rstrName,
                          CWinUSBCommDevice::string &rstrPath)
{
  BOOL bResult = TRUE;
  rstrName.clear();
  rstrPath.clear();

  if ( bResult )
  {
    bResult = setupDiEnumDeviceInfo(hDeviceInfo, DeviceInfoData, I, rbListed);
  }

  if ( bResult && !rbListed )
  {
    return TRUE;
  }

  if ( !bResult )
  {
    return FALSE;
  }

  SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
  deviceInterfaceData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

  //Get information about the device interface.
  bResult = CSetupAPIWrapper::SetupDiEnumDeviceInterfaces( 
    hDeviceInfo,
    &DeviceInfoData,
    &rGUID,
    0, 
    &deviceInterfaceData);

  //Check for some other error
  if ( !bResult ) 
  {
    PTRACE("Error SetupDiEnumDeviceInterfaces: %d.\n", GetLastError());
    return FALSE;
  }

  //Interface data is returned in SP_DEVICE_INTERFACE_DETAIL_DATA
  //which we need to allocate, so we have to call this function twice.
  //First to get the size so that we know how much to allocate
  //Second, the actual call with the allocated buffer
  ULONG requiredLength = 0;
  bResult = CSetupAPIWrapper::SetupDiGetDeviceInterfaceDetailW(
    hDeviceInfo,
    &deviceInterfaceData,
    NULL, 0,
    &requiredLength,
    NULL);

  PSP_DEVICE_INTERFACE_DETAIL_DATA_W pInterfaceDetailData = NULL;

  //Check for some other error
  if ( !bResult ) 
  {
    if ( ( ERROR_INSUFFICIENT_BUFFER == GetLastError() ) && ( requiredLength > 0 ) )
    {
      bResult = TRUE;
      //we got the size, allocate buffer
      pInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)LocalAlloc(LPTR, requiredLength);

      if ( !pInterfaceDetailData ) 
      { 
        // ERROR 
        PTRACE("Error allocating memory for the device detail buffer.\n");
        return FALSE;
      }
    }
    else
    {
      PTRACE("Error SetupDiEnumDeviceInterfaces: %d.\n", GetLastError());
      return FALSE;
    }
  }

  //get the interface detailed data
  pInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

  //Now call it with the correct size and allocated buffer
  if ( bResult )
  {
    bResult = CSetupAPIWrapper::SetupDiGetDeviceInterfaceDetailW(
      hDeviceInfo,
      &deviceInterfaceData,
      pInterfaceDetailData,
      requiredLength,
      NULL,
      &DeviceInfoData);
  }

  //Check for some other error
  if ( bResult ) 
  {
    //copy device path
    rstrPath = W2STR(pInterfaceDetailData->DevicePath);
    PTRACE("Device path:  %s\n", rstrPath.c_str());

    if ( 0 )
    {
      DWORD dwDataType;
      BYTE abyBuffer[1024];
      DWORD dwBufferSize = sizeof(abyBuffer);

      for ( int n = 0; n < SPDRP_MAXIMUM_PROPERTY; ++n )
      {
        if ( bResult )
        {
          bResult = CSetupAPIWrapper::SetupDiGetDeviceRegistryPropertyW(hDeviceInfo, &DeviceInfoData, n, &dwDataType, (PBYTE)abyBuffer, sizeof(abyBuffer), &dwBufferSize);
        }
      }
    }
  }

  // NOTE: can't open twice but can Initialize twice
  HANDLE hDeviceHandle = INVALID_HANDLE_VALUE;
  if ( bResult )
  {
      hDeviceHandle = CreateFileW (
      A2CSTRW(rstrPath),
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_OVERLAPPED,
      NULL);

    if (hDeviceHandle == INVALID_HANDLE_VALUE)
    {
      //Error.
      PTRACE("Error %d.", GetLastError());
      bResult = FALSE;
    }
  }

  WINUSB_INTERFACE_HANDLE hWinUSBHandle = INVALID_HANDLE_VALUE;
  if ( bResult )
  {
    bResult = CWinUSBWrapper::Initialize(hDeviceHandle, &hWinUSBHandle);
    if( !bResult )
    {
      //Error.
      PTRACE("WinUsb_Initialize Error %d.", GetLastError());
      bResult = FALSE;
    }
  }
  BYTE abyDescriptor[1024] = {0};
  DWORD dwDescriptorSize = 0;
  if ( bResult )
  {
    // We'll get interface descriptor index in place of product string index
    bResult = CWinUSBWrapper::GetDescriptor(hWinUSBHandle, USB_DEVICE_DESCRIPTOR_TYPE, 0, 0, abyDescriptor, sizeof(abyDescriptor), &dwDescriptorSize);
  }

  PUSB_DEVICE_DESCRIPTOR pDD = (PUSB_DEVICE_DESCRIPTOR)abyDescriptor;
  rwVID = pDD->idVendor;
  rwPID = pDD->idProduct;
  BYTE byVendorStringIndex = pDD->iManufacturer;
  BYTE byProductStringIndex = pDD->iProduct;
  BYTE bySerialStringIndex = pDD->iSerialNumber;


  CWinUSBCommDevice::string strVendor;
  if ( bResult )
  {
    bResult = getStringDescriptor(hWinUSBHandle, byVendorStringIndex, strVendor);
  }

  CWinUSBCommDevice::string strProduct;
  if ( bResult )
  {
    bResult = getStringDescriptor(hWinUSBHandle, byProductStringIndex, strProduct);
  }

  CWinUSBCommDevice::string strSerial;
  if ( bResult )
  {
    bResult = getStringDescriptor(hWinUSBHandle, bySerialStringIndex, strSerial);
  }

  if ( bResult )
  {
    makeName(strVendor, strProduct, strSerial, TRUE, rstrName);
  }

  if ( INVALID_HANDLE_VALUE != hWinUSBHandle )
  {
    CWinUSBWrapper::Free(hWinUSBHandle);
  }

  if ( INVALID_HANDLE_VALUE != hDeviceHandle )
  {
    CloseHandle(hDeviceHandle);
  }
                                                     
  if ( pInterfaceDetailData )
  {
    LocalFree(pInterfaceDetailData);
  }
  return bResult;
}


BOOL CWinUSBCommDevice::ListDevices(TStringList &rNamesList, WORD wVID /* = 0 */, WORD wPID /* = 0 */)
{
  TStringList paths;
  return listDevices(rNamesList, paths, wVID, wPID);
}

BOOL CWinUSBCommDevice::listDevices(TStringList &rNamesList, TStringList &rPathsList, WORD wVID /* = 0 */, WORD wPID /* = 0 */)
{
  rPathsList.clear();
  rNamesList.clear();
  BOOL bResult = TRUE;
  HDEVINFO hDeviceInfo = INVALID_HANDLE_VALUE;
  if ( bResult )
  {
    // Get information about all the installed devices for the specified
    // device interface class.
    hDeviceInfo = CSetupAPIWrapper::SetupDiGetClassDevsW(&sm_WinUSBCommInterfaceGUID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  }

  if ( hDeviceInfo == INVALID_HANDLE_VALUE ) 
  { 
    PTRACE("Error SetupDiGetClassDevs: %d.\n", GetLastError());
    bResult = FALSE;
    return FALSE;
  }

  SP_DEVINFO_DATA DeviceInfoData;
  //Enumerate all the device interfaces in the device information set.
  for ( DWORD I = 0; ; I++ )
  {
    string strDevName;
    string strDevPath;
    BOOL bDeviceListed = FALSE;
    WORD wDetectedVID = 0;
    WORD wDetectedPID = 0;

    if ( bResult )
    {
      bResult = getDeviceInfo(hDeviceInfo, DeviceInfoData, sm_WinUSBCommInterfaceGUID, I, wDetectedVID, wDetectedPID, bDeviceListed, strDevName, strDevPath);
    }

    if ( !bResult )
    {
      break;  // stop on error
    }

    if ( !bDeviceListed )
    {
      break;  // stop if no more devices
    }

    if ( wVID && ( wVID != wDetectedVID ) )
    {
      continue; // skip if VID provided and doesn't match
    }

    if ( wPID && ( wPID != wDetectedPID ) )
    {
      continue; // skip if PID provided and doesn't match
    }

    BOOL bGotPath = !strDevPath.empty();
    BOOL bGotName = !strDevName.empty();
    if ( !bGotPath )
    {
      bResult = FALSE;  // what?
      break;  // stop on error
    }

    if ( bGotName )
    {
      BOOL bIsDuplicate = FALSE;
      for ( DWORD dwKnownDeviceIndex = 0; dwKnownDeviceIndex < rNamesList.size(); dwKnownDeviceIndex++ )
      {
        if ( rNamesList[dwKnownDeviceIndex] == strDevName )
        {
          bIsDuplicate = TRUE;
          break;
        }
      }
      if ( !bIsDuplicate )  // Skip duplicates. Only one interface is supported by this lib.
      {
        rNamesList.push_back(strDevName);
        rPathsList.push_back(strDevPath);
      }
    }
    else
    {
      PTRACE("Info Can't open device: %s.\n", strDevPath);  // Device already opened. WinUSB limitation - single access to device only.
    }
  }


  CSetupAPIWrapper::SetupDiDestroyDeviceInfoList(hDeviceInfo);
  return bResult;
}

BOOL CWinUSBCommDevice::getPath(LPCSTR pcszDevice, string &rstrPath)
{
  BOOL bResult = TRUE;

  TStringList names;
  TStringList paths;

  rstrPath.clear();

  if ( bResult )
  {
    bResult = listDevices(names, paths);
  }

  if ( !paths.size() )
  {
    return FALSE;
  }

  if ( bResult )
  {
    if ( pcszDevice )
    {
      if ( ( 0 == strlen(pcszDevice) ) && ( 1 == paths.size() ) )
      {
        rstrPath = paths[0];
        return TRUE;
      }
    }
    else
    {
      if ( 1 == paths.size() )
      {
        rstrPath = paths[0];
        return TRUE;
      }
    }
  }

  string strName = pcszDevice;
  if ( bResult )
  {
    for ( DWORD I = 0; I < names.size(); I++ )
    {
      if ( strName == names[I] )
      {
        rstrPath = paths[I];
        return TRUE;
      }
    }
  }

  return FALSE;
}

BOOL CWinUSBCommDevice::IsConnected() const
{
  return ( m_hDeviceHandle != INVALID_HANDLE_VALUE ) && ( m_hWinUSBHandle != INVALID_HANDLE_VALUE );
}

BOOL CWinUSBCommDevice::Connect(LPCSTR pcszDevice)
{
  BOOL bResult = TRUE;
  string strPath;

  if ( IsConnected() )
  {
    Disconnect();
  }

  if ( bResult )
  {
    bResult = getPath(pcszDevice, strPath);
  }

  if ( bResult )
  {
    m_hDeviceHandle = CreateFileW (
      A2CSTRW(strPath),
      GENERIC_READ | GENERIC_WRITE,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_FLAG_OVERLAPPED,
      NULL);

    if (m_hDeviceHandle == INVALID_HANDLE_VALUE)
    {
      //Error.
      PTRACE("Error %d.", GetLastError());
      bResult = FALSE;
    }
  }

  if ( bResult )
  {
    bResult = CWinUSBWrapper::Initialize(m_hDeviceHandle, &m_hWinUSBHandle);
    if( !bResult )
    {
      //Error.
      PTRACE("WinUsb_Initialize Error %d.", GetLastError());
      bResult = FALSE;
    }
  }

  if ( bResult )
  {
    bResult = queryDeviceEndpoints();
  }

  m_byDeviceVersion = winusbcommversion1;

  if ( bResult )
  {
    bResult = controlRead(winusbcomm2commandGetVersion, (BYTE *)&m_byDeviceVersion, sizeof(m_byDeviceVersion));
    switch ( m_byDeviceVersion )
    {
    case winusbcommversion1a:
    case winusbcommversion1b:
    case winusbcommversion1c:
    case winusbcommversion1:
      m_bycommandReset = winusbctrlRESET;
      m_bycommandGetState = winusbctrlGETSTATUS;
      m_bycommandGetBufferSize = winusbctrlGETBUFFSIZE;
      m_bycommandGetReturnSize = winusbctrlGETDATASIZE;
      break;
    case winusbcommversion2:
      m_bycommandReset = winusbcomm2commandReset;
      m_bycommandGetState = winusbcomm2commandGetState;
      m_bycommandGetBufferSize = winusbcomm2commandGetBufferSize;
      m_bycommandGetReturnSize = winusbcomm2commandGetReturnSize;
      break;
    default:
      bResult = FALSE;
      break;
    }
  }

  m_dwCommBufferSizeInBytes = 0;
  
  if ( bResult )
  {
    bResult = controlRead(m_bycommandGetBufferSize, (BYTE *)&m_dwCommBufferSizeInBytes, sizeof(m_dwCommBufferSizeInBytes));
  }

  if ( bResult )
  {
    bResult = reset();
  }

  if ( !bResult )
  {
    Disconnect();
  }

  return bResult;
}

BOOL CWinUSBCommDevice::queryDeviceEndpoints()
{
  BOOL bResult = IsConnected();

  USB_INTERFACE_DESCRIPTOR InterfaceDescriptor;
  ZeroMemory(&InterfaceDescriptor, sizeof(USB_INTERFACE_DESCRIPTOR));

  WINUSB_PIPE_INFORMATION  Pipe;
  ZeroMemory(&Pipe, sizeof(WINUSB_PIPE_INFORMATION));

  m_byPipeInId = INVALID_PIPE_ID;
  m_byPipeOutId = INVALID_PIPE_ID;

  if ( bResult )
  {
    bResult = CWinUSBWrapper::QueryInterfaceSettings(m_hWinUSBHandle, 0, &InterfaceDescriptor);
  }

  if (bResult)
  {
    for (int index = 0; index < InterfaceDescriptor.bNumEndpoints; index++)
    {
      bResult = CWinUSBWrapper::QueryPipe(m_hWinUSBHandle, 0, index, &Pipe);

      if (bResult)
      {
        if (Pipe.PipeType == UsbdPipeTypeControl)
        {
          PTRACE("Endpoint index: %d Pipe type: Control Pipe ID: %d.\n", index, Pipe.PipeId);
        }
        if (Pipe.PipeType == UsbdPipeTypeIsochronous)
        {
          printf("Endpoint index: %d Pipe type: Isochronous Pipe ID: %d.\n", index, Pipe.PipeId);
        }
        if (Pipe.PipeType == UsbdPipeTypeBulk)
        {
          if (USB_ENDPOINT_DIRECTION_IN(Pipe.PipeId))
          {
            PTRACE("Endpoint index: %d Pipe type: Bulk Pipe ID: 0x%02X.\n", index, Pipe.PipeId);
            m_byPipeInId = Pipe.PipeId;
          }
          if (USB_ENDPOINT_DIRECTION_OUT(Pipe.PipeId))
          {
            PTRACE("Endpoint index: %d Pipe type: Bulk Pipe ID: 0x%02X.\n", index, Pipe.PipeId);
            m_byPipeOutId = Pipe.PipeId;
          }

        }
        if (Pipe.PipeType == UsbdPipeTypeInterrupt)
        {
          PTRACE("Endpoint index: %d Pipe type: Interrupt Pipe ID: %d.\n", index, Pipe.PipeId);
        }
      }
      else
      {
        continue;
      }
    }
  }
  if ( INVALID_PIPE_ID == m_byPipeInId )
  {
    return FALSE;
  }
  if ( INVALID_PIPE_ID == m_byPipeOutId )
  {
    return FALSE;
  }
  return bResult;
}

BOOL CWinUSBCommDevice::controlWrite(BYTE byWinUSBCommControl, BYTE *pbyData /* = NULL */, WORD wNumBytesCount /* = 0 */)
{
  BOOL bResult = IsConnected();
  WINUSB_SETUP_PACKET SetupPacket;
  ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
  ULONG cbSent = 0;

  //Create the setup packet
  SetupPacket.RequestType = (BMREQUEST_HOST_TO_DEVICE << 7) | (BMREQUEST_VENDOR << 5) | BMREQUEST_TO_INTERFACE;
  SetupPacket.Request = byWinUSBCommControl;
  SetupPacket.Value = 0;
  SetupPacket.Index = 0; // specify WinUSBComm interface - it should be at index 0
  SetupPacket.Length = wNumBytesCount;

  if ( bResult )
  {
    bResult = CWinUSBWrapper::ControlTransfer(m_hWinUSBHandle, SetupPacket, pbyData, wNumBytesCount, &cbSent, 0);
    PTRACE("Data sent: %d \nActual data transferred: %d.\n", wNumBytesCount, cbSent);
  }


  return bResult;
}

BOOL CWinUSBCommDevice::controlRead(BYTE byWinUSBCommControl, BYTE *pbyData, WORD wNumBytesCount)
{
  if ( 0 == wNumBytesCount )
  {
    return TRUE;
  }

  if ( NULL == pbyData )
  {
    return FALSE;
  }

  BOOL bResult = IsConnected();
  WINUSB_SETUP_PACKET SetupPacket;
  ZeroMemory(&SetupPacket, sizeof(WINUSB_SETUP_PACKET));
  ULONG cbSent = 0;

  //Create the setup packet
  SetupPacket.RequestType = (BMREQUEST_DEVICE_TO_HOST << 7) | (BMREQUEST_VENDOR << 5) | BMREQUEST_TO_INTERFACE;
  SetupPacket.Request = byWinUSBCommControl;
  SetupPacket.Value = 0;
  SetupPacket.Index = 0; // specify WinUSBComm interface
  SetupPacket.Length = wNumBytesCount;

  if ( bResult )
  {
    bResult = CWinUSBWrapper::ControlTransfer(m_hWinUSBHandle, SetupPacket, pbyData, wNumBytesCount, &cbSent, 0);
    PTRACE("Data get : %d \nActual data transferred: %d.\n", wNumBytesCount, cbSent);
  }


  return bResult;
}

BOOL CWinUSBCommDevice::bulkWrite(BYTE *pbyData, DWORD dwNumBytesCount)
{
  if ( 0 == dwNumBytesCount )
  {
    return TRUE;
  }

  if ( NULL == pbyData )
  {
    return FALSE;
  }

  BOOL bResult = IsConnected();
  ULONG cbSent = 0;

  if ( bResult )
  {
    bResult = CWinUSBWrapper::WritePipe(m_hWinUSBHandle, m_byPipeOutId, pbyData, dwNumBytesCount, &cbSent, 0);
    PTRACE("Wrote to pipe %d: %d bytes\nActual data transferred: %d.\n", m_byPipeOutId, dwNumBytesCount, cbSent);
  }

  if ( bResult )
  {
     bResult = cbSent == dwNumBytesCount;
  }

  return bResult;
}

BOOL CWinUSBCommDevice::bulkRead(BYTE *pbyData, DWORD dwNumBytesCount)
{
  if ( 0 == dwNumBytesCount )
  {
    return TRUE;
  }

  if ( NULL == pbyData )
  {
    return FALSE;
  }

  BOOL bResult = IsConnected();
  ULONG cbRead = 0;

  if ( bResult )
  {
    bResult = CWinUSBWrapper::ReadPipe(m_hWinUSBHandle, m_byPipeInId, pbyData, dwNumBytesCount, &cbRead, 0);
    PTRACE("Read from pipe %d: %d bytes\nActual data read: %d.\n", m_byPipeInId, dwNumBytesCount, cbRead);
  }

  if ( bResult )
  {
    bResult = cbRead == dwNumBytesCount;
  }

  return bResult;
}

BOOL CWinUSBCommDevice::SendData(BYTE *pbyBuffer, DWORD dwBufferSizeInBytes)
{
  if ( !IsConnected() )
  {
    return FALSE;
  }

  return doSend(pbyBuffer, dwBufferSizeInBytes);
}

BOOL CWinUSBCommDevice::doSend(BYTE *pbyData, DWORD dwNumBytesCount)
{
  BOOL bResult = TRUE;
  switch ( m_byDeviceVersion )
  {
  case winusbcommversion1a:
  case winusbcommversion1b:
  case winusbcommversion1c:
  case winusbcommversion1:
    bResult = controlWrite(m_bycommandReset);
    break;
  case winusbcommversion2:
    bResult = controlWrite(winusbcomm2commandFollowingPacketSize, (BYTE *)&dwNumBytesCount, sizeof(dwNumBytesCount));
    break;
  default:
    bResult = FALSE;
    break;
  }

  if ( bResult )
  {
    bResult = bulkWrite(pbyData, dwNumBytesCount);
  }

  if ( bResult )
  {
    switch ( m_byDeviceVersion )
    {
    case winusbcommversion1a:
    case winusbcommversion1b:
    case winusbcommversion1c:
    case winusbcommversion1:
      bResult = controlWrite(winusbctrlTXDONE);
      break;
    case winusbcommversion2:
      break;
    default:
      bResult = FALSE;
      break;
    }
  }
  return bResult;
}

BOOL CWinUSBCommDevice::CanReceive(DWORD &rdwNumBytes)
{
  rdwNumBytes = 0;
  if ( !IsConnected() )
  {
    return FALSE;
  }

  if ( m_byDeviceVersion <= winusbcommversion1 )
  {
    EWinUSBComm2State eStatus = (EWinUSBComm2State)readStatus();
    if ( winusbcomm2stateReceiving == eStatus )
    {
      return TRUE;
    }
    if ( winusbcomm2stateError == eStatus )
    {
      return FALSE;
    }
  }

  BOOL bResult = getResponseLength(rdwNumBytes);
  if ( !bResult )
  {
    rdwNumBytes = 0;
    return FALSE;
  }
  return TRUE;
}

BOOL CWinUSBCommDevice::DoReceive(unsigned char *pbyData, DWORD dwNumBytes)
{
  if ( !IsConnected() )
  {
    return FALSE;
  }

  BOOL bResult = bulkRead(pbyData, dwNumBytes);
  return bResult;
}

BOOL CWinUSBCommDevice::reset()
{
  if ( !IsConnected() )
  {
    return FALSE;
  }
  BOOL bResult = controlWrite(m_bycommandReset);
  return bResult;
}

BYTE CWinUSBCommDevice::readStatus()
{
  if ( !IsConnected() )
  {
    return winusbcomm2stateError;
  }

  BYTE byCommStatus = winusbcomm2stateError;
  BOOL bResult = controlRead(m_bycommandGetState, &byCommStatus, sizeof(byCommStatus));
  if ( !bResult )
  {
    return winusbcomm2stateError;
  }

  if ( m_byDeviceVersion <= winusbcommversion1 )
  {
    switch ( byCommStatus )
    {
    case winusbcommIDLE:  byCommStatus = winusbcomm2stateIdle; break;
    case winusbcommPROCESSING:  byCommStatus = winusbcomm2stateIdle; break;
    case winusbcommERROR: // fall through
    default:  byCommStatus = winusbcomm2stateError; break;
    }
  }

  return byCommStatus;
}

BOOL CWinUSBCommDevice::getResponseLength(DWORD &rdwResponseByteCount)
{
  BOOL bResult = controlRead(m_bycommandGetReturnSize, (BYTE *)&rdwResponseByteCount, sizeof(rdwResponseByteCount));
  return bResult;
}

void CWinUSBCommDevice::Disconnect()
{
  if ( INVALID_HANDLE_VALUE != m_hWinUSBHandle )
  {
    CWinUSBWrapper::Free(m_hWinUSBHandle);
    m_hWinUSBHandle = INVALID_HANDLE_VALUE;
  }

  if ( INVALID_HANDLE_VALUE != m_hDeviceHandle )
  {
    CloseHandle(m_hDeviceHandle);
    m_hDeviceHandle = INVALID_HANDLE_VALUE;
  }
}

