# Microsoft Developer Studio Project File - Name="OmnetppWizard" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OmnetppWizard - Win32 Pseudo-Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "OmnetppWizard.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "OmnetppWizard.mak" CFG="OmnetppWizard - Win32 Pseudo-Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "OmnetppWizard - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OmnetppWizard - Win32 Pseudo-Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OmnetppWizard - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Ext "awx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Ext "awx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386
# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=.\Release
TargetPath=.\Release\OmnetppWizard.awx
TargetName=OmnetppWizard
InputPath=.\Release\OmnetppWizard.awx
SOURCE="$(InputPath)"

"$(MSDEVDIR)\Template\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist "$(MSDEVDIR)\Template\nul" md "$(MSDEVDIR)\Template"
	copy "$(TargetPath)" "$(MSDEVDIR)\Template"
	if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb" "$(MSDEVDIR)\Template"

# End Custom Build

!ELSEIF  "$(CFG)" == "OmnetppWizard - Win32 Pseudo-Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Ext "awx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Ext "awx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /Gm /GX /Zi /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PSEUDO_DEBUG" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_PSEUDO_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386
# Begin Custom Build - Copying custom AppWizard to Template directory...
OutDir=.\Debug
TargetPath=.\Debug\OmnetppWizard.awx
TargetName=OmnetppWizard
InputPath=.\Debug\OmnetppWizard.awx
SOURCE="$(InputPath)"

"$(MSDEVDIR)\Template\$(TargetName).awx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if not exist "$(MSDEVDIR)\Template\nul" md "$(MSDEVDIR)\Template"
	copy "$(TargetPath)" "$(MSDEVDIR)\Template"
	if exist "$(OutDir)\$(TargetName).pdb" copy "$(OutDir)\$(TargetName).pdb" "$(MSDEVDIR)\Template"

# End Custom Build

!ENDIF

# Begin Target

# Name "OmnetppWizard - Win32 Release"
# Name "OmnetppWizard - Win32 Pseudo-Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Chooser.cpp
# End Source File
# Begin Source File

SOURCE=.\Cstm1Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Debug.cpp
# End Source File
# Begin Source File

SOURCE=.\Dirs.cpp
# End Source File
# Begin Source File

SOURCE=.\EditComp.cpp
# End Source File
# Begin Source File

SOURCE=.\OmnetppWizard.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\OmnetppWizard.hpj

!IF  "$(CFG)" == "OmnetppWizard - Win32 Release"

# PROP Ignore_Default_Tool 1
USERDEP__OMNET="hlp\AfxCore.rtf"	"hlp\AfxPrint.rtf"	"hlp\$(TargetName).hm"
# Begin Custom Build - Making help file...
OutDir=.\Release
TargetName=OmnetppWizard
InputPath=.\hlp\OmnetppWizard.hpj
InputName=OmnetppWizard

BuildCmds= \
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" \
	if errorlevel 1 goto :Error \
	if not exist "hlp\$(InputName).hlp" goto :Error \
	copy "hlp\$(InputName).hlp" $(OutDir) \
	copy "$(OutDir)\$(TargetName).hlp" "$(MSDEVDIR)\Template" \
	goto :done \
	:Error \
	echo hlp\$(InputName).hpj(1) : error: \
	type "hlp\$(InputName).log" \
	:done \


"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(MSDEVDIR)\Template" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "OmnetppWizard - Win32 Pseudo-Debug"

# PROP Ignore_Default_Tool 1
USERDEP__OMNET="hlp\AfxCore.rtf"	"hlp\AfxPrint.rtf"	"hlp\$(TargetName).hm"
# Begin Custom Build - Making help file...
OutDir=.\Debug
TargetName=OmnetppWizard
InputPath=.\hlp\OmnetppWizard.hpj
InputName=OmnetppWizard

BuildCmds= \
	start /wait hcw /C /E /M "hlp\$(InputName).hpj" \
	if errorlevel 1 goto :Error \
	if not exist "hlp\$(InputName).hlp" goto :Error \
	copy "hlp\$(InputName).hlp" $(OutDir) \
	copy "$(OutDir)\$(TargetName).hlp" "$(MSDEVDIR)\Template" \
	goto :done \
	:Error \
	echo hlp\$(InputName).hpj(1) : error: \
	type "hlp\$(InputName).log" \
	:done \


"$(OutDir)\$(InputName).hlp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(MSDEVDIR)\Template" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\OmnetppWizard.rc
# End Source File
# Begin Source File

SOURCE=.\OmnetppWizardAw.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Chooser.h
# End Source File
# Begin Source File

SOURCE=.\Cstm1Dlg.h
# End Source File
# Begin Source File

SOURCE=.\Debug.h
# End Source File
# Begin Source File

SOURCE=.\Dirs.h
# End Source File
# Begin Source File

SOURCE=.\EditComp.h
# End Source File
# Begin Source File

SOURCE=.\OmnetppWizard.h
# End Source File
# Begin Source File

SOURCE=.\OmnetppWizardAw.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h

!IF  "$(CFG)" == "OmnetppWizard - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=OmnetppWizard
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm"
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm"
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm"
	echo. >>"hlp\$(TargetName).hm"
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm"
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm"
	echo. >>"hlp\$(TargetName).hm"
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm"
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm"
	echo. >>"hlp\$(TargetName).hm"
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm"
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm"
	echo. >>"hlp\$(TargetName).hm"
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm"
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm"

# End Custom Build

!ELSEIF  "$(CFG)" == "OmnetppWizard - Win32 Pseudo-Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Making help include file...
TargetName=OmnetppWizard
InputPath=.\Resource.h

"hlp\$(TargetName).hm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo. >"hlp\$(TargetName).hm"
	echo // Commands (ID_* and IDM_*) >>"hlp\$(TargetName).hm"
	makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>"hlp\$(TargetName).hm"
	echo. >>"hlp\$(TargetName).hm"
	echo // Prompts (IDP_*) >>"hlp\$(TargetName).hm"
	makehm IDP_,HIDP_,0x30000 resource.h >>"hlp\$(TargetName).hm"
	echo. >>"hlp\$(TargetName).hm"
	echo // Resources (IDR_*) >>"hlp\$(TargetName).hm"
	makehm IDR_,HIDR_,0x20000 resource.h >>"hlp\$(TargetName).hm"
	echo. >>"hlp\$(TargetName).hm"
	echo // Dialogs (IDD_*) >>"hlp\$(TargetName).hm"
	makehm IDD_,HIDD_,0x20000 resource.h >>"hlp\$(TargetName).hm"
	echo. >>"hlp\$(TargetName).hm"
	echo // Frame Controls (IDW_*) >>"hlp\$(TargetName).hm"
	makehm IDW_,HIDW_,0x50000 resource.h >>"hlp\$(TargetName).hm"

# End Custom Build

!ENDIF

# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\omnetpp1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\OmnetppWizard.ico
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter "cnt;rtf"
# Begin Source File

SOURCE=.\hlp\OmnetppWizard.rtf
# End Source File
# End Group
# Begin Group "Template Files"

# PROP Default_Filter "<templates>"
# Begin Source File

SOURCE=.\Template\activity.cpp
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\confirm.inf
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\handlemessage.cpp
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\network.ned
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\newproj.inf
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\omnetpp.ini
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\template.dsp
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\template.h
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\template.ned
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Template\template_n.cpp
# PROP Exclude_From_Scan -1
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
