// Dirs.cpp : implementation file
//

#include "stdafx.h"
#include "OmnetppWizard.h"
#include "Dirs.h"
#include "OmnetppWizardAw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirs dialog


CDirs::CDirs(CWnd* pParent /*=NULL*/)
	: CAppWizStepDlg(CDirs::IDD)
{
	//{{AFX_DATA_INIT(CDirs)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDirs::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDirs)
	DDX_Control(pDX, IDC_TCLVER, m_TclVer);
	DDX_Control(pDX, IDC_TCLDIR, m_TclDir);
	DDX_Control(pDX, IDC_OPPDIR, m_OppDir);
	//}}AFX_DATA_MAP

	m_OppDir.SetWindowText(OppDir);
	m_TclDir.SetWindowText(TclDir);
	m_TclVer.SetWindowText(TclVer);
	m_OppDir.SetLimitText(MAX_PATH);
	m_TclDir.SetLimitText(MAX_PATH);
	m_TclVer.SetLimitText(MAX_PATH);
}


BEGIN_MESSAGE_MAP(CDirs, CDialog)
	//{{AFX_MSG_MAP(CDirs)
	ON_BN_CLICKED(IDC_OPPDIR_BROWSE, OnOppdirBrowse)
	ON_BN_CLICKED(IDC_TCLDIR_BROWSE, OnTcldirBrowse)
	ON_EN_CHANGE(IDC_OPPDIR, OnChangeOppdir)
	ON_EN_CHANGE(IDC_TCLDIR, OnChangeTcldir)
	ON_EN_CHANGE(IDC_TCLVER, OnChangeTclver)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirs message handlers
BOOL CDirs::OnDismiss()
{
	if (!UpdateData(TRUE))
		return FALSE;

	// TODO: Set template variables based on the dialog's data.
	unsigned int i,j;
	char str[100];

	OmnetppWizardaw.m_Dictionary["OPPDIR"] = OppDir;
	OmnetppWizardaw.m_Dictionary["TCLDIR"] = TclDir;
	j=0;
	for(i = 0; i<=strlen(TclVer); i++)
	{
		if(TclVer[i] != '.')
			str[j++] = TclVer[i];
	}

	OmnetppWizardaw.m_Dictionary["TCLVER"] = str;

	return TRUE;	// return FALSE if the dialog shouldn't be dismissed
}


void CDirs::OnOppdirBrowse()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO lpbi;
	char dirN[MAX_PATH];
	LPITEMIDLIST list;
	char dir[MAX_PATH];


    lpbi.hwndOwner = m_hWnd;
	lpbi.pidlRoot = NULL;
    lpbi.pszDisplayName = dirN;
    lpbi.lpszTitle = "Select OMNeT++ root directory";
    lpbi.ulFlags = BIF_EDITBOX;
    lpbi.lpfn = NULL;
    lpbi.lParam = NULL;
    lpbi.iImage = 0;


	list = SHBrowseForFolder(&lpbi);
	SHGetPathFromIDList(list, dir);
	m_OppDir.SetWindowText(dir);

}

void CDirs::OnTcldirBrowse()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO lpbi;
	char dirN[MAX_PATH];
	LPITEMIDLIST list;
	char dir[MAX_PATH];


    lpbi.hwndOwner = m_hWnd;
	lpbi.pidlRoot = NULL;
    lpbi.pszDisplayName = dirN;
    lpbi.lpszTitle = "Select TCL/TK lib directory";
    lpbi.ulFlags = BIF_EDITBOX;
    lpbi.lpfn = NULL;
    lpbi.lParam = NULL;
    lpbi.iImage = 0;


	list = SHBrowseForFolder(&lpbi);
	SHGetPathFromIDList(list, dir);
	m_TclDir.SetWindowText(dir);

}

void CDirs::OnChangeOppdir()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	m_OppDir.GetWindowText(OppDir, MAX_PATH);

}

void CDirs::OnChangeTcldir()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	m_TclDir.GetWindowText(TclDir, MAX_PATH);

}

void CDirs::OnChangeTclver()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	m_TclVer.GetWindowText(TclVer, MAX_PATH);

}
