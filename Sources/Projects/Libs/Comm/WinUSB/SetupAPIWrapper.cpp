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
#include "SetupAPIWrapper.h"

CSetupAPIWrapper CSetupAPIWrapper::sm_SetupAPIWrapper;
HMODULE CSetupAPIWrapper::sm_hSetupAPI = NULL;

CSetupAPIWrapper::pfnSetupDiGetClassDevs CSetupAPIWrapper::sm_pfnSetupDiGetClassDevs = NULL;
CSetupAPIWrapper::pfnSetupDiEnumDeviceInfo CSetupAPIWrapper::sm_pfnSetupDiEnumDeviceInfo = NULL;
CSetupAPIWrapper::pfnSetupDiEnumDeviceInterfaces CSetupAPIWrapper::sm_pfnSetupDiEnumDeviceInterfaces = NULL;
CSetupAPIWrapper::pfnSetupDiGetDeviceInterfaceDetail CSetupAPIWrapper::sm_pfnSetupDiGetDeviceInterfaceDetail = NULL;
CSetupAPIWrapper::pfnSetupDiDestroyDeviceInfoList CSetupAPIWrapper::sm_pfnSetupDiDestroyDeviceInfoList = NULL;
CSetupAPIWrapper::pfnSetupDiGetDeviceRegistryProperty CSetupAPIWrapper::sm_pfnSetupDiGetDeviceRegistryProperty = NULL;

CSetupAPIWrapper::CSetupAPIWrapper(void)
{
  sm_hSetupAPI = ::LoadLibrary(_T("SetupAPI.dll"));
  if ( !sm_hSetupAPI )
  {
    return;
  }
  
// #ifdef UNICODE
  sm_pfnSetupDiGetClassDevs = (pfnSetupDiGetClassDevs)::GetProcAddress(sm_hSetupAPI, "SetupDiGetClassDevsW");
// #else
//   sm_pfnSetupDiGetClassDevs = (pfnSetupDiGetClassDevs)::GetProcAddress(sm_hSetupAPI, "SetupDiGetClassDevsA");
// #endif
  sm_pfnSetupDiEnumDeviceInfo = (pfnSetupDiEnumDeviceInfo)::GetProcAddress(sm_hSetupAPI, "SetupDiEnumDeviceInfo");
  sm_pfnSetupDiEnumDeviceInterfaces = (pfnSetupDiEnumDeviceInterfaces)::GetProcAddress(sm_hSetupAPI, "SetupDiEnumDeviceInterfaces");
// #ifdef UNICODE
  sm_pfnSetupDiGetDeviceInterfaceDetail = (pfnSetupDiGetDeviceInterfaceDetail)::GetProcAddress(sm_hSetupAPI, "SetupDiGetDeviceInterfaceDetailW");
// #else
//   sm_pfnSetupDiGetDeviceInterfaceDetail = (pfnSetupDiGetDeviceInterfaceDetail)::GetProcAddress(sm_hSetupAPI, "SetupDiGetDeviceInterfaceDetailA");
// #endif
  sm_pfnSetupDiDestroyDeviceInfoList = (pfnSetupDiDestroyDeviceInfoList)::GetProcAddress(sm_hSetupAPI, "SetupDiDestroyDeviceInfoList");
// #ifdef UNICODE
  sm_pfnSetupDiGetDeviceRegistryProperty = (pfnSetupDiGetDeviceRegistryProperty)::GetProcAddress(sm_hSetupAPI, "SetupDiGetDeviceRegistryPropertyW");
// #else
//   sm_pfnSetupDiGetDeviceRegistryProperty = (pfnSetupDiGetDeviceRegistryProperty)::GetProcAddress(sm_hSetupAPI, "SetupDiGetDeviceRegistryPropertyA");
// #endif

  BOOL bResult = TRUE;
  bResult = bResult && sm_pfnSetupDiGetClassDevs;
  bResult = bResult && sm_pfnSetupDiEnumDeviceInfo;
  bResult = bResult && sm_pfnSetupDiEnumDeviceInterfaces;
  bResult = bResult && sm_pfnSetupDiGetDeviceInterfaceDetail;
  bResult = bResult && sm_pfnSetupDiDestroyDeviceInfoList;
  bResult = bResult && sm_pfnSetupDiGetDeviceRegistryProperty;

  if ( bResult )
  {
    return;
  }

  ::FreeLibrary(sm_hSetupAPI);
  sm_hSetupAPI = NULL;
}


CSetupAPIWrapper::~CSetupAPIWrapper(void)
{
  if ( sm_hSetupAPI )
  {
    ::FreeLibrary(sm_hSetupAPI);
    sm_hSetupAPI = NULL;
  }
}

