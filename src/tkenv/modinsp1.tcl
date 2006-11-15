#=================================================================
#  MODINSP1.TCL - part of
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

#
#  non-graphical module inspectors etc.
#

proc moduleinspector_add_run_buttons {w} {
    global icons help_tips

    pack_iconbutton $w.toolbar.objs    -image $icons(findobj) -command "inspect_filteredobjectlist $w"
    pack_iconbutton $w.toolbar.sep11   -separator

    pack_iconbutton $w.toolbar.mrun    -image $icons(mrun)    -command "runsimulation_local $w normal"
    pack_iconbutton $w.toolbar.mfast   -image $icons(mfast)   -command "runsimulation_local $w fast"
    pack_iconbutton $w.toolbar.sep13   -separator

    pack_iconbutton $w.toolbar.vrun     -image $icons(run)     -command {run_normal}
    pack_iconbutton $w.toolbar.vruncfg  -image $icons(down_vs) -command "moduleinspector_setrunmode $w.toolbar.vrun"

    #pack_iconbutton $w.toolbar.fastrun -image $icons(fast)    -command {run_fast}
    #pack_iconbutton $w.toolbar.exprrun -image $icons(express) -command {run_express}
    #pack_iconbutton $w.toolbar.sep12   -separator
    #pack_iconbutton $w.toolbar.until   -image $icons(until)   -command {run_until}
    pack_iconbutton $w.toolbar.sep14   -separator

    pack_iconbutton $w.toolbar.stop    -image $icons(stop)    -command {stop_simulation}
    pack_iconbutton $w.toolbar.sep16   -separator

    bind $w <Control-F4> "runsimulation_local $w fast"

    set help_tips($w.toolbar.objs)    {Find and inspect messages, queues, watched variables, statistics, etc (Ctrl+S)}
    set help_tips($w.toolbar.vrun)    {Run with full animation (F5)}
    set help_tips($w.toolbar.mrun)    {Run until next event in this module}
    set help_tips($w.toolbar.mfast)   {Fast run until next event in this module (Ctrl+F4)}
    set help_tips($w.toolbar.stop)    {Stop the simulation (F8)}
}

#
# Invoked by the small down arrow next to the "Run" icon on the toolbar, and displays
# a menu to select between Run, Fast, Express and Until. $w is the icon button to configure.
#
proc moduleinspector_setrunmode {w} {
    global icons help_tips

    catch {destroy .popup}
    menu .popup -tearoff 0

    .popup add command -label "Run" \
        -command [list config_iconbutton $w $icons(run)  run_normal "Run with full animation (F5)"]
    .popup add command -label "Fast Run" \
        -command [list config_iconbutton $w $icons(fast) run_fast "Run faster: no animation and rare inspector updates (F6)"]
    .popup add command -label "Express Run" \
        -command [list config_iconbutton $w $icons(express) run_express "Run at full speed: no text output, animation or inspector updates (F7)"]
    .popup add command -label "Until..."  \
        -command [list config_iconbutton $w $icons(until) run_until "Run until time or event number"]

    .popup post [winfo rootx $w] [expr [winfo rooty $w]+[winfo height $w]]

}

