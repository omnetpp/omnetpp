#=================================================================
#  CANVASINSPECTOR.TCL - part of
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

#
# Note: tooltips on canvas come from the proc whose name is stored in
# $help_tips(helptip_proc). This is currently getHelpTip.
#

#TODO: limitations compared to ModuleInspector: no panning, no marquee zoom

proc createCanvasInspector {insp geom} {
    global icons help_tips config inspectordata CTRL_

    if {$config(layout-may-resize-window)} {
        # remove size from geom
        regsub -- {^[0-9]+x[0-9]+} $geom {} geom
    }

    createInspectorToplevel $insp $geom

    packToolbutton $insp.toolbar.sep2    -separator
    packToolbutton $insp.toolbar.redraw  -image $icons(redraw) -command "CanvasInspector:redraw $insp" ;# Note" this is re-layout in ModuleInspector
    packToolbutton $insp.toolbar.zoomin  -image $icons(zoomin)  -command "CanvasInspector:zoomIn $insp"
    packToolbutton $insp.toolbar.zoomout -image $icons(zoomout) -command "CanvasInspector:zoomOut $insp"

    set help_tips($insp.toolbar.parent)  "Go to parent"
    set help_tips($insp.toolbar.redraw)  "Redraw (${CTRL_}R)"
    set help_tips($insp.toolbar.zoomin)  "Zoom in (${CTRL_}M)"
    set help_tips($insp.toolbar.zoomout) "Zoom out (${CTRL_}N)"

    # create canvas
    createGraphicalCanvasViewer $insp
}

proc createEmbeddedCanvasInspector {insp} {
    global icons help_tips CTRL_

    createGraphicalCanvasViewer $insp

    set tb [inspector:createInternalToolbar $insp $insp.c]

    packToolbutton $tb.back    -image $icons(back)    -command "inspector:back $insp"
    packToolbutton $tb.forward -image $icons(forward) -command "inspector:forward $insp"
    packToolbutton $tb.parent  -image $icons(parent)  -command "inspector:inspectParent $insp"
    packToolbutton $tb.sep2    -separator
    packToolbutton $tb.redraw  -image $icons(redraw)  -command "CanvasInspector:redraw $insp"
    packToolbutton $tb.zoomin  -image $icons(zoomin)  -command "CanvasInspector:zoomIn $insp"
    packToolbutton $tb.zoomout -image $icons(zoomout) -command "CanvasInspector:zoomOut $insp"

    set help_tips($tb.back)    "Back"
    set help_tips($tb.forward) "Forward"
    set help_tips($tb.parent)  "Go to parent"
    set help_tips($tb.redraw)  "Redraw (${CTRL_}R)"
    set help_tips($tb.zoomin)  "Zoom in (${CTRL_}M)"
    set help_tips($tb.zoomout) "Zoom out (${CTRL_}N)"
}

proc createGraphicalCanvasViewer {insp} {
    global inspectordata
    global B2 B3 CTRL Control

    # create widgets inside $insp
    ttk::frame $insp.grid
    pack $insp.grid -expand yes -fill both -padx 1 -pady 1

    set c $insp.c
    tkp::canvas $c -background "#a0e0a0" -relief raised -closeenough 2
    addScrollbars $c $insp.grid

    label $insp.zoominfo -borderwidth 0 -text "Zoom: 0.00x"
    place $insp.zoominfo -in $c -relx 1.0 -rely 1.0 -anchor se -x -2 -y -2

    # initialize state vars
    set inspectordata($c:zoomfactor) 1
    set inspectordata($c:imagesizefactor) 1
    set inspectordata($c:showlabels) 1
    set inspectordata($c:showarrowheads) 1

    # mouse bindings
    $c bind fig <1> "CanvasInspector:click $insp"
    $c bind fig <$B3> "CanvasInspector:rightClick $insp %X %Y %x %y"

    inspector:bindSideButtons $insp

    # keyboard shortcuts
    set ww [winfo toplevel $insp]
    bind $ww <$Control-m> "CanvasInspector:zoomIn $insp"
    bind $ww <$Control-n> "CanvasInspector:zoomOut $insp"
    bind $ww <$Control-plus> "CanvasInspector:zoomIn $insp"
    bind $ww <$Control-minus> "CanvasInspector:zoomOut $insp"
    bind $ww <$Control-KP_Add> "CanvasInspector:zoomIn $insp"
    bind $ww <$Control-KP_Subtract> "CanvasInspector:zoomOut $insp"
    bind $ww <$Control-r> "CanvasInspector:redraw $insp"
}

