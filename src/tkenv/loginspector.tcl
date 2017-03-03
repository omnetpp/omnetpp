#=================================================================
#  LOGINSPECTOR.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2017 Andras Varga
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
    global config B3 Control config inspectordata arrowcursor

    createRuler $f.ruler
    pack $f.ruler -fill x

    # create text widget
    ttk::frame $f.grid
    pack $f.grid -expand yes -fill both -padx 1 -pady 1
    text $f.text -width 80 -height 15 -font LogFont -cursor $arrowcursor -bg white ;# on some Linux versions the bg is grey by default
    addScrollbars $f.text $f.grid
    $f.text config -xscrollcommand "ruler:xscroll $f.ruler $f.text; [$f.text cget -xscrollcommand]"

    set inspectordata($insp:showprefix) $config(editor-showprefix)

    LogInspector:configureTags $insp
    LogInspector:updateTabStops $insp

    # bindings for the ruler
    bind $f.ruler <<Changed>> "LogInspector:updateTabStops $insp"

    # bindings for find
    bindCommandsToTextWidget $f.text

    # bind Ctrl+H to the whole (main or inspector) window
    # ('break' is needed because originally ^H is bound to DEL)
    set w [winfo toplevel $f]
    bind $w <$Control-h> "LogInspector:openFilterDialog $insp; break"
    bind $w <$Control-H> "LogInspector:openFilterDialog $insp; break"

    # let the widget generate <<CursorMove>> events
    addCursorMoveEvent $f.text
    bind $f.text <<CursorMove>> "LogInspector:onCursorMove $insp"

    highlightcurrentline $f.text
    makereadonly $f.text
    $f.text config -insertwidth 0

    # bind global shortcuts to this widget (otherwise makereadonly's bindings would mask them)
    bindRunCommands $f.text

    # bind a context menu as well
    catch {$f.text config -wrap $config(editor-wrap)}
    bind $f.text <Button-$B3> [list LogInspector:contextMenu $insp %X %Y]

    after idle "LogInspector:setMode $insp $config(new-loginspector-mode)"
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
    global config

    opp_inspectorcommand $insp setmode $mode
    LogInspector:refreshModeButtons $insp

    set ruler $insp.main.ruler
    set txt $insp.main.text
    if {$mode=="messages"} {
        ruler:setColumnWidths $ruler {60 180 180 180 300}
        ruler:setColumnTitles $ruler {Event# Time Src/Dest Name Info}
    } else {
        ruler:setColumnTitles $ruler {}
        ruler:setColumnWidths $ruler {}
    }
    LogInspector:updateTabStops $insp
    set config(new-loginspector-mode) $mode
}

proc LogInspector:getMode {insp} {
    return [opp_inspectorcommand $insp getmode]
}

proc LogInspector:refreshModeButtons {insp} {
    set mode [opp_inspectorcommand $insp getmode]
    set tb $insp.toolbar
    toolbutton:setsunken $tb.msgs [expr {$mode=="messages"}]
    toolbutton:setsunken $tb.log [expr {$mode=="log"}]
}

proc LogInspector:configureTags {insp} {
    global inspectordata

    set txt $insp.main.text

    $txt tag configure SELECT -back #808080 -fore #ffffff
    $txt tag configure linehighlight -back #e0e0e0
    $txt tag configure hyperlink -fore #0000ff -underline 1

    $txt tag configure banner -foreground #0000f0
    $txt tag configure auxbanner -foreground #7070ff
    $txt tag configure info -foreground #006000
    $txt tag configure prefix -foreground #909090
    $txt tag configure warning -foreground #ff0000

    $txt tag configure eventnumcol -foreground #606060
    $txt tag configure repeatedeventnumcol -foreground #c0c0c0
    $txt tag configure timecol -foreground #606060
    $txt tag configure repeatedtimecol -foreground #c0c0c0
    $txt tag configure srcdestcol -foreground #808000
    $txt tag configure namecol -foreground #008000
    $txt tag configure msginfocol -foreground black

    $txt tag raise hyperlink
    $txt tag raise SELECT
    $txt tag raise sel

    $txt tag configure prefix -elide [expr {$inspectordata($insp:showprefix)=="0"}]
}

proc LogInspector:updateTabStops {insp} {
    set tabstops [ruler:getTabstops $insp.main.ruler]
    $insp.main.text config -tabs $tabstops
}

