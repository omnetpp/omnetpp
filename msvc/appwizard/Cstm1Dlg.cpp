// cstm1dlg.cpp : implementation file
//

#include "stdafx.h"
#include "OmnetppWizard.h"
#include "cstm1dlg.h"
#include "OmnetppWizardaw.h"

#include "EditComp.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg dialog

CCustom1Dlg::CCustom1Dlg()
	: CAppWizStepDlg(CCustom1Dlg::IDD)
{
	//{{AFX_DATA_INIT(CCustom1Dlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCustom1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustom1Dlg)
	DDX_Control(pDX, IDC_DESCRIPTION, m_Description);
	DDX_Control(pDX, IDC_COMP_LIST, m_CompList);
	//}}AFX_DATA_MAP

//	m_Description.SetWindowText(projectDescription);
	m_Description.SetLimitText(PROJ_DESC_L);
}

// This is called whenever the user presses Next, Back, or Finish with this step
//  present.  Do all validation & data exchange from the dialog in this function.
BOOL CCustom1Dlg::OnDismiss()
{
	if (!UpdateData(TRUE))
		return FALSE;

	// TODO: Set template variables based on the dialog's data.
	int i,j,k;
	char nameStr[COMP_NAME_L+4];
	char tmp[PROJ_DESC_L+1];
	
	/* Get project descriotion */
	m_Description.GetWindowText(tmp, PROJ_DESC_L);
	for(i=0,j=0; j<PROJ_DESC_L; i++)
	{
		if(tmp[i] == '\r')
		{
			strcpy(&(projectDescription[j]), "\n// ");
			j += 4; i += 1;
		}
		else
		{
			projectDescription[j++] = tmp[i];
			if(tmp[i] == '\0')
				break;
		}
	}

	sprintf(nameStr, "%d", compN);
	OmnetppWizardaw.m_Dictionary["SIMPLE_N"] = nameStr;
	for(i=0; i< compN; i++)
	{
		sprintf(nameStr, "MODULE_NAME_%d", i);
		OmnetppWizardaw.m_Dictionary[nameStr] = comp[i].name;

		sprintf(nameStr, "DESCRIPTION_%d", i);
		for(k=0,j=0; j<COMP_DESCRIPTION_L; k++)
		{
			if(comp[i].description[k] == '\r')
			{
				strcpy(&(tmp[j]), "\n// ");
				j += 4; k += 1;
			}
			else
			{
				tmp[j++] = comp[i].description[k];
				if(comp[i].description[k] == '\0')
					break;
			}
		}
		OmnetppWizardaw.m_Dictionary[nameStr] = tmp;

		sprintf(nameStr, "IS_ACTIVITY_%d", i);
		if(comp[i].isActivity == true)
			OmnetppWizardaw.m_Dictionary[nameStr] = "1";
		else
			OmnetppWizardaw.m_Dictionary.RemoveKey(nameStr);

		sprintf(nameStr, "STACKSIZE_%d", i);
		OmnetppWizardaw.m_Dictionary[nameStr] = comp[i].stacksize;
	}
	OmnetppWizardaw.m_Dictionary["PROJECT_DESCRIPTION"] = projectDescription;

	return TRUE;	// return FALSE if the dialog shouldn't be dismissed
}


BEGIN_MESSAGE_MAP(CCustom1Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(CCustom1Dlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_LBN_DBLCLK(IDC_COMP_LIST, OnDblclkCompList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg message handlers

void CCustom1Dlg::OnAdd() 
{
	// TODO: Add your control notification handler code here
	CEditComp dlg;

//	dlg.m_CompName.SetDlgItemText(IDC_COMP_NAME, "New");
//	dlg.m_CompName.SetWindowText("New");
	compSel = -1;
	dlg.DoModal();
	Refresh();
}

void CCustom1Dlg::OnDelete() 
{
	// TODO: Add your control notification handler code here
	int i;

	compSel = m_CompList.GetCurSel();
	if(compSel == LB_ERR)
		return;

	for(i = (compSel+1); i < compN; i++)
//		strcpy(comp[i-1], comp[i]);
		memcpy((void*)(&comp[i-1]), (void*)(&comp[i]), COMP_L);

	compN -= 1;

	Refresh();
}

void CCustom1Dlg::OnEdit() 
{
	// TODO: Add your control notification handler code here
	CEditComp dlg;

	compSel = m_CompList.GetCurSel();
	if(compSel == LB_ERR)
		return;

	dlg.DoModal();
	Refresh();
	
}


void CCustom1Dlg::Refresh() 
{
	int i;

	m_CompList.ResetContent();

	for(i = 0; i < compN; i++)
		m_CompList.AddString(comp[i].name);
}

void CCustom1Dlg::OnDblclkCompList() 
{
	// TODO: Add your control notification handler code here
	OnEdit();
}