proc CanvasInspector:onSetObject {insp} {
    #update idletasks
    update

    CanvasInspector:recallPreferences $insp

    if [catch {
       opp_inspectorcommand $insp redraw
    } errmsg] {
       tk_messageBox -type ok -title "Error" -icon error -parent [winfo toplevel [focusOrRoot]] \
                     -message "Error displaying graphics: $errmsg"
    }

    CanvasInspector:updateZoomLabel $insp
    CanvasInspector:adjustWindowSizeAndZoom $insp
}

proc CanvasInspector:redraw {insp} {
    opp_inspectorcommand $insp redraw
}

proc CanvasInspector:recallPreferences {insp} {
    global config inspectordata

    # defaults
    set c $insp.c
    set inspectordata($c:zoomfactor) 1
    set inspectordata($c:imagesizefactor) 1
    set inspectordata($c:showlabels) 1
    set inspectordata($c:showarrowheads) 1
    set inspectordata($c:layoutseed) 0

    # overwrite them with saved values if they exist
    set ptr [opp_inspector_getobject $insp]
    if [opp_isnull $ptr] return
    set typename [opp_getobjectshorttypename $ptr]
    set insptype [opp_inspector_istoplevel $insp]
    set key $typename:$insptype

    if [info exist config($key:zoomfactor)] {set inspectordata($c:zoomfactor) $config($key:zoomfactor)}
    if [info exist config($key:imagesizefactor)] {set inspectordata($c:imagesizefactor) $config($key:imagesizefactor)}
    if [info exist config($key:showlabels)] {set inspectordata($c:showlabels) $config($key:showlabels)}
    if [info exist config($key:showarrowheads)] {set inspectordata($c:showarrowheads) $config($key:showarrowheads)}
    if [info exist config($key:layoutseed)] {set inspectordata($c:layoutseed) $config($key:layoutseed)}
}

proc CanvasInspector:updatePreferences {insp} {
    global config inspectordata

    set ptr [opp_inspector_getobject $insp]
    if [opp_isnull $ptr] return
    set typename [opp_getobjectshorttypename $ptr]
    set insptype [opp_inspector_istoplevel $insp]
    set key $typename:$insptype
    set c $insp.c

    set config($key:zoomfactor) $inspectordata($c:zoomfactor)
    set config($key:imagesizefactor) $inspectordata($c:imagesizefactor)
    set config($key:showlabels) $inspectordata($c:showlabels)
    set config($key:showarrowheads) $inspectordata($c:showarrowheads)
    set config($key:layoutseed) $inspectordata($c:layoutseed)
}

