#if !defined(AFX_OMNETPPWIZARD_H__51F2F244_395D_11D4_931B_005004331574__INCLUDED_)
#define AFX_OMNETPPWIZARD_H__51F2F244_395D_11D4_931B_005004331574__INCLUDED_

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

// TODO: You may add any other custom AppWizard-wide declarations here.
#define OPP_DIR "c:\\OMNeT++"
#define TCL_DIR "c:\\OMNeT++\\lib"
#define TCL_VER "8.2"

#define PROJ_DESC_L 300
#define COMP_N      100
#define COMP_NAME_L  30
#define COMP_DESCRIPTION_L  300
#define COMP_STACKSIZE_L  6
typedef struct
{
	char name[COMP_NAME_L+1];
	char description[COMP_DESCRIPTION_L+1];
	bool isActivity;
	char stacksize[COMP_STACKSIZE_L+1];

} comp_t;
extern comp_t comp[COMP_N];
#define COMP_L sizeof(comp_t)
//extern char comp[COMP_N][COMP_NAME_L+1];
extern char projectDescription[PROJ_DESC_L+1];
extern int compN;
extern int compSel;
extern char OppDir[MAX_PATH+1];
extern char TclDir[MAX_PATH+1];
extern char TclVer[MAX_PATH+1];


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMNETPPWIZARD_H__51F2F244_395D_11D4_931B_005004331574__INCLUDED_)
