
// test_runner_mfc.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CtestrunnermfcApp:
// See test_runner_mfc.cpp for the implementation of this class
//

class CtestrunnermfcApp : public CWinApp
{
public:
	CtestrunnermfcApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CtestrunnermfcApp theApp;
