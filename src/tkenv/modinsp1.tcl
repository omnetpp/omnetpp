#==========================================================================
#  MODINSP1.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2004 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
#  non-graphical module inspectors etc.
#

proc create_compoundmodinspector {name geom} {
    global icons help_tips

    set w $name
    create_inspector_toplevel $w $geom

    iconbutton $w.toolbar.graph  -image $icons(asgraphics) -command "inspect_this $w {As Graphics}"
    iconbutton $w.toolbar.win    -image $icons(asoutput) -command "inspect_this $w {Module output}"
    iconbutton $w.toolbar.sep1   -separator
    #iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
    #iconbutton $w.toolbar.sep2   -separator
    iconbutton $w.toolbar.mrun   -image $icons(mrun) -command "module_run $w"
    iconbutton $w.toolbar.mfast  -image $icons(mfast) -command "module_run_fast $w"
    iconbutton $w.toolbar.stop   -image $icons(stop) -command "stop_simulation"
    foreach i {graph win sep1 mrun mfast stop} {
       pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
    }

    iconbutton $w.toolbar.apply  -image $icons(apply) -command "opp_writebackinspector $w; opp_updateinspectors"
    iconbutton $w.toolbar.revert -image $icons(revert) -command "opp_updateinspectors"
    pack $w.toolbar.revert -anchor n -side right -padx 0 -pady 2
    pack $w.toolbar.apply -anchor n -side right -padx 0 -pady 2

    bind $w <Control-F4> "module_run_fast $w"

    set help_tips($w.toolbar.owner)   {Inspect parent module}
    set help_tips($w.toolbar.graph)   {Inspect as network graphics}
    set help_tips($w.toolbar.win)     {See module output}
    set help_tips($w.toolbar.mrun)    {Run until next event in this module}
    set help_tips($w.toolbar.mfast)   {Fast run until next event in this module (Ctrl-F4)}
    set help_tips($w.toolbar.stop)    {Stop running simulation (F8)}
    set help_tips($w.toolbar.apply)   {Apply changes (Enter)}
    set help_tips($w.toolbar.revert)  {Revert}

    set nb $w.nb
    notebook $nb
    $nb config -width 460 -height 260
    pack $nb -expand 1 -fill both

    notebook_addpage $nb info    {Info}
    notebook_addpage $nb submods {Submodules}
    notebook_addpage $nb params  {Params}
    notebook_addpage $nb gates   {Gates}

    # page 1: info
    label-entry $nb.info.name {Module name:}
    label-sunkenlabel $nb.info.id {Module ID:}
    label-entry $nb.info.dispstr {Display string:}
    label-entry $nb.info.dispstrpt {Background disp.str:}
    pack $nb.info.name -anchor center -fill x -side top
    pack $nb.info.id -anchor center -fill x -side top
    pack $nb.info.dispstr -anchor center -fill x -side top
    pack $nb.info.dispstrpt -anchor center -fill x -side top

    # other pages:
    create_inspector_listbox $nb.submods
    create_inspector_listbox $nb.params
    create_inspector_listbox $nb.gates
}

proc create_simplemodinspector {name geom} {
    global icons help_tips

    set w $name
    create_inspector_toplevel $w $geom

    iconbutton $w.toolbar.win    -image $icons(asoutput) -command "inspect_this $w {Module output}"
    iconbutton $w.toolbar.sep1   -separator
    #iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
    #iconbutton $w.toolbar.sep2   -separator
    iconbutton $w.toolbar.mrun   -image $icons(mrun) -command "runsimulation_local $w normal"
    iconbutton $w.toolbar.mfast  -image $icons(mfast) -command "runsimulation_local $w fast"
    iconbutton $w.toolbar.stop   -image $icons(stop) -command "stop_simulation"
    foreach i {win sep1 mrun mfast stop} {
       pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
    }
    iconbutton $w.toolbar.apply  -image $icons(apply) -command "opp_writebackinspector $w; opp_updateinspectors"
    iconbutton $w.toolbar.revert -image $icons(revert) -command "opp_updateinspectors"
    pack $w.toolbar.revert -anchor n -side right -padx 0 -pady 2
    pack $w.toolbar.apply -anchor n -side right -padx 0 -pady 2

    set help_tips($w.toolbar.owner)   {Inspect parent module}
    set help_tips($w.toolbar.win)     {See module output}
    set help_tips($w.toolbar.mrun)    {Run until next event in this module}
    set help_tips($w.toolbar.mfast)   {Fast run until next event in this module (Ctrl-F4)}
    set help_tips($w.toolbar.stop)    {Stop running simulation (F8)}
    set help_tips($w.toolbar.apply)   {Apply changes (Enter)}
    set help_tips($w.toolbar.revert)  {Revert}

    bind $w <Control-F4> "runsimulation_local $w fast"

    set nb $w.nb
    notebook $nb
    $nb config -width 460 -height 260
    pack $nb -expand 1 -fill both

    notebook_addpage $nb info    {Info}
    notebook_addpage $nb params  {Params}
    notebook_addpage $nb gates   {Gates}
    notebook_addpage $nb vars    {Contents}
    notebook_addpage $nb submods {Submodules}

    # page 1: info
    label-entry $nb.info.name {Module name:}
    label-sunkenlabel $nb.info.id {Module ID:}
    #label-sunkenlabel $nb.info.phase {Phase:}
    label-entry $nb.info.dispstr {Display string:}
    label-entry $nb.info.dispstrpt {Background disp.str:}
    label-sunkenlabel $nb.info.state {State:}
    label-sunkenlabel $nb.info.stacksize {Stack size:}
    label-sunkenlabel $nb.info.stackused {Stack used:}
    pack $nb.info.name -anchor center -fill x -side top
    pack $nb.info.id -anchor center -fill x -side top
    pack $nb.info.state -anchor center  -fill x -side top
    pack $nb.info.dispstr -anchor center -fill x -side top
    pack $nb.info.dispstrpt -anchor center -fill x -side top
    #pack $nb.info.phase -anchor center  -fill x -side top
    pack $nb.info.stacksize -anchor center  -fill x -side top
    pack $nb.info.stackused -anchor center  -fill x -side top

    # other pages:
    create_inspector_listbox $nb.params
    create_inspector_listbox $nb.gates
    create_inspector_listbox $nb.vars
    create_inspector_listbox $nb.submods
}

