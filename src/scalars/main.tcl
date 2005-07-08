#=================================================================
#  MAIN.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2005 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


# config
set config(configfile)       "~/.scalarsrc"

#
# simple debug-print procedure
#
proc debug {str} {
   # uncomment the next line if you want debug messages
   #puts "dbg: $str"
}

proc createMenubar {w} {

    global widget tcl_version

    #################################
    # Menu bar
    #################################

    # Create menus
    foreach i {
       {filemenu     -$label_opt File -underline 0}
       {editmenu     -$label_opt Edit -underline 0}
       {chartmenu    -$label_opt Chart -underline 0}
       {helpmenu     -$label_opt Help -underline 0}
    } {
       set label_opt "label"; set m ""
       eval $w add cascade -menu $w.$i
       menu "$w.[lindex $i 0]" -tearoff 0
    }
    # not yet:   {optionsmenu  -$label_opt Options -underline 0}

    # File menu
    foreach i {
      {command -command fileLoad -label {Load output scalar file...} -underline 0}
      {separator}
      {command -command fileExit -label Exit -underline 1}
    } {
       eval $w.filemenu$m add $i
    }

    # Edit menu
    foreach i {
      {command -command editCopy -label {Copy} -underline 0}
    } {
      eval $w.editmenu$m add $i
    }

    # Chart menu
    foreach i {
      {command -command chartCreateBarChart -label {Create bar chart...} -underline 7}
      {command -command chartCreateScatterPlot -label {Create scatter plot...} -underline 7}
    } {
      eval $w.chartmenu$m add $i
    }

    # Options menu
    #foreach i {
    #  {separator}
    #} {
    #  eval $w.optionsmenu$m add $i
    #}

    # Help menu
    foreach i {
      {command -command helpAbout -label {About OMNeT++/Scalars...} -underline 0}
    } {
      eval $w.helpmenu$m add $i
    }
}