proc CanvasInspector:adjustWindowSizeAndZoom {insp} {
    global config

    if {!$config(layout-may-resize-window) && !$config(layout-may-change-zoom)} {
        CanvasInspector:setScrollRegion $insp 1
        return
    }

    set c $insp.c

    # if needed, resize window to fit graphics; if not enough, additionally zoom out as well
    set bb [$c bbox "fig"]
    set graphicswidth [expr [lindex $bb 2]-[lindex $bb 0]]
    set graphicsheight [expr [lindex $bb 3]-[lindex $bb 1]]

    if {!$config(layout-may-resize-window)} {
        # do not resize, but change zoom so that graphics fills the window
        # note: $config(layout-may-change-zoom) is TRUE here because of the above "if"
        set canvaswidth [winfo width $c]
        set canvasheight [winfo height $c]
        if {$canvaswidth == 0} {set canvaswidth 400}  ;# approximate size for (rare) case when window size is not yet available
        if {$canvasheight == 0} {set canvasheight 300}
        set canvaswidth2 [expr $canvaswidth - 20]  ;# deduce 10px border around compound Canvas
        set canvasheight2 [expr $canvasheight - 20]
        if {$canvaswidth2 < 50} {set canvaswidth2 50}
        if {$canvasheight2 < 50} {set canvasheight2 50}

        set zoomx [expr $canvaswidth2 / double($graphicswidth)]
        set zoomy [expr $canvasheight2 / double($graphicsheight)]
        set zoom [mathMin $zoomx $zoomy]
        if {$zoom < 0.001} {set zoom 0.001}
        if {$zoom > 1000} {set zoom 1000}

        if {$zoom < 1.0} {
            CanvasInspector:zoomBy $insp $zoom
        }
        CanvasInspector:setScrollRegion $insp 1

    } else {
        # we'll need to resize the window, and then either zoom out or not

        # compute maximum available canvas size first that would fit on the screen
        wmGetDesktopBounds $insp desktop ;# fills $desktop(top), $desktop(width), etc.
        wmGetDecorationSize border    ;# fills $border(top), $border(left), etc.
        set maxwinwidth [expr $desktop(width) - $border(left) - $border(right) - 30]
        set maxwinheight [expr $desktop(height) - $border(top) - $border(bottom) - 20]

        set chromewidth [expr [winfo width $insp] - [winfo width $c]]
        set chromeheight [expr [winfo height $insp] - [winfo height $c]]
        if {$chromewidth <= 0} {set chromewidth 0}  ;# in case something was wrong with "winfo width"
        if {$chromeheight <= 0} {set chromeheight 0}

        set scrollbarwidth 24
        set scrollbarheight 24

        set margin 10  ;# 10px space around canvas

        set maxcanvaswidth [expr $maxwinwidth - $chromewidth - $scrollbarwidth - $margin]
        set maxcanvasheight [expr $maxwinheight - $chromeheight - $scrollbarheight - $margin]

        # compute zoomby factor; this is either 1.0 (no change), or less than 1.0 (zoom out)
        if {!$config(layout-may-change-zoom)} {
            set zoom 1
        } else {
            set zoomx 1.0
            set zoomy 1.0
            if {$graphicswidth > $maxcanvaswidth} {
                set zoomx [expr $maxcanvaswidth / double($graphicswidth)]
            }
            if {$graphicsheight > $maxcanvasheight} {
                set zoomy [expr $maxcanvasheight / double($graphicsheight)]
            }
            if {$zoomx < $zoomy} {set zoom $zoomx} else {set zoom $zoomy}
            if {$zoom < 0.001} {set zoom 0.001}
            if {$zoom > 1000} {set zoom 1000}
        }

        set zoomedgraphicswidth [expr $zoom * $graphicswidth]
        set zoomedgraphicsheight [expr $zoom * $graphicsheight]

        set canvaswidth [expr [mathMin $zoomedgraphicswidth $maxcanvaswidth] + 30]
        set canvasheight [expr [mathMin $zoomedgraphicsheight $maxcanvasheight] + 30]

        # set size and zoom
        $c config -width $canvaswidth
        $c config -height $canvasheight
        CanvasInspector:zoomBy $insp $zoom

        # allow the window to appear, so that scrollbars know their real size;
        # this is needed for "$c xview moveto" inside CanvasInspector:setScrollRegion
        # to work properly
        update idletasks

        CanvasInspector:setScrollRegion $insp 1

        # move the window so that it is fully on the screen -- this is not
        # such a good idea in practice (can be annoying/confusing)
        #moveToScreen $insp
    }
}

#
# Sets the scrolling region for a graphical Canvas inspector.
# NOTE: This method is invoked from C++.
#
proc CanvasInspector:setScrollRegion {insp moveToOrigin} {
    set c $insp.c

    # scrolling region
    set bb [$c bbox all]
    set x1 [expr [lindex $bb 0]-10]
    set y1 [expr [lindex $bb 1]-10]
    set x2 [expr [lindex $bb 2]+10]
    set y2 [expr [lindex $bb 3]+10]
    $c config -scrollregion [list $x1 $y1 $x2 $y2]

    # scroll to top-left corner of compound Canvas to top-left corner of window
    if {$moveToOrigin} {
        set enclosingmodbb [$c bbox mod]
        set mx1 [expr [lindex $enclosingmodbb 0]-10]
        set my1 [expr [lindex $enclosingmodbb 1]-10]

        $c xview moveto [expr ($mx1 - $x1) / double($x2 - $x1)]
        $c yview moveto [expr ($my1 - $y1) / double($y2 - $y1)]
    }
}

# proc mathMin {a b} {
#     return [expr ($a < $b) ? $a : $b]
# }

