
// test_runner_mfcDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "test_runner_mfc.h"
#include "test_runner_mfcDlg.h"
#include "afxdialogex.h"

#include <ctest_item.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestrunnermfcDlg dialog



CtestrunnermfcDlg::CtestrunnermfcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEST_RUNNER_MFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestrunnermfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INFO, m_list_info);
}

BEGIN_MESSAGE_MAP(CtestrunnermfcDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CtestrunnermfcDlg::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_CREATE_NAMED_OBJECT, &CtestrunnermfcDlg::OnBnClickedButtonCreateNamedObject)
	ON_BN_CLICKED(IDC_BUTTONCHECK_NAMED_OBJECT, &CtestrunnermfcDlg::OnBnClickedButtoncheckNamedObject)
END_MESSAGE_MAP()


// CtestrunnermfcDlg message handlers

BOOL CtestrunnermfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_MINIMIZE);

	m_list_info.AddString(L" : OK : OnInitDialog().");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CtestrunnermfcDlg::OnPaint()
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
HCURSOR CtestrunnermfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestrunnermfcDlg::OnBnClickedButtonTest()
{
	_test();
}

void CtestrunnermfcDlg::_test()
{
	std::wstring s_temp, s_mutex, s_event;

	s_temp = ctest_item::GetNameShareQ();
	s_mutex = s_temp + L"_MUTEXT_";
	s_event = s_temp + L"_EVENT_";

	m_list_info.AddString(s_mutex.c_str());
	if (_ns_tools::ct_system::is_exist_named_mutex(s_mutex)) {
		m_list_info.AddString(L" : OK : GetNameShareQ()-mutex.");//user,system
	}
	else {
		m_list_info.AddString(L" : NO : GetNameShareQ()-mutex.");
	}

	m_list_info.AddString(s_event.c_str());
	if (_ns_tools::ct_system::is_exist_named_event(s_event)) {
		m_list_info.AddString(L" : OK : GetNameShareQ()-event.");//user,system
	}
	else {
		m_list_info.AddString(L" : NO : GetNameShareQ()-event.");
	}


	//
	m_list_info.AddString(ctest_item::GetNameNDMInstatnce().c_str());
	if (_ns_tools::ct_system::is_exist_named_mutex(ctest_item::GetNameNDMInstatnce())) {
		m_list_info.AddString(L" : OK : GetNameNDMInstatnce().");//user,system
	}
	else {
		m_list_info.AddString(L" : NO : GetNameNDMInstatnce().");
	}
	//
	m_list_info.AddString(ctest_item::GetNameNDMSetupOk().c_str());
	if (_ns_tools::ct_system::is_exist_named_mutex(ctest_item::GetNameNDMSetupOk())) {
		m_list_info.AddString(L" : OK : GetNameNDMSetupOk().");//user,system
	}
	else {
		m_list_info.AddString(L" : NO : GetNameNDMSetupOk().");
	}
	//
	m_list_info.AddString(ctest_item::GetNameNDMKill().c_str());
	if (_ns_tools::ct_system::is_exist_named_event(ctest_item::GetNameNDMKill())) {
		m_list_info.AddString(L" : OK : GetNameNDMKill().");//user,system
	}
	else {
		m_list_info.AddString(L" : NO : GetNameNDMKill().");
	}
	//
	m_list_info.AddString(ctest_item::GetNameSSS_GEvent().c_str());
	if (_ns_tools::ct_system::is_exist_named_event(ctest_item::GetNameSSS_GEvent())) {
		m_list_info.AddString(L" : OK : GetNameSSS_GEvent().");//system
	}
	else {
		m_list_info.AddString(L" : NO : GetNameSSS_GEvent().");//user
	}


}


void CtestrunnermfcDlg::OnBnClickedButtonCreateNamedObject()
{
	std::wstring s_name;
	int n_index(0);

	s_name = ctest_item::get_test_ns_local_named();
	m_v_warp_ptr_event.emplace_back(
		_ns_tools::ct_warp::cevent::type_ptr_cevent(
			new _ns_tools::ct_warp::cevent(
				_ns_tools::ct_system::create_event_for_ipc(true, false, s_name)
			)
		)
	);

	if (m_v_warp_ptr_event[n_index]) {
		if (m_v_warp_ptr_event[n_index]->get_handle()) {
			m_list_info.AddString(L" : OK : create event get_test_ns_local_named() .");
		}
		else {
			m_list_info.AddString(L" : ER : create event get_test_ns_local_named() #1.");
		}

	}
	else {
		m_list_info.AddString(L" : ER : create event get_test_ns_local_named() #2.");
	}
	///////
	++n_index;
	s_name = ctest_item::get_test_ns_global_named();
	m_v_warp_ptr_event.emplace_back(
		_ns_tools::ct_warp::cevent::type_ptr_cevent(
			new _ns_tools::ct_warp::cevent(
				_ns_tools::ct_system::create_event_for_ipc(true, false, s_name)
			)
		)
	);

	if (m_v_warp_ptr_event[n_index]) {
		if (m_v_warp_ptr_event[n_index]->get_handle()) {
			m_list_info.AddString(L" : OK : create event get_test_ns_global_named() .");
		}
		else{
			m_list_info.AddString(L" : ER : create event get_test_ns_global_named() #1.");
		}
	
	}
	else {
		m_list_info.AddString(L" : ER : create event get_test_ns_global_named() #2.");
	}
	///////
	++n_index;
	s_name = ctest_item::get_test_ns_none_global_named();
	m_v_warp_ptr_event.emplace_back(
		_ns_tools::ct_warp::cevent::type_ptr_cevent(
			new _ns_tools::ct_warp::cevent(
				_ns_tools::ct_system::create_event_for_ipc(true, false, s_name)
			)
		)
	);
	if (m_v_warp_ptr_event[n_index]) {
		if (m_v_warp_ptr_event[n_index]->get_handle()) {
			m_list_info.AddString(L" : OK : create event get_test_ns_none_global_named() .");
		}
		else {
			m_list_info.AddString(L" : ER : create event get_test_ns_none_global_named() #1.");
		}

	}
	else {
		m_list_info.AddString(L" : ER : create event get_test_ns_none_global_named() #2.");
	}
	///////

}


void CtestrunnermfcDlg::OnBnClickedButtoncheckNamedObject()
{
	m_list_info.AddString(ctest_item::get_test_ns_local_named().c_str());
	if (_ns_tools::ct_system::is_exist_named_event(ctest_item::get_test_ns_local_named())) {
		m_list_info.AddString(L" : OK : get_test_ns_local_named().");
	}
	else {
		m_list_info.AddString(L" : NO : get_test_ns_local_named().");
	}
	//
	m_list_info.AddString(ctest_item::get_test_ns_global_named().c_str());
	if (_ns_tools::ct_system::is_exist_named_event(ctest_item::get_test_ns_global_named())) {
		m_list_info.AddString(L" : OK : get_test_ns_global_named().");
	}
	else {
		m_list_info.AddString(L" : NO : get_test_ns_global_named().");
	}
	//
	m_list_info.AddString(ctest_item::get_test_ns_none_global_named().c_str());
	if (_ns_tools::ct_system::is_exist_named_event(ctest_item::get_test_ns_none_global_named())) {
		m_list_info.AddString(L" : OK : get_test_ns_none_global_named().");
	}
	else {
		m_list_info.AddString(L" : NO : get_test_ns_none_global_named().");
	}
}