proc createMainArea {w} {

    global fonts help_tips tmp

    frame $w.f
    pack $w.f -expand 1 -fill both

    # two panels: $w.f.filter is the upper panel for filters, and
    # $w.f.main is the lower one with the listbox.

    # panel for filters
    frame $w.f.filter
    pack $w.f.filter -anchor center -expand 0 -fill x -side top

    #labelframe $w.f.filter.pars -text "Filter list of scalars displayed"
    frame $w.f.filter.pars
    pack $w.f.filter.pars -anchor center -expand 0 -fill x -side top
    set fp $w.f.filter.pars

    label $fp.filelabel -text "File name and run number:" -justify left -anchor w
    label $fp.modulelabel -text "Module name:" -justify left -anchor w
    label $fp.namelabel -text "Scalar name:" -justify left -anchor w

    #combo $fp.fileentry [concat {{}} [getClassNames]]
    combo $fp.fileentry {*}
    $fp.fileentry.entry config -textvariable tmp(file)
    combo $fp.moduleentry {*}
    $fp.moduleentry.entry config -textvariable tmp(module)
    combo $fp.nameentry {*}
    $fp.nameentry.entry config -textvariable tmp(name)

    #set filehelptext "Wildcards accepted (*,?)"
    #set modulehelptext "Wildcards accepted (*,?)"
    #set namehelptext "Wildcards accepted (*,?)"
    #label $fp.filehelp -text $filehelptext -justify left -anchor w
    #label $fp.modulehelp -text $modulehelptext -justify left -anchor w
    #label $fp.namehelp -text $namehelptext -justify left -anchor w

    grid $fp.filelabel $fp.modulelabel $fp.namelabel -sticky news -padx 5
    grid $fp.fileentry $fp.moduleentry $fp.nameentry -sticky news -padx 5
    #grid $fp.filehelp  $fp.modulehelp  $fp.namehelp  -sticky news -padx 5
    grid columnconfig $fp 0 -weight 1
    grid columnconfig $fp 1 -weight 2
    grid columnconfig $fp 2 -weight 2

    # "Refresh" button
    frame $w.f.filter.buttons
    pack $w.f.filter.buttons -anchor center -expand 1 -fill x -side top
    button $w.f.filter.buttons.refresh -text "Apply filter" -width 10 -command "applyFilter"
    pack $w.f.filter.buttons.refresh -anchor e -expand 0 -fill none -side top -padx 5 -pady 5

    # panel for listbox
    frame $w.f.main
    scrollbar $w.f.main.vsb -command "$w.f.main.list yview"
    scrollbar $w.f.main.hsb -command "$w.f.main.list xview" -orient horiz
    multicolumnlistbox $w.f.main.list {
        {dir     Directory 50}
        {file    File      70}
        {run     Run#}
        {module  Module    150}
        {name    Name      130}
        {value   Value     100}
    } -height 200 -yscrollcommand "$w.f.main.vsb set" -xscrollcommand "$w.f.main.hsb set"

    grid $w.f.main.list $w.f.main.vsb -sticky news
    grid $w.f.main.hsb  x             -sticky news
    grid rowconfig $w.f.main 0 -weight 1
    grid columnconfig $w.f.main 0 -weight 1

    pack $w.f.main  -anchor center -expand 1 -fill both -side top

    set lb $w.f.main.list

    set type "(default)"
    #button $w.buttons.inspect -text "Open inspector" -command "inspectfromlistbox_insp $lb \{$type\}; after 500 \{raise $w; focus $lb\}"
    #pack $w.buttons.inspect -side top -anchor e -padx 2

    # leave listbox empty -- filling it with all objects might take too long

    # Configure dialog
    bind $fp.fileentry.entry <Return> "$w.f.filter.buttons.refresh invoke"
    bind $fp.moduleentry.entry <Return> "$w.f.filter.buttons.refresh invoke"
    bind $fp.nameentry.entry <Return> "$w.f.filter.buttons.refresh invoke"

    focus $fp.nameentry

}

proc createMainWindow {{geom ""}} {

    global widget tcl_version icons help_tips

    set w .
    wm focusmodel $w passive
    if {$geom != ""} {wm geometry $w $geom} else {wm geometry $w "700x500"}
    #wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm deiconify .
    wm protocol $w WM_DELETE_WINDOW "fileExit"

    wm title . "Scalars"


    #################################
    # menu
    #################################
    menu .menubar
    createMenubar .menubar
    . config -menu .menubar

    #################################
    # toolbar
    #################################
    frame .toolbar -relief raised -borderwidth 1
    foreach i {
      {sep0  -separator}
      {load  -image $icons(load)     -command fileLoad}
      {sep1  -separator}
      {copy  -image $icons(copy)     -command editCopy}
      {sep2  -separator}
      {chart -image $icons(barchart) -command chartCreateBarChart}
      {plot -image $icons(xyplot)    -command chartCreateScatterPlot}
    } {
      set b [eval iconbutton .toolbar.$i]
      pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
    }

    set help_tips(.toolbar.load)  {Load output scalar file}
    set help_tips(.toolbar.copy)  {Copy to clipboard (for spreadsheet)}
    set help_tips(.toolbar.chart) {Create bar chart}
    set help_tips(.toolbar.plot)  {Create scatter plot}


    ##########################################
    # status bar
    ##########################################
    frame .statusbar
    label .statusbar.label -anchor w -relief sunken -bd 1 -text {Ready} -justify left
    pack .statusbar.label -anchor w -expand 1 -fill x -padx 2 -pady 2 -side left

    set widget(status) .statusbar.label

    ##########################################
    # main window
    ##########################################
    frame .main -borderwidth 1 -height 30 -relief flat -width 30
    createMainArea .main

    pack .toolbar -anchor center -expand 0 -fill x -side top
    pack .main -expand 1 -fill both
    pack .statusbar -expand 0 -fill x -side bottom
}

proc busyCursor {{msg {}}} {
    global widget
    $widget(status) config -text $msg
    . config -cursor watch
    catch {.bltwin config -cursor watch}
    update idletasks
}

proc idleCursor {} {
    global widget
    $widget(status) config -text "Ready"
    update idletasks
    . config -cursor ""
    catch {.bltwin config -cursor ""}
}

#===================================================================
#    MAIN PROGRAM
#===================================================================

proc startScalars {argv} {
   global config
   global OMNETPP_SCALARS_DIR

   checkTclTkVersion
   setupTkOptions
   init_balloons
   createBltGraphPopup
   createMainWindow

   set origconfigfile $config(configfile)
   set defaultconfigfile [file join $OMNETPP_SCALARS_DIR .scalarsrc]
   if [file readable $defaultconfigfile] {
       loadConfig $defaultconfigfile
   }
   set config(configfile) $origconfigfile
   if [file readable $config(configfile)] {
       loadConfig $config(configfile)
   }

   foreach f $argv {
       loadScalarFile $f
   }

   wm deiconify .
}

