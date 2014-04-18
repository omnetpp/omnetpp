#=================================================================
#  LOGINSPECTOR.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2008 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc createLogInspector {insp geom} {
    global icons help_tips B2 B3

    createInspectorToplevel $insp $geom
    set help_tips($insp.toolbar.parent)  "Go to parent module"

    packToolbutton $insp.toolbar.sep1 -separator
    textWindowAddIcons $insp modulewindow
    LogInspector:addModeButtons $insp $insp.toolbar
    ModuleInspector:addRunButtons $insp

    ttk::frame $insp.main
    pack $insp.main -expand 1 -fill both -side top
    createLogViewer $insp $insp.main

    inspector:bindSideButtons $insp
}

proc createEmbeddedLogInspector {insp} {
    global icons help_tips CTRL_

    ttk::frame $insp.main
    pack $insp.main -expand 1 -fill both -side top

    createLogViewer $insp $insp.main

    set tb [inspector:createInternalToolbar $insp $insp.main.text]
    packToolbutton $tb.copy   -image $icons(copy) -command "editCopy $insp.main.text"
    packToolbutton $tb.find   -image $icons(find) -command "findDialog $insp.main.text"
    packToolbutton $tb.filter -image $icons(filter) -command "editFilterWindowContents $insp"

    set help_tips($tb.copy)   "Copy selected text to clipboard (${CTRL_}C)"
    set help_tips($tb.find)   "Find string in window (${CTRL_}F)"
    set help_tips($tb.filter) "Filter window contents (${CTRL_}H)"

    LogInspector:addModeButtons $insp $tb

    # note: intentionally no bindSideButtons (embedded log inspector is tied to the canvas,
    # and cannot be navigated independently)
}

proc createLogViewer {insp f} {
    global config B3 Control

    panedwindow $f.ruler -orient horizontal -sashrelief raised
    label $f.ruler.l1 -text "Event#" -anchor w
    label $f.ruler.l2 -text "Time" -anchor w
    label $f.ruler.l3 -text "Src/Dest" -anchor w
    label $f.ruler.l4 -text "Info" -anchor w
    $f.ruler add $f.ruler.l1 $f.ruler.l2 $f.ruler.l3 $f.ruler.l4
    $f.ruler paneconfigure $f.ruler.l1 -minsize 60
    $f.ruler paneconfigure $f.ruler.l2 -minsize 150
    $f.ruler paneconfigure $f.ruler.l3 -minsize 150
    $f.ruler paneconfigure $f.ruler.l4 -minsize 200

    text $f.text -yscrollcommand "$f.sb set" -width 80 -height 15 -font LogFont
    ttk::scrollbar $f.sb -command "$f.text yview"
    LogInspector:configureTags $insp
    LogInspector:updateTabStops $insp

    pack $f.ruler -anchor center -expand 0 -fill x -side top
    pack $f.sb -anchor center -expand 0 -fill y -side right
    pack $f.text -anchor center -expand 1 -fill both -side left

    # bindings for the ruler
    bind $f.ruler <ButtonRelease-1> "LogInspector:updateTabStops $insp"

    # bindings for find
    bindCommandsToTextWidget $f.text

    # bind Ctrl+H to the whole (main or inspector) window
    # ('break' is needed because originally ^H is bound to DEL)
    set w [winfo toplevel $f]
    bind $w <$Control-h> "LogInspector:openFilterDialog $insp; break"
    bind $w <$Control-H> "LogInspector:openFilterDialog $insp; break"

    # bind a context menu as well
    catch {$f.text config -wrap $config(editor-wrap)}
    bind $f.text <Button-$B3> [list textwidget:contextMenu %W $insp %X %Y]
}

proc LogInspector:addModeButtons {insp tb} {
    global icons help_tips
    packToolbutton $tb.msgs   -image $icons(pkheader) -command "LogInspector:setMode $insp messages"
    packToolbutton $tb.log    -image $icons(log) -command "LogInspector:setMode $insp log"
    set help_tips($tb.msgs)   "Show message/packet traffic"
    set help_tips($tb.log)    "Show module log"
    after idle "LogInspector:refreshModeButtons $insp"
}

proc LogInspector:setMode {insp mode} {
    opp_inspectorcommand $insp setmode $mode
    LogInspector:refreshModeButtons $insp

    set txt $insp.main.text
    if {$mode=="messages"} {
        LogInspector:updateTabStops $insp
    } else {
        $txt config -tabs {}
    }
}

proc LogInspector:refreshModeButtons {insp} {
    set mode [opp_inspectorcommand $insp getmode]
    set tb $insp.toolbar
    toolbutton:setsunken $tb.msgs [expr {$mode=="messages"}]
    toolbutton:setsunken $tb.log [expr {$mode=="log"}]
}

proc LogInspector:configureTags {insp} {
    set txt $insp.main.text

    $txt tag configure SELECT -back #808080 -fore #ffffff
    $txt tag configure event -foreground blue
    $txt tag configure log -foreground #006000
    $txt tag configure prefix -foreground #909090
    $txt tag configure warning -foreground #ff0000

    $txt tag configure eventnumcol -foreground #808080
    $txt tag configure timecol -foreground #808080
    $txt tag configure srcdestcol -foreground #909000
    $txt tag configure msginfocol -foreground black
}

proc LogInspector:updateTabStops {insp} {
    # read sash positions from ruler
    set ruler $insp.main.ruler
    set stops {}
    set n [llength [$ruler panes]]
    for {set i 0} {$i < $n-1} {incr i} {
        lappend stops [lindex [$ruler sash coord $i] 0]
    }

    # set them on the text widget as tab stops
    set txt $insp.main.text
    $txt config -tabs $stops
}

proc LogInspector:clear {insp} {
    # Note: the direct way ($txt delete 0.1 end) is very slow if there's
    # a lot of lines (100,000). Luckily, removing all tags beforehand
    # speeds up the whole process about a hundred times. We need to
    # re-define the tags afterwards though.

    set txt $insp.main.text
    $txt tag delete log event prefix
    $txt delete 0.1 end
    LogInspector:configureTags $insp
}

proc LogInspector:openFilterDialog {insp} {
    set modptr [opp_inspector_getobject $insp]
    set excludedModuleIds [opp_inspectorcommand $insp getexcludedmoduleids]
    set excludedModuleIds [moduleOutputFilterDialog $modptr $excludedModuleIds]
    if {$excludedModuleIds!="0"} {
        opp_inspectorcommand $insp setexcludedmoduleids $excludedModuleIds
        opp_inspectorcommand $insp redisplay
    }
}

proc LogInspector:trimlines {insp} {
    global config
    set t $insp.main.text
    set numlines $config(logwindow-scrollbacklines)

    if {$numlines==""} {return}
    set endline [$t index {end linestart}]
    if {$endline > $numlines + 100} {  ;# for performance, we want to delete in at least 100-line chunks
        set linestodelete [expr int($endline-$numlines)]
        $t delete 1.0 $linestodelete.0
        $t insert 1.0 "\[Lines have been discarded from here due to the scrollback limit, see Preferences\]\n" warning
    }

}


