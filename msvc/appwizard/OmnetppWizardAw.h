#if !defined(AFX_OMNETPPWIZARDAW_H__51F2F248_395D_11D4_931B_005004331574__INCLUDED_)
#define AFX_OMNETPPWIZARDAW_H__51F2F248_395D_11D4_931B_005004331574__INCLUDED_

// OmnetppWizardaw.h : header file
//

class CDialogChooser;

// All function calls made by mfcapwz.dll to this custom AppWizard (except for
//  GetCustomAppWizClass-- see OmnetppWizard.cpp) are through this class.  You may
//  choose to override more of the CCustomAppWiz virtual functions here to
//  further specialize the behavior of this custom AppWizard.
class COmnetppWizardAppWiz : public CCustomAppWiz
{
public:
	virtual CAppWizStepDlg* Next(CAppWizStepDlg* pDlg);
	virtual CAppWizStepDlg* Back(CAppWizStepDlg* pDlg);

	virtual void InitCustomAppWiz();
	virtual void ExitCustomAppWiz();
	virtual void CustomizeProject(IBuildProject* pProject);

protected:
	CDialogChooser* m_pChooser;
	bool GetFromReg(HKEY hkey, char *subkey, char *variable, char *value);
	bool PutToReg(HKEY hkey, char *subkey, char *variable, char *value);
};

// This declares the one instance of the COmnetppWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global OmnetppWizardaw.  (Its definition is in OmnetppWizardaw.cpp.)
extern COmnetppWizardAppWiz OmnetppWizardaw;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMNETPPWIZARDAW_H__51F2F248_395D_11D4_931B_005004331574__INCLUDED_)
