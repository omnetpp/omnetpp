#==========================================================================
#  MODINSP1.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
#  non-graphical module inspectors etc.
#

proc create_compoundmodinspector {name} {
    global icons help_tips

    set w $name
    create_inspector_toplevel $w

    iconbutton $w.toolbar.graph  -image $icons(asgraphics) -command "inspect_this $w {As Graphics}"
    iconbutton $w.toolbar.win    -image $icons(asoutput) -command "inspect_this $w {Module output}"
    iconbutton $w.toolbar.sep1   -separator
    iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
    iconbutton $w.toolbar.sep2   -separator
    iconbutton $w.toolbar.step   -image $icons(step) -command "one_step_in_module $w"
    foreach i {graph win sep1 parent sep2 step} {
       pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
    }

    set help_tips($w.toolbar.graph)   {Inspect as network graphics}
    set help_tips($w.toolbar.win)     {See module output}
    set help_tips($w.toolbar.parent)  {Inspect parent module}
    set help_tips($w.toolbar.step)    {Stop at events in this module}

    set nb $w.nb
    notebook $nb
    $nb config  -width 300 -height 200
    pack $nb -expand 1 -fill both

    notebook_addpage $nb info    {Info}
    notebook_addpage $nb submods {Submodules}
    notebook_addpage $nb params  {Params}
    notebook_addpage $nb gates   {Gates}

    # page 1: info
    label-sunkenlabel $nb.info.name {Module name:}
    label-sunkenlabel $nb.info.id {Module ID:}
    pack $nb.info.name -anchor center -fill x -side top
    pack $nb.info.id -anchor center -fill x -side top

    # other pages:
    create_inspector_listbox $nb.submods
    create_inspector_listbox $nb.params
    create_inspector_listbox $nb.gates
}

proc create_simplemodinspector {name} {
    global icons help_tips

    set w $name
    create_inspector_toplevel $w

    iconbutton $w.toolbar.win    -image $icons(asoutput) -command "inspect_this $w {Module output}"
    iconbutton $w.toolbar.sep1   -separator
    iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
    iconbutton $w.toolbar.sep2   -separator
    iconbutton $w.toolbar.step   -image $icons(step) -command "one_step_in_module $w"
    foreach i {win sep1 parent sep2 step} {
       pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
    }

    set help_tips($w.toolbar.win)    {See module output}
    set help_tips($w.toolbar.parent) {Inspect parent module}
    set help_tips($w.toolbar.step)   {Stop at events in this module}

    set nb $w.nb
    notebook $nb
    $nb config  -width 300 -height 200
    pack $nb -expand 1 -fill both

    notebook_addpage $nb info    {Info}
    notebook_addpage $nb params  {Params}
    notebook_addpage $nb gates   {Gates}
    notebook_addpage $nb vars    {Objects/Watches}
    notebook_addpage $nb paq     {Putaside Queue}
    notebook_addpage $nb submods {Submods}

    # page 1: info
    label-sunkenlabel $nb.info.name {Module name:}
    label-sunkenlabel $nb.info.id {Module ID:}
    label-sunkenlabel $nb.info.state {State:}
    label-sunkenlabel $nb.info.phase {Phase:}
    label-sunkenlabel $nb.info.stacksize {Stack size:}
    label-sunkenlabel $nb.info.stackused {Stack used:}
    pack $nb.info.name -anchor center -fill x -side top
    pack $nb.info.id -anchor center -fill x -side top
    pack $nb.info.state -anchor center  -fill x -side top
    pack $nb.info.phase -anchor center  -fill x -side top
    pack $nb.info.stacksize -anchor center  -fill x -side top
    pack $nb.info.stackused -anchor center  -fill x -side top

    # other pages:
    create_inspector_listbox $nb.params
    create_inspector_listbox $nb.gates
    create_inspector_listbox $nb.vars
    create_inspector_listbox $nb.paq
    create_inspector_listbox $nb.submods
}

proc one_step_in_module {w} {
    # implements modulewindow's Step icon

    if [check_running] return

    if {[network_ready] == 1} {
        #display_stopdialog normal
        opp_onestepinmodule $w
        #remove_stopdialog
    }
}


proc create_simplemodulewindow {name} {
    _create_modulewindow $name 0
}

proc create_compoundmodulewindow {name} {
    _create_modulewindow $name 1
}

proc _create_modulewindow {name iscompound} {
    global icons help_tips

    set w $name
    create_inspector_toplevel $w

    # Add icons
    if {$iscompound} {
        # for compound module
        iconbutton $w.toolbar.obj    -image $icons(asobject) -command "inspect_this $w {As Object}"
        iconbutton $w.toolbar.graph  -image $icons(asgraphics) -command "inspect_this $w {As Graphics}"
        iconbutton $w.toolbar.sep1   -separator
        iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
        iconbutton $w.toolbar.sep2   -separator
        iconbutton $w.toolbar.step   -image $icons(step) -command "one_step_in_module $w"

        foreach i {obj graph sep1 parent sep2 step} {
           pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
        }

        set help_tips($w.toolbar.obj)    {Inspect as object}
        set help_tips($w.toolbar.parent) {Inspect parent module}
        set help_tips($w.toolbar.step)   {Stop at events in this module}
    } else {
        # for simple module
        iconbutton $w.toolbar.obj    -image $icons(asobject) -command "inspect_this $w {As Object}"
        iconbutton $w.toolbar.sep1   -separator
        iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
        iconbutton $w.toolbar.sep2   -separator
        iconbutton $w.toolbar.step   -image $icons(step) -command "one_step_in_module $w"

        foreach i {obj sep1 parent sep2 step} {
           pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
        }

        set help_tips($w.toolbar.obj)    {Inspect as object}
        set help_tips($w.toolbar.parent) {Inspect parent module}
        set help_tips($w.toolbar.step)   {Stop at events in this module}
    }

    frame $w.main
    text $w.main.text -yscrollcommand "$w.main.sb set" -width 80 -height 15
    scrollbar $w.main.sb -command "$w.main.text yview"
    $w.main.text tag configure event -foreground blue
    $w.main.text tag configure SELECT -back #808080 -fore #ffffff

    pack $w.main -anchor center -expand 1 -fill both -side top
    pack $w.main.sb -anchor center -expand 0 -fill y -side right
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    # bindings for find
    #   'break' is needed below because
    #      ^F is originally bound to 1 char right
    #      ^N is originally bound to 1 line down
    set txt $w.main.text
    bind $txt <Control-f> "findDialog $txt;break"
    bind $txt <Control-F> "findDialog $txt;break"
    bind $txt <Control-n> "findNext $txt;break"
    bind $txt <Control-N> "findNext $txt;break"
    bind $txt <F3> "findNext $txt"
}


