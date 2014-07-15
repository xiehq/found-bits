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


// CommTestDlg.h : header file
//

#pragma once

#include "WinUSBCommDevice.h"
#include "CommCore.h"

// CCommTestDlg dialog
class CCommTestDlg : public CDialogEx
{
  typedef CDialogEx base_class;
// Construction
public:
	CCommTestDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CCommTestDlg();

  enum EPacketSize
  {
    packetSizeConstant,
    packetSizeDynamic
  };

  struct STestConfiguration
  {
    STestConfiguration() : 
      m_ePacketSize(packetSizeConstant),
      m_dwConstatPacketSize(16 * 1024),
      m_dwMinPacketSize(1 * 1024),
      m_dwMaxPacketSize(16 * 1024),
      m_bRunContiniously(FALSE)
      {     
      }
    int m_ePacketSize;
    DWORD m_dwConstatPacketSize;
    DWORD m_dwMinPacketSize;
    DWORD m_dwMaxPacketSize;
    BOOL m_bRunContiniously;
  };
  STestConfiguration m_sTestConfiguration;
  struct STestResult
  {
    STestResult() :
      m_dwSpeedInBps(0),
      m_dwPacketSize(0)
      {
      }
    DWORD m_dwPacketSize;
    DWORD m_dwSpeedInBps;
  };
  STestResult m_sTestResult;
// Dialog Data
	enum { IDD = IDD_COMMTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  void UpdateControls();


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedCancel();
  afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);

private:
  void enumerateDevices();
  DWORD calculateTestSize(DWORD dwPrevoiusSize) const;
  void doOneCommTest();

  HCOMMSTACK connect();
  void disconnect();
private:
  BOOL m_bRunning;
  CString m_strDeviceName;
  CComboBox	m_wndDeviceName;
  
  DWORD m_dwAvailablePacketSize;
  BYTE *m_pbyTestBuffer;
  BYTE *m_pbyReponseData;

  HCOMMSTACK m_hComm;
public:
  afx_msg void OnBnClickedDoTest();
  afx_msg void OnBnClickedStopTest();
  afx_msg void OnCbnDropdownComboName();
  afx_msg void OnBnClickedRadio1();
  afx_msg void OnBnClickedRadio2();
};