proc runsimulation_local {w mode} {
    # invoked from toolbar in module inspectors
    if [is_running] {
        set_gui_for_runmode $mode $w
        opp_set_run_mode $mode
        opp_set_run_until_module $w
    } else {
        if {![network_ready]} {return}
        set_gui_for_runmode $mode $w
        opp_onestepinmodule $w $mode
        set_gui_for_runmode notrunning
    }
}

proc create_simplemodulewindow {name geom} {
    _create_modulewindow $name $geom 0
}

proc create_compoundmodulewindow {name geom} {
    _create_modulewindow $name $geom 1
}

proc _create_modulewindow {name geom iscompound} {
    global icons help_tips

    set w $name
    create_inspector_toplevel $w $geom

    # Add icons
    if {$iscompound} {
        # for compound module
        iconbutton $w.toolbar.graph  -image $icons(asgraphics) -command "inspect_this $w {As Graphics}"
        iconbutton $w.toolbar.obj    -image $icons(asobject) -command "inspect_this $w {As Object}"
        iconbutton $w.toolbar.sep1   -separator
        #iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
        #iconbutton $w.toolbar.sep2   -separator
        iconbutton $w.toolbar.mrun   -image $icons(mrun) -command "runsimulation_local $w normal"
        iconbutton $w.toolbar.mfast  -image $icons(mfast) -command "runsimulation_local $w fast"
        iconbutton $w.toolbar.stop   -image $icons(stop) -command "stop_simulation"
        iconbutton $w.toolbar.sep3   -separator
        iconbutton $w.toolbar.find   -image $icons(find) -command "findDialog $w.main.text"
        foreach i {obj graph sep1 mrun mfast stop sep3 find} {
           pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
        }
        bind $w <Control-F4> "runsimulation_local $w fast"

        set help_tips($w.toolbar.owner)  {Inspect parent module}
        set help_tips($w.toolbar.graph)  {Inspect as network graphics}
        set help_tips($w.toolbar.obj)    {Inspect as object}
        set help_tips($w.toolbar.mrun)   {Run until next event in this module}
        set help_tips($w.toolbar.mfast)  {Fast run until next event in this module (Ctrl-F4)}
        set help_tips($w.toolbar.stop)   {Stop running simulation (F8)}
        set help_tips($w.toolbar.find)   {Find string in window}
    } else {
        # for simple module
        iconbutton $w.toolbar.obj    -image $icons(asobject) -command "inspect_this $w {As Object}"
        iconbutton $w.toolbar.sep1   -separator
        #iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
        #iconbutton $w.toolbar.sep2   -separator
        iconbutton $w.toolbar.mrun   -image $icons(mrun) -command "runsimulation_local $w normal"
        iconbutton $w.toolbar.mfast  -image $icons(mfast) -command "runsimulation_local $w fast"
        iconbutton $w.toolbar.stop   -image $icons(stop) -command "stop_simulation"
        iconbutton $w.toolbar.sep3   -separator
        iconbutton $w.toolbar.find   -image $icons(find) -command "findDialog $w.main.text"
        foreach i {obj sep1 mrun mfast stop sep3 find} {
           pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
        }
        bind $w <Control-F4> "runsimulation_local $w fast"

        set help_tips($w.toolbar.owner)  {Inspect parent module}
        set help_tips($w.toolbar.obj)    {Inspect as object}
        set help_tips($w.toolbar.mrun)   {Run until next event in this module}
        set help_tips($w.toolbar.mfast)  {Fast run until next event in this module (Ctrl-F4)}
        set help_tips($w.toolbar.stop)   {Stop running simulation (F8)}
        set help_tips($w.toolbar.find)   {Find string in window}
    }

    #frame $w.statusbar
    #label $w.statusbar.name -anchor w -relief groove -text {(unknown module)}
    #label $w.statusbar.phase -anchor w -relief groove -text {Phase: -}

    frame $w.main
    text $w.main.text -yscrollcommand "$w.main.sb set" -width 80 -height 15
    scrollbar $w.main.sb -command "$w.main.text yview"
    $w.main.text tag configure event -foreground blue
    $w.main.text tag configure SELECT -back #808080 -fore #ffffff

    #pack $w.statusbar -anchor center -expand 0 -fill x -side top
    #pack $w.statusbar.name -anchor n -expand 1 -fill x -side left
    #pack $w.statusbar.phase   -anchor n -expand 1 -fill x -side right
    pack $w.main -anchor center -expand 1 -fill both -side top
    pack $w.main.sb -anchor center -expand 0 -fill y -side right
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    # bindings for find
    bind_findcommands_to_textwidget $w.main.text
}


