AddNedFile macro
================
Activate macro file:
  Tools|Customize --> select Add-ins and Macro Files tab --> in the listbox,
  tick 'omnetpp' item
Add to toolbar:
  Tools|Customize again --> select Commands tab --> from the Category combo,
  select Macros --> addNEDfileToProject should appear in the listbox -->
  drag it to a toolbar --> select icon for it --> close dialog
Usage:
  click icon --> inputbox appears --> type ned file name --> the macro will
  add it to the project
Possible caveats:
  - the macro occasionally crashes the Scripting Host program and MSVC :-(
    (very remarkable from a Visual Basic macro, isn't it?)
  - the nedc compiler is referred to as 'nedc' in the project file, which
    means the omnetpp bin directrory should be in the path. Or, you can
    substitute the full path of nedc (e.g. c:\omnetpp\bin\ned.exe), either
    in the Custom Build Step for the NED files, or by find/replace in
    the dsp file.

OMNeT++ AppWizard for MSVC
==========================

Installation (should be done by install program):
  copy the binary (OmnetppWizard.awx) into C:\Program Files\Microsoft Visual
  Studio\Common\MSDev98\Template

Usage:
  Select start MSVC, select File|New... from the menu, choose "OMNeT++
  AppWizard" from the list. Then enjoy...

Tested with MSVC 6.0, NT4.0.

Todo:
- add pictures
- make separate source file group in the project tree for NED files
- add this notice: "The warning when loading the project for the first time
  can be ignored"
- add option: [ ] include sample code in generated sources
- add copyright statement (GPL)