#proc create_compoundmodinspector {name geom} {
#    global icons help_tips
#
#    set w $name
#    create_inspector_toplevel $w $geom
#
#    pack_iconbutton $w.toolbar.graph   -image $icons(asgraphics) -command "inspect_this $w {As Graphics}"
#    pack_iconbutton $w.toolbar.win     -image $icons(asoutput) -command "inspect_this $w {Module output}"
#    pack_iconbutton $w.toolbar.sep1    -separator
#
#    moduleinspector_add_run_buttons $w
#
#    rpack_iconbutton $w.toolbar.apply  -image $icons(apply) -command "opp_writebackinspector $w; opp_updateinspectors"
#    rpack_iconbutton $w.toolbar.revert -image $icons(revert) -command "opp_updateinspectors"
#
#    set help_tips($w.toolbar.owner)   {Inspect parent module}
#    set help_tips($w.toolbar.graph)   {Inspect as network graphics}
#    set help_tips($w.toolbar.win)     {See module output}
#    set help_tips($w.toolbar.apply)   {Apply changes (Enter)}
#    set help_tips($w.toolbar.revert)  {Revert}
#
#    set nb [inspector_createnotebook $w]
#
#    notebook_addpage $nb info    {Info}
#    notebook_addpage $nb contents {Contents}
#
#    notebook_showpage $nb contents
#
#    # page 1: info
#    label-entry $nb.info.name {Module name:}
#    label-sunkenlabel $nb.info.id {Module ID:}
#    label-entry $nb.info.dispstr {Display string:}
#    label-entry $nb.info.dispstrpt {Background disp.str:}
#    pack $nb.info.name -anchor center -fill x -side top
#    pack $nb.info.id -anchor center -fill x -side top
#    pack $nb.info.dispstr -anchor center -fill x -side top
#    pack $nb.info.dispstrpt -anchor center -fill x -side top
#
#    # other pages:
#    create_inspector_listbox $nb.contents
#
#    # XXX experimental page
#    inspector_createfields2page $w
#}
#
#proc create_simplemodinspector {name geom} {
#    global icons help_tips
#
#    set w $name
#    create_inspector_toplevel $w $geom
#
#    pack_iconbutton $w.toolbar.win    -image $icons(asoutput) -command "inspect_this $w {Module output}"
#    pack_iconbutton $w.toolbar.sep1   -separator
#
#    moduleinspector_add_run_buttons $w
#
#    rpack_iconbutton $w.toolbar.apply  -image $icons(apply) -command "opp_writebackinspector $w; opp_updateinspectors"
#    rpack_iconbutton $w.toolbar.revert -image $icons(revert) -command "opp_updateinspectors"
#
#    set help_tips($w.toolbar.owner)   {Inspect parent module}
#    set help_tips($w.toolbar.win)     {See module output}
#    set help_tips($w.toolbar.apply)   {Apply changes (Enter)}
#    set help_tips($w.toolbar.revert)  {Revert}
#
#    bind $w <Control-F4> "runsimulation_local $w fast"
#
#    set nb [inspector_createnotebook $w]
#
#    notebook_addpage $nb info    {Info}
#    notebook_addpage $nb contents {Contents}
#
#    notebook_showpage $nb contents
#
#    # page 1: info
#    label-entry $nb.info.name {Module name:}
#    label-sunkenlabel $nb.info.id {Module ID:}
#    #label-sunkenlabel $nb.info.phase {Phase:}
#    label-entry $nb.info.dispstr {Display string:}
#    label-entry $nb.info.dispstrpt {Background disp.str:}
#    label-sunkenlabel $nb.info.state {State:}
#    label-sunkenlabel $nb.info.stacksize {Stack size:}
#    label-sunkenlabel $nb.info.stackused {Stack used:}
#    pack $nb.info.name -anchor center -fill x -side top
#    pack $nb.info.id -anchor center -fill x -side top
#    pack $nb.info.state -anchor center  -fill x -side top
#    pack $nb.info.dispstr -anchor center -fill x -side top
#    pack $nb.info.dispstrpt -anchor center -fill x -side top
#    #pack $nb.info.phase -anchor center  -fill x -side top
#    pack $nb.info.stacksize -anchor center  -fill x -side top
#    pack $nb.info.stackused -anchor center  -fill x -side top
#
#    # other pages:
#    create_inspector_listbox $nb.contents
#
#    # XXX experimental page
#    inspector_createfields2page $w
#}

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
        pack_iconbutton $w.toolbar.graph  -image $icons(asgraphics) -command "inspect_this $w {As Graphics}"
        pack_iconbutton $w.toolbar.obj    -image $icons(asobject) -command "inspect_this $w {As Object}"
        pack_iconbutton $w.toolbar.sep1   -separator
        set help_tips($w.toolbar.owner)  {Inspect parent module}
        set help_tips($w.toolbar.graph)  {Inspect as network graphics}
        set help_tips($w.toolbar.obj)    {Inspect as object}

        textwindow_add_icons $w

        moduleinspector_add_run_buttons $w

    } else {
        # for simple module
        pack_iconbutton $w.toolbar.obj    -image $icons(asobject) -command "inspect_this $w {As Object}"
        pack_iconbutton $w.toolbar.sep1   -separator
        set help_tips($w.toolbar.owner)  {Inspect parent module}
        set help_tips($w.toolbar.obj)    {Inspect as object}

        textwindow_add_icons $w

        moduleinspector_add_run_buttons $w
    }

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

proc mainlogwindow_trimlines {} {
    global config
    text_trimlines .main.text $config(logwindow-scrollbacklines)
}

proc modulewindow_trimlines {w} {
    global config
    text_trimlines $w.main.text $config(logwindow-scrollbacklines)
}

proc text_trimlines {t numlines} {
    if {$numlines==""} {return}
    set endline [$t index {end linestart}]
    if {$endline > $numlines + 100} {  ;# for performance, we want to delete in at least 100-line chunks
        set linestodelete [expr int($endline-$numlines)]
        $t delete 1.0 $linestodelete.0
    }
}