// #ifdef UNICODE
HDEVINFO CSetupAPIWrapper::SetupDiGetClassDevsW(
    __in_opt CONST GUID *ClassGuid,
    __in_opt PCWSTR Enumerator,
    __in_opt HWND hwndParent,
    __in DWORD Flags
    )
{
  if ( !sm_hSetupAPI )
  {
    return NULL;
  }
  return sm_pfnSetupDiGetClassDevs(ClassGuid, Enumerator, hwndParent, Flags); 
}
// #else
// HDEVINFO CSetupAPIWrapper::SetupDiGetClassDevsA(
//     __in_opt CONST GUID *ClassGuid,
//     __in_opt PCSTR Enumerator,
//     __in_opt HWND hwndParent,
//     __in DWORD Flags
//     )
// {
//   if ( !sm_hSetupAPI )
//   {
//     return NULL;
//   }
//   return sm_pfnSetupDiGetClassDevs(ClassGuid, Enumerator, hwndParent, Flags); 
// }
// #endif

BOOL CSetupAPIWrapper::SetupDiEnumDeviceInfo(
    __in HDEVINFO DeviceInfoSet,
    __in DWORD MemberIndex,
    __out PSP_DEVINFO_DATA DeviceInfoData
    )
{
  if ( !sm_hSetupAPI )
  {
    return FALSE;
  }
  return sm_pfnSetupDiEnumDeviceInfo(DeviceInfoSet, MemberIndex, DeviceInfoData); 
}

BOOL CSetupAPIWrapper::SetupDiEnumDeviceInterfaces(
    __in HDEVINFO DeviceInfoSet,
    __in_opt PSP_DEVINFO_DATA DeviceInfoData,
    __in CONST GUID *InterfaceClassGuid,
    __in DWORD MemberIndex,
    __out PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData
    )
{
  if ( !sm_hSetupAPI )
  {
    return FALSE;
  }
  return sm_pfnSetupDiEnumDeviceInterfaces(DeviceInfoSet, DeviceInfoData, InterfaceClassGuid, MemberIndex, DeviceInterfaceData); 
}

// #ifdef UNICODE
BOOL CSetupAPIWrapper::SetupDiGetDeviceInterfaceDetailW(
    __in HDEVINFO DeviceInfoSet,
    __in PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
    __out_bcount_opt(DeviceInterfaceDetailDataSize) PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData,
    __in DWORD DeviceInterfaceDetailDataSize,
    __out_opt PDWORD RequiredSize, 
    __out_opt PSP_DEVINFO_DATA DeviceInfoData
    )
{
  if ( !sm_hSetupAPI )
  {
    return FALSE;
  }
  return sm_pfnSetupDiGetDeviceInterfaceDetail(DeviceInfoSet, DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize, DeviceInfoData); 
}
// #else
// BOOL CSetupAPIWrapper::SetupDiGetDeviceInterfaceDetailA(
//     __in HDEVINFO DeviceInfoSet,
//     __in PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
//     __inout_bcount_opt(DeviceInterfaceDetailDataSize) PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData, 
//     __in DWORD DeviceInterfaceDetailDataSize,
//     __out_opt PDWORD RequiredSize,
//     __out_opt PSP_DEVINFO_DATA DeviceInfoData
//     )
// {
//   if ( !sm_hSetupAPI )
//   {
//     return FALSE;
//   }
//   return sm_pfnSetupDiGetDeviceInterfaceDetail(DeviceInfoSet, DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize, DeviceInfoData); 
// }
// #endif

BOOL CSetupAPIWrapper::SetupDiDestroyDeviceInfoList(
    __in HDEVINFO DeviceInfoSet
    )
{
  if ( !sm_hSetupAPI )
  {
    return FALSE;
  }
  return sm_pfnSetupDiDestroyDeviceInfoList(DeviceInfoSet); 
}

// #ifdef UNICODE
BOOL CSetupAPIWrapper::SetupDiGetDeviceRegistryPropertyW(
  __in HDEVINFO DeviceInfoSet,
  __in PSP_DEVINFO_DATA DeviceInfoData,
  __in DWORD Property,
  __out_opt PDWORD PropertyRegDataType,
  __out_bcount_opt(PropertyBufferSize) PBYTE PropertyBuffer,
  __in DWORD PropertyBufferSize,
  __out_opt PDWORD RequiredSize
  )
{
  if ( !sm_hSetupAPI )
  {
    return FALSE;
  }
  return sm_pfnSetupDiDestroyDeviceInfoList(DeviceInfoSet); 
}
// #else
// BOOL CSetupAPIWrapper::SetupDiGetDeviceRegistryPropertyA(
//   __in HDEVINFO DeviceInfoSet,
//   __in PSP_DEVINFO_DATA DeviceInfoData,
//   __in DWORD Property,
//   __out_opt PDWORD PropertyRegDataType, 
//   __out_bcount_opt(PropertyBufferSize) PBYTE PropertyBuffer,
//   __in DWORD PropertyBufferSize,
//   __out_opt PDWORD RequiredSize 
//   )
// {
//   if ( !sm_hSetupAPI )
//   {
//     return FALSE;
//   }
//   return sm_pfnSetupDiGetDeviceRegistryProperty(DeviceInfoSet, DeviceInfoData, Property, PropertyRegDataType, PropertyBuffer, PropertyBufferSize, RequiredSize); 
// }
// #endif