# proc mathMax {a b} {
#     return [expr ($a > $b) ? $a : $b]
# }

proc CanvasInspector:zoomIn {insp} {
    global config
    CanvasInspector:zoomBy $insp $config(zoomby-factor) 1
}

proc CanvasInspector:zoomOut {insp} {
    global config
    CanvasInspector:zoomBy $insp [expr 1.0 / $config(zoomby-factor)] 1
}

proc CanvasInspector:zoomBy {insp mult {snaptoone 0}} {
    global inspectordata
    set c $insp.c
    if {($mult<1 && $inspectordata($c:zoomfactor)>0.001) || ($mult>1 && $inspectordata($c:zoomfactor)<1000)} {
        # update zoom factor and redraw
        set inspectordata($c:zoomfactor) [expr $inspectordata($c:zoomfactor) * $mult]

        # snap to 1 (note: this is not desirable when zoom is set programmatically to fit network into window)
        if {$snaptoone} {
            set m [expr $mult < 1 ? 1.0/$mult : $mult]
            set a [expr  1 - 0.9*(1 - 1.0/$m)]
            set b [expr  1 + 0.9*($m - 1)]
            if {$inspectordata($c:zoomfactor) > $a && $inspectordata($c:zoomfactor) < $b} {
                set inspectordata($c:zoomfactor) 1
            }
        }

        opp_inspectorcommand $insp redraw
        CanvasInspector:setScrollRegion $insp 0

        CanvasInspector:updateZoomLabel $insp
    }

    CanvasInspector:updatePreferences $insp

    #CanvasInspector:popOutToolbarButtons $insp
}

proc CanvasInspector:updateZoomLabel {insp} {
    global inspectordata
    set c $insp.c
    set value [format "%.2f" $inspectordata($c:zoomfactor)]
    $insp.zoominfo config -text "Zoom: ${value}x"
}

proc CanvasInspector:click {insp} {
    set c $insp.c
    set item [$c find withtag current]
    set tags [$c gettags $item]

    set ptr ""
    if {[lsearch $tags "ptr*"] != -1} {
        regexp "ptr.*" $tags ptr
    }

    if {$ptr!=""} {
        mainWindow:selectionChanged $insp $ptr
    }
}

proc CanvasInspector:rightClick {insp X Y x y} {
   global inspectordata tmp CTRL

   #TODO: CanvasInspector:zoomMarqueeCancel $insp ;# just in case

   set c $insp.c
   set ptrs [ModuleInspector:getPtrsUnderMouse $c $x $y]  ;#TODO

   if {$ptrs != {}} {

      set popup [createInspectorContextMenu $insp $ptrs]

      $popup add separator
      $popup add command     -label "Show/Hide Canvas Layers..." -command "CanvasInspector:layers $insp"

      $popup add separator
      $popup add command -label "Zoom In"  -accel "$CTRL+M" -command "CanvasInspector:zoomIn $insp"
      $popup add command -label "Zoom Out" -accel "$CTRL+N" -command "CanvasInspector:zoomOut $insp"

      tk_popup $popup $X $Y
   }
}

proc CanvasInspector:layers {insp} {
    set allTags [opp_inspectorcommand $insp getalltags]
    set enabledTags [opp_inspectorcommand $insp getenabledtags]
    set exceptTags [opp_inspectorcommand $insp getexcepttags]

    set result [CanvasInspectors:layersDialog $allTags $enabledTags $exceptTags]

    if {$result != {}} {
        set enabledTags [lindex $result 0]
        set exceptTags [lindex $result 1]
        opp_inspectorcommand $insp setenabledtags $enabledTags
        opp_inspectorcommand $insp setexcepttags $exceptTags
        opp_inspectorcommand $insp redraw
    }
}

# TODO: remember tags settings (save to tkenvrc)

