// EditComp.cpp : implementation file
//

#include "stdafx.h"
#include "OmnetppWizard.h"
#include "EditComp.h"
#include "Cstm1Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditComp dialog

static bool isActivity;


CEditComp::CEditComp(CWnd* pParent /*=NULL*/)
	: CDialog(CEditComp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditComp)
	//}}AFX_DATA_INIT

}


void CEditComp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditComp)
	DDX_Control(pDX, IDC_SIMPLE_STACKSIZE, m_SimpleStacksize);
	DDX_Control(pDX, IDC_SIMPLE_NAME, m_SimpleName);
	DDX_Control(pDX, IDC_SIMPLE_DESCRIPTION, m_SimpleDescription);
	//}}AFX_DATA_MAP

	if(compSel == -1)
	{
		m_SimpleName.SetWindowText("New");
		m_SimpleStacksize.SetWindowText("16384");
	}
	else
	{
		m_SimpleName.SetWindowText(comp[compSel].name);
		m_SimpleDescription.SetWindowText(comp[compSel].description);
		m_SimpleStacksize.SetWindowText(comp[compSel].stacksize);
	}

	m_SimpleName.SetSel(0, -1, FALSE);
	m_SimpleName.SetLimitText(COMP_NAME_L);
	m_SimpleDescription.SetLimitText(COMP_DESCRIPTION_L);
	m_SimpleStacksize.SetLimitText(COMP_STACKSIZE_L);

//	SendDlgItemMessage(hWnd,
	CheckRadioButton(IDC_SIMPLE_ACTIVITY, IDC_SIMPLE_ACTIVITY, IDC_SIMPLE_ACTIVITY);

	isActivity = true;

}


BEGIN_MESSAGE_MAP(CEditComp, CDialog)
	//{{AFX_MSG_MAP(CEditComp)
	ON_BN_CLICKED(IDC_SIMPLE_ACTIVITY, OnSimpleActivity)
	ON_BN_CLICKED(IDC_SIMPLE_HANDLEMESSAGE, OnSimpleHandlemessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditComp message handlers

void CEditComp::OnOK() 
{
	// TODO: Add extra validation here
	if(compSel == -1)
	{
		m_SimpleName.GetWindowText(comp[compN].name, COMP_NAME_L);
		m_SimpleDescription.GetWindowText(comp[compN].description, COMP_DESCRIPTION_L);
		comp[compN].isActivity = isActivity;
		m_SimpleStacksize.GetWindowText(comp[compN].stacksize, COMP_STACKSIZE_L);
		compN+=1;
	}
	else
	{
		m_SimpleName.GetWindowText(comp[compSel].name, COMP_NAME_L);
		m_SimpleDescription.GetWindowText(comp[compSel].description, COMP_DESCRIPTION_L);
		comp[compSel].isActivity = isActivity;
		m_SimpleStacksize.GetWindowText(comp[compSel].stacksize, COMP_STACKSIZE_L);
	}

	CDialog::OnOK();
}

void CEditComp::OnCancel() 
{
	// TODO: Add extra cleanup here

	CDialog::OnCancel();
}


void CEditComp::OnSimpleActivity() 
{
	// TODO: Add your control notification handler code here
	isActivity = true;
	m_SimpleStacksize.SetReadOnly(FALSE);
}

void CEditComp::OnSimpleHandlemessage() 
{
	// TODO: Add your control notification handler code here
	isActivity = false;
	m_SimpleStacksize.SetReadOnly(TRUE);

}
