# Microsoft Developer Studio Project File - Name="$$root$$" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=$$root$$ - Win32 Debug Tkenv
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "$$root$$.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "$$root$$.mak" CFG="$$root$$ - Win32 Debug Tkenv"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "$$root$$ - Win32 Debug Tkenv" (based on "Win32 (x86) Console Application")
!MESSAGE "$$root$$ - Win32 Debug Cmdenv" (based on "Win32 (x86) Console Application")
!MESSAGE "$$root$$ - Win32 Release Tkenv" (based on "Win32 (x86) Console Application")
!MESSAGE "$$root$$ - Win32 Release Cmdenv" (based on "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "$$root$$ - Win32 Debug Tkenv"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "$$root$$___Win32_Debug_Tkenv"
# PROP BASE Intermediate_Dir "$$root$$___Win32_Debug_Tkenv"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /ZI /Od /I "$$OPPDIR$$\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /ZI /Od /I "$$OPPDIR$$\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sim_std.lib envir.lib tkenv.lib tcl$$TCLVER$$.lib tk$$TCLVER$$.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0x1000000 /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"$$OPPDIR$$\lib" /libpath:"$$TCLDIR$$"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Debug Cmdenv"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "$$root$$___Win32_Debug_Cmdenv"
# PROP BASE Intermediate_Dir "$$root$$___Win32_Debug_Cmdenv"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /ZI /Od /I "$$OPPDIR$$\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /ZI /Od /I "$$OPPDIR$$\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 sim_std.lib envir.lib cmdenv.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0x1000000 /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"$$OPPDIR$$\lib"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Release Tkenv"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "$$root$$___Win32_Release_Tkenv"
# PROP BASE Intermediate_Dir "$$root$$___Win32_Release_Tkenv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /O2 /I "$$OPPDIR$$\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /I "$$OPPDIR$$\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 sim_std.lib envir.lib tkenv.lib tcl$$TCLVER$$.lib tk$$TCLVER$$.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0x1000000 /subsystem:console /machine:I386 /libpath:"$$OPPDIR$$\lib" /libpath:"$$TCLDIR$$"
# SUBTRACT LINK32 /nodefaultlib

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Release Cmdenv"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "$$root$$___Win32_Release_Cmdenv"
# PROP BASE Intermediate_Dir "$$root$$___Win32_Release_Cmdenv"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "."
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /O2 /I "$$OPPDIR$$\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /O2 /I "$$OPPDIR$$\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 sim_std.lib envir.lib cmdenv.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /stack:0x1000000 /subsystem:console /machine:I386 /libpath:"$$OPPDIR$$\lib"
# SUBTRACT LINK32 /nodefaultlib

!ENDIF

# Begin Target

# Name "$$root$$ - Win32 Debug Tkenv"
# Name "$$root$$ - Win32 Debug Cmdenv"
# Name "$$root$$ - Win32 Release Tkenv"
# Name "$$root$$ - Win32 Release Cmdenv"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;ned"
# Begin Source File

SOURCE=.\$$root$$.ned

!IF  "$(CFG)" == "$$root$$ - Win32 Debug Tkenv"

# Begin Custom Build - NED Compiling $(InputPath)
InputPath=.\$$root$$.ned
InputName=$$root$$

"$(InputName)_n.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$$OPPDIR$$\bin\nedc.exe -s _n.cpp $(InputName).ned

# End Custom Build

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Debug Cmdenv"

# Begin Custom Build - NED Compiling $(InputPath)
InputPath=.\$$root$$.ned
InputName=$$root$$

"$(InputName)_n.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$$OPPDIR$$\bin\nedc.exe -s _n.cpp $(InputName).ned

# End Custom Build

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Release Tkenv"

# Begin Custom Build - NED Compiling $(InputPath)
InputPath=.\$$root$$.ned
InputName=$$root$$

"$(InputName)_n.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$$OPPDIR$$\bin\nedc.exe -s _n.cpp $(InputName).ned

# End Custom Build

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Release Cmdenv"

# Begin Custom Build - NED Compiling $(InputPath)
InputPath=.\$$root$$.ned
InputName=$$root$$

"$(InputName)_n.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$$OPPDIR$$\bin\nedc.exe -s _n.cpp $(InputName).ned

# End Custom Build

!ENDIF



# End Source File
# Begin Source File

SOURCE=.\$$root$$_n.cpp
# End Source File
$$BEGINLOOP(SIMPLE_N)
# Begin Source File

SOURCE=.\$$MODULE_NAME$$.ned

!IF  "$(CFG)" == "$$root$$ - Win32 Debug Tkenv"

# Begin Custom Build - NED Compiling $(InputPath)
InputPath=.\$$MODULE_NAME$$.ned
InputName=$$MODULE_NAME$$

"$(InputName)_n.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$$OPPDIR$$\bin\nedc.exe -s _n.cpp $(InputName).ned

# End Custom Build

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Debug Cmdenv"

# Begin Custom Build - NED Compiling $(InputPath)
InputPath=.\$$MODULE_NAME$$.ned
InputName=$$MODULE_NAME$$

"$(InputName)_n.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$$OPPDIR$$\bin\nedc.exe -s _n.cpp $(InputName).ned

# End Custom Build

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Release Tkenv"

# Begin Custom Build - NED Compiling $(InputPath)
InputPath=.\$$MODULE_NAME$$.ned
InputName=$$MODULE_NAME$$

"$(InputName)_n.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$$OPPDIR$$\bin\nedc.exe -s _n.cpp $(InputName).ned

# End Custom Build

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 Release Cmdenv"

# Begin Custom Build - NED Compiling $(InputPath)
InputPath=.\$$MODULE_NAME$$.ned
InputName=$$MODULE_NAME$$

"$(InputName)_n.cpp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$$OPPDIR$$\bin\nedc.exe -s _n.cpp $(InputName).ned

# End Custom Build

!ENDIF



# End Source File
# Begin Source File

SOURCE=.\$$MODULE_NAME$$_n.cpp
# End Source File
# Begin Source File

SOURCE=.\$$MODULE_NAME$$.cpp
# End Source File
$$ENDLOOP // MODULE_NAME_N
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
$$//# Begin Source File
$$//
$$//SOURCE=.\$$root$$.h
$$//# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\omnetpp.ini
# End Source File
# End Target
# End Project
