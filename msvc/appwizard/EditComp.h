#if !defined(AFX_EDITCOMP_H__51F2F25F_395D_11D4_931B_005004331574__INCLUDED_)
#define AFX_EDITCOMP_H__51F2F25F_395D_11D4_931B_005004331574__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditComp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditComp dialog

class CEditComp : public CDialog
{
// Construction
public:
	CEditComp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditComp)
	enum { IDD = IDD_EDIT };
	CEdit	m_SimpleStacksize;
	CEdit	m_SimpleName;
	CEdit	m_SimpleDescription;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditComp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditComp)
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSimpleActivity();
	afx_msg void OnSimpleHandlemessage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITCOMP_H__51F2F25F_395D_11D4_931B_005004331574__INCLUDED_)
