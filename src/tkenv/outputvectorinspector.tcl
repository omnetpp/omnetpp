#=================================================================
#  OUTPUTVECTORINSPECTOR.TCL - part of
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


proc createOutputVectorInspector {insp geom} {
    createInspectorToplevel $insp $geom
    createOutputVectorViewer $insp
}

proc createEmbeddedOutputVectorInspector {insp geom} {
    createOutputVectorViewer $insp
}

proc createOutputVectorViewer {insp} {
    ttk::frame $insp.main
    ttk::frame $insp.bot
    pack $insp.bot -anchor center -expand 0 -fill x -side bottom
    pack $insp.main -anchor center -expand 1 -fill both -side top

    canvas $insp.main.canvas -borderwidth 2 -relief raised -background wheat1
    pack $insp.main.canvas -anchor center -expand 1 -fill both -side top

    ttk::label $insp.bot.info -width 50 -relief groove
    ttk::button $insp.bot.view -text "Options..." -command "OutputVectorInspector:options $insp"
    pack $insp.bot.view -anchor center -expand 0 -fill none -side right
    pack $insp.bot.info -anchor center -expand 1 -fill x -side left

    $insp.main.canvas bind all <Any-Enter> "OutputVectorInspector:mouse $insp %x %y 1"
    $insp.main.canvas bind all <Any-Leave> "OutputVectorInspector:mouse $insp %x %y 0"

    inspector:bindSideButtons $insp

    # make the window respond to resize events
    bind $insp <Configure> "opp_refreshinspector $insp"

    # we need to let the window display, otherwise the canvas size
    # (needed by the first draw) returned by [winfo width/height ...]
    # will be 1
    update idletasks
}

proc OutputVectorInspector:options {insp} {
    global tmp
    set w $insp.outvectoroptionsdialog

    createOkCancelDialog $w "Plotting Options"

    ttk::checkbutton $w.f.auto -text "Autoscale time and value axes" -variable tmp(autoscale)
    ttk::label $w.f.lbl -text "Manual axis settings:"
    label-entry $w.f.time "Time scale (sec/px):"
    label-entry $w.f.ymin "Ymin:"
    label-entry $w.f.ymax "Ymax:"
    label-combo $w.f.combo "Style:" {dots pins bars sample-hold lines}
    ttk::label $w.f.pad -text " "

    pack $w.f -anchor center -expand 1 -fill both -side top -padx 5 -pady 5
    pack $w.f.auto -anchor w -expand 0 -fill none -padx 3m -side top
    pack $w.f.lbl -anchor w -expand 0 -fill none -padx 3m -side top
    pack $w.f.time -anchor center -expand 1 -fill x -padx 5m -side top
    pack $w.f.ymin -anchor center -expand 1 -fill x -padx 5m -side top
    pack $w.f.ymax -anchor center -expand 1 -fill x -padx 5m -side top
    pack $w.f.combo -anchor center -expand 1 -fill x -padx 3m -side top
    pack $w.f.pad -anchor center -expand 1 -fill x -padx 3m -side top

    set settings [opp_inspectorcommand $insp config]
    set tmp(autoscale) [lindex $settings 0]
    $w.f.time.e insert 0 [lindex $settings 1]
    $w.f.ymin.e insert 0 [lindex $settings 2]
    $w.f.ymax.e insert 0 [lindex $settings 3]
    $w.f.combo.e set [lindex $settings 4]

    #TODO: add Apply button that invokes OutputVectorInspector:apply too
    if [execOkCancelDialog $w] {
        OutputVectorInspector:apply $w $insp
    }
    destroy $w
}

proc OutputVectorInspector:apply {w insp} {
    global tmp
    opp_inspectorcommand $insp config \
                     $tmp(autoscale) \
                     [$w.f.time.e get] \
                     [$w.f.ymin.e get] \
                     [$w.f.ymax.e get] \
                     [$w.f.combo.e get]
    opp_refreshinspector $insp
}

proc OutputVectorInspector:mouse {insp x y on} {
    global opp

    # mouse enters/leaves a data point's drawing in a outvector window
    # the index of the point is in the drawing's tag: "value12"
    set c $insp.main.canvas
    set tags [$c gettags current]
    if [regexp {.*value([0-9]+).*} $tags match value] {
       set no_outline [catch {$c itemconfig current -outline}]
       if {$on == 1} {
           # mouse enters a bar/dot/line/etc: paint it black
           if {$no_outline} {
              set opp(oldfill)    [lindex [$c itemconfig current -fill] 4]
              $c itemconfig current -fill black
           } else {
              set opp(oldfill)    [lindex [$c itemconfig current -fill] 4]
              set opp(oldoutline) [lindex [$c itemconfig current -outline] 4]
              $c itemconfig current -fill black -outline black
           }
           $insp.bot.info config -text [opp_inspectorcommand $insp value $value]
       } else {
           # mouse leaves the bars/dots/lines: restore original color
           if {$no_outline} {
              $c itemconfig current -fill $opp(oldfill)
           } else {
              $c itemconfig current -fill $opp(oldfill) -outline $opp(oldoutline)
           }
           $insp.bot.info config -text [opp_inspectorcommand $insp value]
       }
    }
}


