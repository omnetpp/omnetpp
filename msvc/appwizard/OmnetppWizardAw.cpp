// OmnetppWizardaw.cpp : implementation file
//

#include "stdafx.h"
#include "OmnetppWizard.h"
#include "OmnetppWizardaw.h"
#include "chooser.h"

#include "ObjModel/bldauto.h"
#include "ObjModel/bldguid.h"
#include "ObjModel/blddefs.h"


#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void COmnetppWizardAppWiz::InitCustomAppWiz()
{
	// Create a new dialog chooser; CDialogChooser's constructor initializes
	//  its internal array with pointers to the steps.
	m_pChooser = new CDialogChooser;

	// Set the maximum number of steps.
	SetNumberOfSteps(LAST_DLG);

	// TODO: Add any other custom AppWizard-wide initialization here.
	if(!GetFromReg("RootPath", OppDir))
		strcpy(OppDir, OPP_DIR);
	if(!GetFromReg("TclPath", TclDir))
		strcpy(TclDir, TCL_DIR);
	if(!GetFromReg("TclVer", TclVer))
		strcpy(TclVer, TCL_VER);
}

// This is called just before the custom AppWizard is unloaded.
void COmnetppWizardAppWiz::ExitCustomAppWiz()
{
	// Deallocate memory used for the dialog chooser
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;

	// TODO: Add code here to deallocate resources used by the custom AppWizard
	PutToReg("RootPath", OppDir);
	PutToReg("TclPath", TclDir);
	PutToReg("TclVer", TclVer);


	char in[MAX_PATH], out[MAX_PATH];

	sprintf(in, "%s.dsp.in", m_Dictionary["ROOT"]);
	sprintf(out, "%s.dsp", m_Dictionary["ROOT"]);
	CopyFile(in, out, FALSE);

	// A generalt cpp-k legyenek benne a project-ben, de nem kell megjeleniuk
	for(int i=0; i<compN; i++)
	{
		sprintf(out, "%s_n.cpp", comp[i].name);
		unlink(out);
	}
}

// This is called when the user clicks "Create..." on the New Project dialog
//  or "Next" on one of the custom AppWizard's steps.
CAppWizStepDlg* COmnetppWizardAppWiz::Next(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Next(pDlg);
}

// This is called when the user clicks "Back" on one of the custom
//  AppWizard's steps.
CAppWizStepDlg* COmnetppWizardAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return m_pChooser->Back(pDlg);
}

void COmnetppWizardAppWiz::CustomizeProject(IBuildProject* pProject)
{
	// TODO: Add code here to customize the project.  If you don't wish
	//  to customize project, you may remove this virtual override.

	// This is called immediately after the default Debug and Release
	//  configurations have been created for each platform.  You may customize
	//  existing configurations on this project by using the methods
	//  of IBuildProject and IConfiguration such as AddToolSettings,
	//  RemoveToolSettings, and AddCustomBuildStep. These are documented in
	//  the Developer Studio object model documentation.

	// WARNING!!  IBuildProject and all interfaces you can get from it are OLE
	//  COM interfaces.  You must be careful to release all new interfaces
	//  you acquire.  In accordance with the standard rules of COM, you must
	//  NOT release pProject, unless you explicitly AddRef it, since pProject
	//  is passed as an "in" parameter to this function.  See the documentation
	//  on CCustomAppWiz::CustomizeProject for more information.
//	static struct tagVARIANT reserved;
//	CComVariant reserced=NULL;

//	pProject->AddConfiguration((unsigned short*)"Release TkEnv", reserved);


}

bool COmnetppWizardAppWiz::GetFromReg(char *variable, char *value)
{
	HKEY hk;
	unsigned long type;
	unsigned long size;

	if(RegOpenKey(HKEY_CURRENT_USER, REG_KEY, &hk) != ERROR_SUCCESS)
		return false;
	size = 255;
	if(RegQueryValueEx(hk, variable, NULL, &type, (unsigned char *)value, &size) != ERROR_SUCCESS)
		return false;
	if(RegCloseKey(hk) != ERROR_SUCCESS)
		return false;

	return true;
}

bool COmnetppWizardAppWiz::PutToReg(char *variable, char *value)
{
	HKEY hk;
	unsigned long type;
	unsigned long size;

	if(RegCreateKey(HKEY_CURRENT_USER, REG_KEY, &hk) != ERROR_SUCCESS)
		return FALSE;
	size = strlen(value);
	type = REG_SZ;
	if(RegSetValueEx(hk, variable, NULL, type, (unsigned char *)value, size) != ERROR_SUCCESS)
		return FALSE;
	if(RegCloseKey(hk) != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

// Here we define one instance of the COmnetppWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global OmnetppWizardaw.
COmnetppWizardAppWiz OmnetppWizardaw;

