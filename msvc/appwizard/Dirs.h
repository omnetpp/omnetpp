#if !defined(AFX_DIRS_H__51F2F264_395D_11D4_931B_005004331574__INCLUDED_)
#define AFX_DIRS_H__51F2F264_395D_11D4_931B_005004331574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dirs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDirs dialog

class CDirs : public CAppWizStepDlg
{
// Construction
public:
	CDirs(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDirs)
	enum { IDD = IDD_DIRS };
	CEdit	m_TclVer;
	CEdit	m_TclDir;
	CEdit	m_OppDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirs)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDirs)
	afx_msg void OnOppdirBrowse();
	afx_msg void OnTcldirBrowse();
	afx_msg void OnChangeOppdir();
	afx_msg void OnChangeTcldir();
	afx_msg void OnChangeTclver();
	//}}AFX_MSG
	BOOL OnDismiss();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRS_H__51F2F264_395D_11D4_931B_005004331574__INCLUDED_)
