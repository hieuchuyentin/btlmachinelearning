
// BTL_HM.h : main header file for the BTL_HM application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "BTL_HMDialog.h"

// CBTL_HMApp:
// See BTL_HM.cpp for the implementation of this class
//

class CBTL_HMApp : public CWinAppEx
{
public:
	CBTL_HMApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CBTL_HMApp theApp;
