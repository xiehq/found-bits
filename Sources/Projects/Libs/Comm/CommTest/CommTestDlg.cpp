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


// CommTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CommTest.h"
#include "CommTestDlg.h"
#include "afxdialogex.h"

#include "CommWinUSB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCommTestDlg dialog




CCommTestDlg::CCommTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCommTestDlg::IDD, pParent),
  m_bRunning(FALSE),
  m_pbyTestBuffer(NULL),
  m_pbyReponseData(NULL),
  m_hComm(NULL),
  m_dwAvailablePacketSize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CCommTestDlg::~CCommTestDlg()
{
  if ( m_pbyTestBuffer )
  {
    delete[] m_pbyTestBuffer;
    m_pbyTestBuffer = NULL;
  }
}

void CCommTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO_NAME, m_wndDeviceName);
  DDX_CBString(pDX, IDC_COMBO_NAME, m_strDeviceName);
  DDX_Check(pDX, IDC_CHECK1, m_sTestConfiguration.m_bRunContiniously);
  DDX_Radio(pDX, IDC_RADIO1, m_sTestConfiguration.m_ePacketSize);

  DDX_Text(pDX, IDC_EDIT4, m_sTestConfiguration.m_dwConstatPacketSize);
  DDX_Text(pDX, IDC_EDIT2, m_sTestConfiguration.m_dwMinPacketSize);
  DDX_Text(pDX, IDC_EDIT3, m_sTestConfiguration.m_dwMaxPacketSize);

  if ( !pDX->m_bSaveAndValidate )
  {
    DDX_Text(pDX, IDC_EDIT5, m_sTestResult.m_dwPacketSize);
    DDX_Text(pDX, IDC_EDIT6, m_sTestResult.m_dwSpeedInBps);
  }
}

void CCommTestDlg::UpdateControls()
{
  UpdateData(TRUE);
  UpdateData(FALSE);

  CWnd * pCtrl = NULL;
  pCtrl = GetDlgItem(IDC_COMBO_NAME); pCtrl->EnableWindow(!m_bRunning);
  pCtrl = GetDlgItem(IDC_RADIO1); pCtrl->EnableWindow(!m_bRunning);
  pCtrl = GetDlgItem(IDC_RADIO2); pCtrl->EnableWindow(!m_bRunning);
  pCtrl = GetDlgItem(IDC_EDIT4); pCtrl->EnableWindow((!m_bRunning) && (packetSizeConstant == m_sTestConfiguration.m_ePacketSize));
  pCtrl = GetDlgItem(IDC_EDIT2); pCtrl->EnableWindow((!m_bRunning) && (packetSizeDynamic == m_sTestConfiguration.m_ePacketSize));
  pCtrl = GetDlgItem(IDC_EDIT3); pCtrl->EnableWindow((!m_bRunning) && (packetSizeDynamic == m_sTestConfiguration.m_ePacketSize));
  pCtrl = GetDlgItem(IDC_BUTTON1); pCtrl->EnableWindow(!m_bRunning);
  pCtrl = GetDlgItem(IDC_BUTTON2); pCtrl->EnableWindow(m_bRunning);
  pCtrl = GetDlgItem(IDC_CHECK1); pCtrl->EnableWindow(!m_bRunning);
}


