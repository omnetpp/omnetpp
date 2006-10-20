Visual C++ Integration
======================

There are two macros provided:
  - addMSGFileToProject: adds blank .msg file with opp_msgc invocation as
                         Custom Build Step, and corresponding _m.cpp/h file
  - addNEDFileToProject: adds blank .ned file with nedtool invocation as
                         Custom Build Step, and corresponding _n.cpp file

Requirements: MSVC 6.0.SP1. Without SP1, AddNedFile may crash the IDE due to
bugs in the Visual Studio Scripting Host.

NOTE: An OMNeT++ Wizard is not provided in OMNeT++/OMNEST 3.0 or later;
we recommend that you start your project by copying one of the sample
simulations provided.

Also: since OMNeT++/OMNEST 3.0 and up supports dynamic NED loading, there's
usually little point in translating NED files to C++ (_m.cpp files) and
compiling them into the executable. As a result, you probably WON'T NEED
THE ADDNEDFILETOPROJECT MACRO AT ALL. You can still add the NED files
to the project though as a resource that requires no processing, via the
normal ways, e.g. drag&drop from Explorer. To enable dynamic loading, add the
      preload-ned-files = *.ned
line under the [General] section of your omnetpp.ini.


Installation of the macros:
1. The macro file (omnetpp.dsm) should be copied into the following directory:
   C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Macros. This is
   normally done by the OMNeT++/OMNEST installer.

2. Activate the macro file within the MSVC IDE:
   Tools|Customize --> select Add-ins and Macro Files tab --> in the listbox,
   tick the 'omnetpp' item. This should also be done by the OMNeT++/OMNEST
   installer.

3. Add icons to the MSVC toolbar:
   Tools|Customize --> select Commands tab --> from the Category combo,
   select Macros --> addMSGFileToProject and addNEDFileToProject should appear
   in the listbox --> drag them to a toolbar --> pick icons for them --> close
   dialog.

Possible caveat with both addMSGFileToProject and addNEDFileToProject:
 - Building the simulation may report 'nedtool: invalid command or file name'
   or 'perl: invalid command or filename'. This is due to the bug in the MSVC IDE:
   it apparently doesn't use the normal PATH environment variable for the
   Custom Build Step. Workaround is to substitute the full path of nedtool
   or opp_msgc (e.g. C:\OMNeT++\bin\nedtool.exe) either in the Custom Build Step
   page of the Settings dialog for the .ned and .msg files, or by find/replace
   in the dsp file.


