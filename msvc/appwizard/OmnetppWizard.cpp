// OmnetppWizard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>
#include "OmnetppWizard.h"
#include "OmnetppWizardaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static AFX_EXTENSION_MODULE OmnetppWizardDLL = { NULL, NULL };

//char comp[COMP_N][COMP_NAME_L+1];
char projectDescription[PROJ_DESC_L+1] = "\0";
comp_t comp[COMP_N];
int compN = 0;
int compSel = 0;
char OppDir[MAX_PATH+1];
char TclDir[MAX_PATH+1];
char TclVer[MAX_PATH+1];

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("OMNETPPWIZARD.AWX Initializing!\n");

		// Extension DLL one-time initialization
		AfxInitExtensionModule(OmnetppWizardDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(OmnetppWizardDLL);

		// Register this custom AppWizard with MFCAPWZ.DLL
		SetCustomAppWizClass(&OmnetppWizardaw);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("OMNETPPWIZARD.AWX Terminating!\n");

		// Terminate the library before destructors are called
		AfxTermExtensionModule(OmnetppWizardDLL);
	}
	return 1;   // ok
}