BEGIN_MESSAGE_MAP(CCommTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
  ON_BN_CLICKED(IDCANCEL, &CCommTestDlg::OnBnClickedCancel)
  ON_BN_CLICKED(IDC_BUTTON1, &CCommTestDlg::OnBnClickedDoTest)
  ON_BN_CLICKED(IDC_BUTTON2, &CCommTestDlg::OnBnClickedStopTest)
  ON_CBN_DROPDOWN(IDC_COMBO_NAME, &CCommTestDlg::OnCbnDropdownComboName)
  ON_BN_CLICKED(IDC_RADIO1, &CCommTestDlg::OnBnClickedRadio1)
  ON_BN_CLICKED(IDC_RADIO2, &CCommTestDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()


// CCommTestDlg message handlers

BOOL CCommTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
  
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
  enumerateDevices();
  UpdateData(FALSE);
  UpdateControls();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCommTestDlg::enumerateDevices()
{
  CWinUSBCommDevice::TStringList devs;
  CWinUSBCommDevice::ListDevices(devs);
  m_wndDeviceName.ResetContent();
  for ( DWORD I = 0; I < devs.size(); I++ )
  {
    m_wndDeviceName.AddString(A2CTSTR(devs[I]));
  }

  if ( ( devs.size() > 0 ) && m_strDeviceName.IsEmpty() )
  {
    m_strDeviceName = devs[0].c_str();
  }
}

DWORD CCommTestDlg::calculateTestSize(DWORD dwPrevoiusSize) const
{
  if ( packetSizeConstant == m_sTestConfiguration.m_ePacketSize )
  {
    return m_sTestConfiguration.m_dwConstatPacketSize;
  }
  
  DWORD dwPacketSize = dwPrevoiusSize;

  if ( dwPrevoiusSize >= m_dwAvailablePacketSize )
  {
    dwPacketSize = m_sTestConfiguration.m_dwMinPacketSize;
  }
  else if ( dwPrevoiusSize >= m_sTestConfiguration.m_dwMaxPacketSize )
  {
    dwPacketSize = m_sTestConfiguration.m_dwMinPacketSize;
  }
  else
  {
    dwPacketSize = dwPrevoiusSize + 0x100;
  }

  if ( dwPacketSize > m_sTestConfiguration.m_dwMaxPacketSize )
  {
    dwPacketSize = m_sTestConfiguration.m_dwMaxPacketSize;
  }
  if ( dwPacketSize > m_dwAvailablePacketSize )
  {
    dwPacketSize = m_dwAvailablePacketSize;
  }
  return dwPacketSize;
}

void CCommTestDlg::doOneCommTest()
{
  m_sTestResult.m_dwSpeedInBps = 0;
  if ( !m_hComm )
  {
    return;
  }

  DWORD dwTestSize = calculateTestSize(m_sTestResult.m_dwPacketSize);
  m_sTestResult.m_dwPacketSize = dwTestSize;

  BYTE bySeed = (BYTE)rand();

  for ( DWORD I = 0; I < dwTestSize; I++ )
  {
    m_pbyTestBuffer[I] = bySeed++;
  }

  LARGE_INTEGER countStart;
  QueryPerformanceCounter(&countStart);

  CommStack_PacketStart(m_hComm);
  CommStack_Send(m_hComm, m_pbyTestBuffer, (COMMCOUNT)dwTestSize);
  CommStack_PacketEnd(m_hComm);
  
  ECommStatus eStatus = commstatusIdle;
  
  do 
  {
    eStatus = CommStack_TransmitProcess(m_hComm);
      // TODO: timeout
  } while ( commstatusActive == eStatus );

  COMMCOUNT cntResponseSize = 0;

  BOOL bReceiving = TRUE;
  do 
  {
    eStatus = CommStack_ReceiveProcess(m_hComm, &cntResponseSize, &m_pbyReponseData);
    switch ( eStatus )
    {
    case commstatusIdle: // fall through
    case commstatusActive: break;
    case commstatusNewPacket: // fall through
    default: bReceiving = FALSE; break;
    }
  } while ( bReceiving );

  LARGE_INTEGER countEnd;
  QueryPerformanceCounter(&countEnd);

  BOOL bResult = commstatusNewPacket == eStatus;
  
  if ( bResult )
  {
    bResult = ( cntResponseSize > 0 ) && ( cntResponseSize == dwTestSize );
  }

  if ( bResult )
  {
    for ( DWORD I = 0; I < dwTestSize; I++ )
    {
      if ( m_pbyReponseData[I] != m_pbyTestBuffer[I] )
      {
        bResult = FALSE;
        break;
      }
    }
  }

  if ( !bResult )
  {
    OnBnClickedStopTest();
    return;
  }

  LARGE_INTEGER f;
  QueryPerformanceFrequency( &f );

  LARGE_INTEGER elapsedCount;
  elapsedCount.QuadPart = countEnd.QuadPart - countStart.QuadPart;

  DOUBLE elapsed = (DOUBLE)elapsedCount.QuadPart / (DOUBLE)f.QuadPart;
  m_sTestResult.m_dwSpeedInBps = (DWORD)((dwTestSize << 1) / elapsed);
}

void CCommTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCommTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCommTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CCommTestDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
  if ( !m_bRunning )
  {
    return TRUE;
  }
  doOneCommTest();
  UpdateData(FALSE);
  if ( !m_sTestConfiguration.m_bRunContiniously )
  {
    m_bRunning = FALSE;
    disconnect();
    UpdateControls();
  }
  return TRUE;
}

void CCommTestDlg::OnBnClickedCancel()
{
  // TODO: Add your control notification handler code here
  CDialogEx::OnCancel();
}


void CCommTestDlg::OnBnClickedDoTest()
{
  UpdateData(TRUE);
  m_hComm = connect();
  BOOL bResult = TRUE;
  if ( bResult )
  {
    bResult = NULL != m_hComm;
  }
  
  DWORD dwAvailablePacketSize = 0;
  if ( bResult )
  {
    dwAvailablePacketSize = CommStack_GetBufferSize(m_hComm);
    bResult = 0 != dwAvailablePacketSize;
  }
  
  if ( bResult )
  {
    if ( packetSizeDynamic == m_sTestConfiguration.m_ePacketSize )
    {
      if ( !m_sTestConfiguration.m_dwMinPacketSize )
      {
        bResult = FALSE;
      }
      if ( m_sTestConfiguration.m_dwMinPacketSize >= m_sTestConfiguration.m_dwMaxPacketSize )
      {
        bResult = FALSE;
      }
      if ( dwAvailablePacketSize < m_sTestConfiguration.m_dwMaxPacketSize )
      {
        bResult = FALSE;
      }
    }
    else
    {
      if ( dwAvailablePacketSize < m_sTestConfiguration.m_dwConstatPacketSize )
      {
        m_sTestConfiguration.m_dwConstatPacketSize = dwAvailablePacketSize;
      }
    }
  }   

  if ( bResult )
  {
    if ( dwAvailablePacketSize > m_dwAvailablePacketSize )
    {
      if ( m_pbyTestBuffer )
      {
        delete[] m_pbyTestBuffer;
        m_pbyTestBuffer = NULL;
        m_dwAvailablePacketSize = 0;
      }
      m_pbyTestBuffer = new BYTE[dwAvailablePacketSize];
      bResult = NULL != m_pbyTestBuffer;
      m_dwAvailablePacketSize = dwAvailablePacketSize;
    }
  }

  m_bRunning = bResult;
  UpdateData(FALSE);
  UpdateControls();
}

BYTE g_abyBuffer[32 * 1024];

SCommStack g_csWinUSB;
SCommLayer g_aclWinUSB[1];
CCommWinUSB g_cWinUSB(&g_aclWinUSB[0]);

/// Setup comm stack as configured.
/// Can have multiple comm stacks - must add configuration (also to GUI).
HCOMMSTACK CCommTestDlg::connect()
{
  if ( m_hComm )
  {
    disconnect();
  }
  g_cWinUSB.Configure(T2CSTR(m_strDeviceName), g_abyBuffer, sizeof(g_abyBuffer));
  HCOMMSTACK hComm = CommStack_Init(0, &g_csWinUSB, g_aclWinUSB, _countof(g_aclWinUSB));
  return hComm;
}

void CCommTestDlg::disconnect()
{
  if ( !m_hComm )
  {
    return;
  }
  CommStack_Disconnect(m_hComm);
  m_hComm = NULL;
}


void CCommTestDlg::OnBnClickedStopTest()
{
  m_bRunning = FALSE;
  disconnect();
  UpdateData(FALSE);
  UpdateControls();
}


void CCommTestDlg::OnCbnDropdownComboName()
{
  UpdateData(TRUE);
  enumerateDevices();
  UpdateData(FALSE);
}


void CCommTestDlg::OnBnClickedRadio1()
{
  UpdateData(TRUE);
  UpdateControls();
}


void CCommTestDlg::OnBnClickedRadio2()
{
  UpdateData(TRUE);
  UpdateControls();
}