proc CanvasInspectors:layersDialog {allTags enabledTags exceptTags} {
    global tmp

    set allTags [split $allTags] ;# needed for tags containing curly braces!
    set enabledTags [split $enabledTags]
    set exceptTags [split $exceptTags]

    catch {unset tmp}

    set title "Select Layers"
    set w .layersdialog
    createOkCancelDialog $w $title 1

    commentlabel $w.f.msg "Select which figures to display, based on the list of tags each figure contains."
    ttk::label $w.f.enabledmsg -text "Show figures having any of the tags:" -anchor w -justify left
    ttk::frame $w.f.enabledframe -relief sunken
    ttk::label $w.f.exceptmsg -text "But hide those that also contain:" -anchor w -justify left
    ttk::frame $w.f.exceptframe -relief sunken
    commentlabel $w.f.note "Note: Untagged figures are always shown.\n" 60

    grid $w.f.msg - -sticky we -padx 10 -pady 5
    grid $w.f.enabledmsg $w.f.exceptmsg -sticky we -padx 10 -pady 5
    grid $w.f.enabledframe $w.f.exceptframe -sticky news -padx 10 -pady 5
    grid $w.f.note - -sticky we -padx 10 -pady 5
    grid columnconfigure $w.f 0 -weight 1 -uniform 42
    grid columnconfigure $w.f 1 -weight 1 -uniform 42
    grid rowconfigure $w.f 2 -weight 1

    set tmp(enabledlist) $w.f.enabledframe.c
    CanvasInspectors:layersDialog:createCanvas $tmp(enabledlist)

    set tmp(exceptlist) $w.f.exceptframe.c
    CanvasInspectors:layersDialog:createCanvas $tmp(exceptlist)

    set allTags [lsort -dictionary $allTags]
    set i 0
    foreach tag $allTags {
        set label [regsub -all -- "_" $tag " "]
        set tmp($tag:enabled) [lcontains $enabledTags $tag]
        ttk::checkbutton $tmp(enabledlist).cb$i -text $label -variable tmp($tag:enabled) -command CanvasInspectors:layersDialog:refreshExceptList
        CanvasInspectors:layersDialog:addToCanvas $tmp(enabledlist) $tmp(enabledlist).cb$i

        set tmp($tag:except) [lcontains $exceptTags $tag]
        ttk::checkbutton $tmp(exceptlist).cb$i -text $label -variable tmp($tag:except)
        incr i
    }

    if {$allTags == {}} {
        ttk::label $tmp(enabledlist).msg -text "<No tags in any figure>" -anchor w -justify left
        CanvasInspectors:layersDialog:addToCanvas $tmp(enabledlist) $tmp(enabledlist).msg
    }

    CanvasInspectors:layersDialog:refreshExceptList

    if [execOkCancelDialog $w] {
        set enabledTags {}
        set exceptTags {}
        foreach tag $allTags {
            # note: use 'append' not 'lappend'! cf with 'split' above
            if {$tmp($tag:enabled)} {append enabledTags $tag; append enabledTags " "; }
            if {!$tmp($tag:enabled) && $tmp($tag:except)} {append exceptTags $tag; append exceptTags " "}
        }
        destroy $w
        return [list $enabledTags $exceptTags]
    }
    destroy $w
    return {}
}

proc CanvasInspectors:layersDialog:createCanvas {c} {
    tkp::canvas $c -width 50 -height 250 -highlightthickness 0
    $c create pline {0 0 0 0} -strokeopacity 0  ;# ensure we always have a valid bbox inside addToCanvas
    addScrollbars $c
    grid configure $c -padx 2 -pady 2 ;# otherwise surrounding frame's border is obscured
}

proc CanvasInspectors:layersDialog:addToCanvas {c w} {
    $c create window 8 [lindex [$c bbox all] 3] -window $w -anchor nw -tags widgets
    $c config -scrollregion [$c bbox all]
}

proc CanvasInspectors:layersDialog:refreshExceptList {} {
    global tmp

    # the "Except" panel should only show the tags not selected in the "Show" panel
    $tmp(exceptlist) delete widgets
    foreach checkbox [winfo children $tmp(exceptlist)] {
        set label [$checkbox cget -text]
        set tag [regsub -all -- " " $label "_"]
        if {!$tmp($tag:enabled)} {
            CanvasInspectors:layersDialog:addToCanvas $tmp(exceptlist) $checkbox
        }
    }
}

# proc lookupImage {imgname {imgsize ""}} {
#     global bitmaps icons
#
#     if {[catch {set img $bitmaps($imgname,$imgsize)}] && \
#         [catch {set img $bitmaps($imgname)}]} {
#        set img $icons(unknown)
#     }
#
#     return $img
# }

