; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCustom1Dlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "OmnetppWizard.h"

ClassCount=3
Class1=CCustom1Dlg

ResourceCount=4
Resource1=IDD_CUSTOM1
Class2=CEditComp
Resource2=IDD_CUSTOM1 (DIRS)
Resource3=IDD_EDIT
Class3=CDirs
Resource4=IDD_DIRS


[CLS:CCustom1Dlg]
Type=0
HeaderFile=cstm1dlg.h
ImplementationFile=cstm1dlg.cpp
Filter=D
LastObject=IDC_DESCRIPTION
BaseClass=CAppWizStepDlg
VirtualFilter=dWC

[DLG:IDD_EDIT]
Type=1
Class=CEditComp
ControlCount=11
Control1=IDC_SIMPLE_NAME,edit,1350631552
Control2=IDC_STATIC,static,1342308352
Control3=IDC_SIMPLE_DESCRIPTION,edit,1350635716
Control4=IDC_STATIC,static,1342308352
Control5=IDC_SIMPLE_ACTIVITY,button,1342177289
Control6=IDC_STATIC,button,1342177287
Control7=IDC_SIMPLE_HANDLEMESSAGE,button,1342177289
Control8=IDC_SIMPLE_STACKSIZE,edit,1350639744
Control9=IDC_STATIC,static,1342308352
Control10=IDOK,button,1342242817
Control11=IDCANCEL,button,1342242816

[CLS:CEditComp]
Type=0
HeaderFile=EditComp.h
ImplementationFile=EditComp.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CEditComp

[DLG:IDD_CUSTOM1 (DIRS)]
Type=1
Class=CCustom1Dlg
ControlCount=8
Control1=IDC_STATIC,button,1342210055
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,button,1342177287
Control4=IDC_COMP_LIST,listbox,1352728835
Control5=IDC_ADD,button,1342242816
Control6=IDC_EDIT,button,1342242816
Control7=IDC_DELETE,button,1342242816
Control8=IDC_STATIC,static,1342179342

[DLG:IDD_CUSTOM1]
Type=1
Class=CCustom1Dlg
ControlCount=10
Control1=IDC_STATIC,button,1342210055
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,button,1342177287
Control4=IDC_SIMPLE_LIST,listbox,1352728835
Control5=IDC_ADD,button,1342242816
Control6=IDC_EDIT,button,1342242816
Control7=IDC_DELETE,button,1342242816
Control8=IDC_STATIC,static,1342177294
Control9=IDC_DESCRIPTION,edit,1350635716
Control10=IDC_STATIC,static,1342308352

[DLG:IDD_DIRS]
Type=1
Class=CDirs
ControlCount=11
Control1=IDC_STATIC,button,1342210055
Control2=IDC_STATIC,button,1342177287
Control3=IDC_OPPDIR,edit,1350631552
Control4=IDC_OPPDIR_BROWSE,button,1342242816
Control5=IDC_STATIC,button,1342177287
Control6=IDC_TCLDIR,edit,1350631552
Control7=IDC_TCLDIR_BROWSE,button,1342242816
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_TCLVER,edit,1350631552
Control11=IDC_STATIC,static,1342177294

[CLS:CDirs]
Type=0
HeaderFile=Dirs.h
ImplementationFile=Dirs.cpp
BaseClass=CDialog
Filter=D
LastObject=CDirs
VirtualFilter=dWC

