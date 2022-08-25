
// test_runner_mfcDlg.h : header file
//

#pragma once

#include <vector>
#include <ct_warp.h>

#include <ctest_item.h>

// CtestrunnermfcDlg dialog
class CtestrunnermfcDlg : public CDialogEx
{
private:
	ctest_item::type_v_warp_ptr_event m_v_warp_ptr_event;

// Construction
public:
	CtestrunnermfcDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_RUNNER_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	void _test();
public:
	afx_msg void OnBnClickedButtonTest();
private:
	CListBox m_list_info;
public:
	afx_msg void OnBnClickedButtonCreateNamedObject();
	afx_msg void OnBnClickedButtoncheckNamedObject();
};
