
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#ifdef _MP  // where is this?!
#undef _MP
#endif

#include <string>

#ifdef UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

std::string W2STR(const WCHAR *pcszString);
std::string T2STR(LPCTSTR pcszString);
std::wstring ASTR2WSTR(LPCSTR pcszString);
std::wstring ASTR2WSTR(std::string strString);
tstring ASTR2TSTR(LPCSTR pcszString);
tstring ASTR2TSTR(std::string strString);

#define W2CSTR(pcszw) W2STR(pcszw).c_str()
#define T2CSTR(pctsz) T2STR(pctsz).c_str()
#define A2CSTRW(pcsz) ASTR2WSTR(pcsz).c_str()
#define A2CTSTR(pcsz) ASTR2TSTR(pcsz).c_str()

#define COMMCOUNT unsigned long
#define COMMINDEX unsigned long