proc LogInspector:onCursorMove {insp} {
    # search for previous msghop bookmark, and set that msg as selection
    set txt $insp.main.text
    set mark "insert lineend" ;# lineend is important for "cursor at line start" case
    while {$mark!="" && [string first "msghop#" $mark]!=0} {
        set mark [$txt mark previous $mark]
    }
    if {$mark!=""} {
        debug "found msghop mark: $mark"
        regexp {^msghop#(\d+):(\d+)$} $mark match eventnum msgsendindex
        set msgptr [opp_inspectorcommand $insp gethopmsg $eventnum $msgsendindex]
        mainWindow:selectionChanged $insp $msgptr  ;# even if it's null!
    }
}

proc LogInspector:clear {insp} {
    # Note: the direct way ($txt delete 0.1 end) is very slow if there's
    # a lot of lines (100,000). Luckily, removing all tags beforehand
    # speeds up the whole process about a hundred times. We need to
    # re-define the tags afterwards though.

    set txt $insp.main.text
    $txt tag delete {*}[$txt tag names]
    $txt mark unset {*}[$txt mark names]
    $txt delete 0.1 end
    LogInspector:configureTags $insp
}

proc LogInspector:contextMenu {insp X Y} {
    # note: this code has some overlap with textwidget:contextMenu
    global tmp config CTRL inspectordata

    set txt $insp.main.text

    set tmp(wrap) [$txt cget -wrap]

    catch {destroy .popup}
    menu .popup -tearoff 0

    .popup add command -command editCopy -label "Copy" -accel "$CTRL+C" -underline 0
    .popup add separator
    .popup add command -command "editFind $txt" -label "Find..." -accel "$CTRL+F" -underline 0
    .popup add command -command "editFindNext $txt" -label "Find Next" -accel "F3" -underline 5
    .popup add separator
    .popup add command -command "LogInspector:openFilterDialog $insp" -label "Filter Window Contents..." -accel "$CTRL+H" -underline 0
    .popup add separator
    .popup add checkbutton -command "LogInspector:refreshPrefix $insp" -variable inspectordata($insp:showprefix) -onvalue 1 -offvalue 0 -label "Show Log Prefix" -underline 0
    .popup add command -label "Logging Options..." -command "preferencesDialog $txt g"
    .popup add checkbutton -command "textwidget:toggleWrap $txt" -variable tmp(wrap) -onvalue "char" -offvalue "none" -label "Wrap Lines" -underline 0
    .popup add separator
    .popup add command -command "$txt tag add sel 1.0 end" -label "Select All" -accel "$CTRL+A" -underline 0

    tk_popup .popup $X $Y
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

proc LogInspector:refreshPrefix {insp} {
    global config inspectordata

    set txt $insp.main.text
    $txt tag configure prefix -elide [expr {$inspectordata($insp:showprefix)=="0"}]

    # set default for further windows
    set config(editor-showprefix) $inspectordata($insp:showprefix)
}

proc LogInspector:trimlines {insp} {
    global config
    set txt $insp.main.text
    set numlines [opp_getsimoption scrollbacklimit]

    if {$numlines==""} {return}
    set endline [$txt index {end linestart}]
    if {$endline > $numlines + 1000} {  ;# for performance, we want to delete in at least 1000-line chunks
        set linestodelete [expr int($endline-$numlines)]
        # we need to remove tags and marks from the range to be deleted,
        # otherwise "$txt delete" takes ages (really!)
        foreach tag [$txt tag names] {
            $txt tag remove $tag 1.0 $linestodelete.0
        }
        set mark [$txt mark previous $linestodelete.0]
        while {$mark!=""} {
            set prev [$txt mark previous $mark]
            $txt mark unset $mark
            set mark $prev
        }
        $txt delete 1.0 $linestodelete.0
        $txt insert 1.0 "\[Lines have been discarded from here due to the scrollback limit, see Preferences\]\n" warning
    }
}

proc LogInspector:dump {insp label} {
    set txt $insp.main.text
    puts $label
    foreach {type value pos} [$txt dump -all 1.0 end] {
        set value [string map {"\n" {\n} "\t" {\t}} $value]
        if {$type=="text"} {set value "\"$value\""}
        puts "$type\t$pos\t$value"
    }
}

