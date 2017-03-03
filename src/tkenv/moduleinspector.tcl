#=================================================================
#  MODULEINSPECTOR.TCL - part of
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


# Halo effect around text items, to improve readability when bg is noisy or several text items overlap.
# grey82 is the halo color that blends in with all kinds of backgrounds, yet doesn't affect readability;
# it's also the default module background color.
# To turn off halo: set canvasTextOptions {}
set canvasTextOptions {-stroke grey82 -strokewidth 5 -strokeopacity 0.6 -filloverstroke on}

#
# Note: tooltips on canvas come from the proc whose name is stored in
# $help_tips(helptip_proc). This is currently getHelpTip.
#

proc createModuleInspector {insp geom} {
    global icons help_tips config inspectordata CTRL_

    if {$config(layout-may-resize-window)} {
        # remove size from geom
        regsub -- {^[0-9]+x[0-9]+} $geom {} geom
    }

    createInspectorToplevel $insp $geom

    # create toolbar
    packToolbutton $insp.toolbar.sep1 -separator
    ModuleInspector:addRunButtons $insp

    packToolbutton $insp.toolbar.sep2    -separator
    packToolbutton $insp.toolbar.redraw  -image $icons(redraw) -command "ModuleInspector:relayout $insp"
    packToolbutton $insp.toolbar.zoomin  -image $icons(zoomin)  -command "ModuleInspector:zoomIn $insp"
    packToolbutton $insp.toolbar.zoomout -image $icons(zoomout) -command "ModuleInspector:zoomOut $insp"

    set help_tips($insp.toolbar.parent)  "Go to parent module"
    set help_tips($insp.toolbar.redraw)  "Re-layout (${CTRL_}R)"
    set help_tips($insp.toolbar.zoomin)  "Zoom in (${CTRL_}M)"
    set help_tips($insp.toolbar.zoomout) "Zoom out (${CTRL_}N)"

    # create canvas
    createGraphicalModuleViewer $insp
}

proc createEmbeddedModuleInspector {insp} {
    global icons help_tips CTRL_

    createGraphicalModuleViewer $insp

    set tb [inspector:createInternalToolbar $insp $insp.c]

    packToolbutton $tb.back    -image $icons(back)    -command "inspector:back $insp"
    packToolbutton $tb.forward -image $icons(forward) -command "inspector:forward $insp"
    packToolbutton $tb.parent  -image $icons(parent)  -command "inspector:inspectParent $insp"
    packToolbutton $tb.sep1    -separator
    packToolbutton $tb.mrun    -image $icons(mrun)    -command "runSimulationLocal $insp normal"
    packToolbutton $tb.mfast   -image $icons(mfast)   -command "runSimulationLocal $insp fast"
    packToolbutton $tb.stop    -image $icons(stop)    -command {stopSimulation}
    packToolbutton $tb.sep2    -separator
    packToolbutton $tb.redraw  -image $icons(redraw)  -command "ModuleInspector:relayout $insp"
    packToolbutton $tb.zoomin  -image $icons(zoomin)  -command "ModuleInspector:zoomIn $insp"
    packToolbutton $tb.zoomout -image $icons(zoomout) -command "ModuleInspector:zoomOut $insp"

    set help_tips($tb.back)    "Back"
    set help_tips($tb.forward) "Forward"
    set help_tips($tb.parent)  "Go to parent module"
    set help_tips($tb.mrun)    "Run until next event in this module"
    set help_tips($tb.mfast)   "Fast run until next event in this module (${CTRL_}F4)"
    set help_tips($tb.stop)    "Stop the simulation (F8)"
    set help_tips($tb.redraw)  "Re-layout (${CTRL_}R)"
    set help_tips($tb.animspeed) "Animation speed -- see Options dialog"
    set help_tips($tb.zoomin)  "Zoom in (${CTRL_}M)"
    set help_tips($tb.zoomout) "Zoom out (${CTRL_}N)"
}

proc createGraphicalModuleViewer {insp} {
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
    $c bind mod <1> "ModuleInspector:click $insp"
    $c bind submod <1> "ModuleInspector:click $insp"
    $c bind conn <1> "ModuleInspector:click $insp"
    $c bind msg <1> "ModuleInspector:click $insp"
    $c bind msgname <1> "ModuleInspector:click $insp"
    $c bind fig <1> "ModuleInspector:click $insp"
    $c bind qlen <1> "ModuleInspector:qlenClick $insp"

    $c bind submod <Double-1> "ModuleInspector:dblClick $insp"
    $c bind conn <Double-1> "ModuleInspector:dblClick $insp"
    $c bind msg <Double-1> "ModuleInspector:dblClick $insp"
    $c bind msgname <Double-1> "ModuleInspector:dblClick $insp"
    $c bind qlen <Double-1> "ModuleInspector:qlenDblclick $insp"

    $c bind submod <$B3> "ModuleInspector:rightClick $insp %X %Y %x %y"
    $c bind conn <$B3> "ModuleInspector:rightClick $insp %X %Y %x %y"
    $c bind msg <$B3> "ModuleInspector:rightClick $insp %X %Y %x %y"
    $c bind msgname <$B3> "ModuleInspector:rightClick $insp %X %Y %x %y"
    $c bind mod <$B3> "ModuleInspector:rightClick $insp %X %Y %x %y"
    $c bind modname <$B3> "ModuleInspector:rightClick $insp %X %Y %x %y"
    $c bind qlen <$B3> "ModuleInspector:qlenRightClick $insp %X %Y %x %y"

    inspector:bindSideButtons $insp

    # keyboard shortcuts
    set ww [winfo toplevel $insp]
    bind $ww <$Control-m> "ModuleInspector:zoomIn $insp"
    bind $ww <$Control-n> "ModuleInspector:zoomOut $insp"
    bind $ww <$Control-plus> "ModuleInspector:zoomIn $insp"
    bind $ww <$Control-minus> "ModuleInspector:zoomOut $insp"
    bind $ww <$Control-KP_Add> "ModuleInspector:zoomIn $insp"
    bind $ww <$Control-KP_Subtract> "ModuleInspector:zoomOut $insp"
    bind $ww <$Control-i> "ModuleInspector:zoomIconsBy $insp 1.25"
    bind $ww <$Control-o> "ModuleInspector:zoomIconsBy $insp 0.8"
    bind $ww <$Control-r> "ModuleInspector:relayout $insp"
    bind $ww <$Control-l> "ModuleInspector:toggleLabels $insp"
    bind $ww <$Control-a> "ModuleInspector:toggleArrowheads $insp"

    # pan / zoom
    $c bind mod <Double-1> "ModuleInspector:zoomIn $insp %x %y"
    $c bind mod <Shift-Double-1> "ModuleInspector:zoomOut $insp %x %y"

    bind $c <$Control-1> "+ModuleInspector:zoomMarqueeBegin $insp %x %y"
    bind $c <B1-Motion> "+ModuleInspector:zoomMarqueeUpdate $insp %x %y"
    bind $c <ButtonRelease-1> "+ModuleInspector:zoomMarqueeEnd $insp %x %y"
    bind $c <2> "+ModuleInspector:zoomMarqueeCancel $insp"
    bind $c <3> "+ModuleInspector:zoomMarqueeCancel $insp"

    bind $c <1> "+ModuleInspector:panStart $insp %x %y"
    bind $c <B1-Motion> "+ModuleInspector:panUpdate $insp %x %y"
    bind $c <ButtonRelease-1> "+ModuleInspector:panEnd $insp %x %y"
}

proc ModuleInspector:onSetObject {insp} {
    #update idletasks
    update

    ModuleInspector:recallPreferences $insp

    if [catch {
       opp_inspectorcommand $insp relayout
    } errmsg] {
       tk_messageBox -type ok -title "Error" -icon error -parent [winfo toplevel [focusOrRoot]] \
                     -message "Error displaying network graphics: $errmsg"
    }

    ModuleInspector:updateZoomLabel $insp
    ModuleInspector:adjustWindowSizeAndZoom $insp
}

proc ModuleInspector:recallPreferences {insp} {
    global config inspectordata

    # defaults
    set c $insp.c
    set inspectordata($c:zoomfactor) 1
    set inspectordata($c:imagesizefactor) 1
    set inspectordata($c:showlabels) 1
    set inspectordata($c:showarrowheads) 1
    set inspectordata($c:layoutseed) [opp_inspectorcommand $insp getdefaultlayoutseed]

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

    opp_inspectorcommand $insp setlayoutseed $inspectordata($c:layoutseed)
}

proc ModuleInspector:updatePreferences {insp} {
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

proc ModuleInspector:adjustWindowSizeAndZoom {insp} {
    global config

    if {!$config(layout-may-resize-window) && !$config(layout-may-change-zoom)} {
        ModuleInspector:setScrollRegion $insp 1
        return
    }

    set c $insp.c

    # if needed, resize window to fit graphics; if not enough, additionally zoom out as well
    set bb [$c bbox "mod"] ;# bounding box of the compound module
    if {$bb == {}} {return}
    set graphicswidth [expr [lindex $bb 2]-[lindex $bb 0]]
    set graphicsheight [expr [lindex $bb 3]-[lindex $bb 1]]

    if {!$config(layout-may-resize-window)} {
        # do not resize, but change zoom so that graphics fills the window
        # note: $config(layout-may-change-zoom) is TRUE here because of the above "if"
        set canvaswidth [winfo width $c]
        set canvasheight [winfo height $c]
        if {$canvaswidth == 0} {set canvaswidth 400}  ;# approximate size for (rare) case when window size is not yet available
        if {$canvasheight == 0} {set canvasheight 300}
        set canvaswidth2 [expr $canvaswidth - 20]  ;# deduce 10px border around compound module
        set canvasheight2 [expr $canvasheight - 20]
        if {$canvaswidth2 < 50} {set canvaswidth2 50}
        if {$canvasheight2 < 50} {set canvasheight2 50}

        set zoomx [expr $canvaswidth2 / double($graphicswidth)]
        set zoomy [expr $canvasheight2 / double($graphicsheight)]
        set zoom [mathMin $zoomx $zoomy]
        if {$zoom < 0.001} {set zoom 0.001}
        if {$zoom > 1000} {set zoom 1000}

        if {$zoom < 1.0} {
            ModuleInspector:zoomBy $insp $zoom
        }
        ModuleInspector:setScrollRegion $insp 1

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

        set margin 10  ;# 10px space around compound module

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
        ModuleInspector:zoomBy $insp $zoom

        # allow the window to appear, so that scrollbars know their real size;
        # this is needed for "$c xview moveto" inside ModuleInspector:setScrollRegion
        # to work properly
        update idletasks

        ModuleInspector:setScrollRegion $insp 1

        # move the window so that it is fully on the screen -- this is not
        # such a good idea in practice (can be annoying/confusing)
        #moveToScreen $insp
    }
}

proc min {a b} {
    if {$a < $b} {return $a} else {return $b}
}

proc max {a b} {
    if {$a < $b} {return $b} else {return $a}
}


#
# Sets the scrolling region for a graphical module inspector.
# NOTE: This method is invoked from C++.
#
proc ModuleInspector:setScrollRegion {insp moveToOrigin} {
    global myScrollRegion
    set c $insp.c

    # scrolling region
    set bbox [$c bbox all]
    if {$bbox == {}} {set bbox {0 0 0 0}}
    lassign $bbox x1 y1 x2 y2
    incr x1 -10; incr y1 -10
    incr x2 10; incr y2 10

    # never shrink scrolling region as it may cause "jerking" when e.g. transmission circles come and go
    set oldRegion [$c cget -scrollregion]
    if {$oldRegion!={}} {
        lassign $oldRegion rx1 ry1 rx2 ry2
        set x1 [min $rx1 $x1]
        set y1 [min $ry1 $y1]
        set x2 [max $rx2 $x2]
        set y2 [max $ry2 $y2]
    }
    $c config -scrollregion [list $x1 $y1 $x2 $y2]

    # store for later use
    set myScrollRegion(x1) $x1
    set myScrollRegion(y1) $y1
    set myScrollRegion(x2) $x2
    set myScrollRegion(y2) $y2

    # scroll to top-left corner of compound module to top-left corner of window
    if {$moveToOrigin} {
        set enclosingmodbb [$c bbox mod]
        set mx1 [expr [lindex $enclosingmodbb 0]-10]
        set my1 [expr [lindex $enclosingmodbb 1]-10]

        $c xview moveto [expr ($mx1 - $x1) / double($x2 - $x1)]
        $c yview moveto [expr ($my1 - $y1) / double($y2 - $y1)]
    }

}

proc ModuleInspector:zoomCanvasCoordsToRealCoords {insp x1 y1 x2 y2} {
    global inspectordata
    set c $insp.c
    set factor $inspectordata($c:zoomfactor)
    set x1 [expr { 1.0 * $x1 / $factor }]
    set y1 [expr { 1.0 * $y1 / $factor }]
    set x2 [expr { 1.0 * $x2 / $factor }]
    set y2 [expr { 1.0 * $y2 / $factor }]
    return [list $x1 $y1 $x2 $y2]
}

proc ModuleInspector:zoomUpdateLabel {insp x1 y1 x2 y2} {
    $insp.zoominfo config -text [format "Zoom: from (%.1f, %.1f) to (%.1f, %.1f)" $x1 $y1 $x2 $y2]
}

set zoomMarquee ""

proc ModuleInspector:zoomMarqueeBegin {insp x y} {
    global zoomMarquee

    # store coordinates
    set c $insp.c
    set x1 [$c canvasx $x]
    set y1 [$c canvasy $y]
    set x2 $x1
    set y2 $y1
    set zoomMarquee [list $x1 $y1 $x2 $y2]

    # add rectangle
    $c create prect {*}$zoomMarquee -stroke black -strokedasharray {3 3} -tag zoomMarqueeRect

    # update label
    set realCoords [ ModuleInspector:zoomCanvasCoordsToRealCoords $insp {*}$zoomMarquee ]
    ModuleInspector:zoomUpdateLabel $insp {*}$realCoords
}

proc ModuleInspector:zoomMarqueeUpdate {insp x y} {
    global zoomMarquee

    if {$zoomMarquee==""} return ;# marquee zoom not in progress

    # store coordinates
    set c $insp.c
    lassign $zoomMarquee x1 y1 - -
    set x2 [$c canvasx $x]
    set y2 [$c canvasy $y]
    set zoomMarquee [list $x1 $y1 $x2 $y2]

    # update rectangle
    $c coords zoomMarqueeRect {*}$zoomMarquee

    # update label
    set realCoords [ ModuleInspector:zoomCanvasCoordsToRealCoords $insp {*}$zoomMarquee ]
    ModuleInspector:zoomUpdateLabel $insp {*}$realCoords
}

proc ModuleInspector:zoomMarqueeEnd {insp x y} {
    global zoomMarquee

    if {$zoomMarquee==""} return ;# marquee zoom not in progress

    # obtain coordinates
    set c $insp.c
    lassign $zoomMarquee x1 y1 - -
    set x2 [$c canvasx $x]
    set y2 [$c canvasy $y]
    if {$x2 < $x1} {foreach {x1 x2} [list $x2 $x1] break}
    if {$y2 < $y1} {foreach {y1 y2} [list $y2 $y1] break}

    # marquee zoom finished
    set zoomMarquee ""
    $c delete zoomMarqueeRect

    # do nothing if marquee is too small
    if {$x1==$x2 || $y1==$y2} {
        ModuleInspector:updateZoomLabel $insp
        return
    }

    set realCoords [ ModuleInspector:zoomCanvasCoordsToRealCoords $insp $x1 $y1 $x2 $y2 ]
    ModuleInspector:zoomToRegion $insp {*}$realCoords
}

proc ModuleInspector:zoomMarqueeCancel {insp} {
    global zoomMarquee
    if {$zoomMarquee==""} return ;# marquee zoom not in progress
    set c $insp.c
    set zoomMarquee ""
    $c delete zoomMarqueeRect
    ModuleInspector:updateZoomLabel $insp
}

proc ModuleInspector:zoomToRegion {insp x1 y1 x2 y2} {
    global inspectordata
    global myScrollRegion

    set c $insp.c

    set winxlength [winfo width $c]
    set winylength [winfo height $c]

    # Calculate $factor
    set xlength [expr {1.0*$x2-$x1}]
    set ylength [expr {1.0*$y2-$y1}]
    set xscale [expr {1.0*$winxlength/$xlength}]
    set yscale [expr {1.0*$winylength/$ylength}]
    if { $xscale > $yscale } {
        set factor $yscale
    } else {
        set factor $xscale
    }

    # Set zoom to $factor
    set inspectordata($c:zoomfactor) $factor
    opp_inspectorcommand $insp redraw
    ModuleInspector:setScrollRegion $insp 0
    ModuleInspector:updateZoomLabel $insp

    # Calculate
    set xcenter [expr {($x1+$x2)/2.0}]
    set ycenter [expr {($y1+$y2)/2.0}]
    set xborder [expr { $xcenter * $factor - .5 * $winxlength}]
    set yborder [expr { $ycenter * $factor - .5 * $winylength}]
    set xfrac [ expr { ($xborder - $myScrollRegion(x1)) / ($myScrollRegion(x2) - $myScrollRegion(x1)) }]
    set yfrac [ expr { ($yborder - $myScrollRegion(y1)) / ($myScrollRegion(y2) - $myScrollRegion(y1)) }]

    $c xview moveto $xfrac
    $c yview moveto $yfrac
}

proc ModuleInspector:panStart {insp x y} {
    set c $insp.c
    $c scan mark $x $y
}

proc ModuleInspector:panUpdate {insp x y} {
    global zoomMarquee
    if {$zoomMarquee != ""} return

    set c $insp.c
    $c scan dragto $x $y 1
}

proc ModuleInspector:panEnd {insp x y} {
}

proc ModuleInspector:layers {insp} {
    if {![opp_inspectorcommand $insp hascanvas]} {
        messagebox Confirm "No default canvas has been created for this module yet." info ok
        return
    }

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

proc lookupImage {imgname {imgsize ""}} {
    global bitmaps icons

    if {[catch {set img $bitmaps($imgname,$imgsize)}] && \
        [catch {set img $bitmaps($imgname)}]} {
       set img $icons(unknown)
    }

    return $img
}


#
# Resolves an image in the display string. Returns a 3-element list: (options, width, height)
#
proc dispstrGetImage {tags_i tags_is imagesizefactor {alphamult 1} {defaultimage ""}} {
    global icons bitmaps imagecache

    set iconminsize [opp_getsimoption iconminsize]

    set key "[join $tags_i ,]:[join $tags_is ,]:$imagesizefactor:$iconminsize:$alphamult"
    if {![info exist imagecache($key)]} {
        set options {}

        # look up base image
        set imgsize [lindex $tags_is 0]
        if {$imgsize==""} {set imgsize "n"}
        set imgname [lindex $tags_i 0]
        if {$imgname==""} {
            if {$defaultimage!=""} {
                set img $defaultimage
            } else {
                set img $icons(unknown)
            }
        } else {
            if {[catch {set img $bitmaps($imgname,$imgsize)}] && \
                [catch {set img $bitmaps($imgname)}]} {
                set img $icons(unknown)
            }
        }
        if {[catch {image type $img}]} {
            error "internal error: image referred to in bitmaps() doesn't exist"
        }
        lappend options -image $img

        # colorization
        if {[llength $tags_i]>1} {
            set tintColor [lindex $tags_i 1]
            set tintPercent [lindex $tags_i 2]
            if {$tintColor!=""} {
                if {[string index $tintColor 0]== "@"} {set tintColor [opp_hsb_to_rgb $tintColor]}
                if {$tintPercent==""} {set tintPercent 30}
                lappend options -tintcolor $tintColor -tintamount [expr $tintPercent / 100.0]
            }
        }

        # opacity
        if {$alphamult != 1} {
            lappend options -fillopacity $alphamult
        }

        # scaling
        set isx [image width $img]
        set isy [image height $img]
        if {$imagesizefactor==1} {
            set scaledsx $isx
            set scaledsy $isy
        } else {
            # iconminsize should not cause icon to grow above its original size
            if {$isx < $iconminsize } { set iconminsize $isx}
            if {$isy < $iconminsize } { set iconminsize $isy}

            # modify imagesizefactor so that both width/height > iconwidthsize, and aspect ratio is kept
            if {$imagesizefactor * $isx < $iconminsize} {
                set imagesizefactor [expr $iconminsize / double($isx)]
            }
            if {$imagesizefactor * $isy < $iconminsize} {
                set imagesizefactor [expr $iconminsize / double($isy)]
            }

            set scaledsx [expr int($imagesizefactor * $isx)]
            set scaledsy [expr int($imagesizefactor * $isy)]
            if {$scaledsx < 1} {set scaledsx 1}
            if {$scaledsy < 1} {set scaledsy 1}
            lappend options -width $scaledsx
            lappend options -height $scaledsy
        }

        set imagecache($key) [list $options $scaledsx $scaledsy]
    }
    return $imagecache($key)
}


#
# helper function
#
proc ModuleInspector:getSubmodCoords {c tag} {

   set id [$c find withtag $tag]
   if {$id==""} {error "$tag not found"}
   return [$c bbox $tag]  ;#-- this could be faster, but somehow doesn't work properly with senddirect animation

   if {[$c type $id]=="image"} {
       set pos [$c coords $id]
       set x [lindex $pos 0]
       set y [lindex $pos 1]
       set img [$c itemcget $id -image]
       set hwidth  [expr [image width $img] / 2]
       set hheight [expr [image height $img] / 2]

       set coords "[expr $x-$hwidth] [expr $y-$hheight] \
                       [expr $x+$hwidth] [expr $y+$hheight]"
   } else {
       set coords [$c coords $id]
   }
   return $coords
}


# ModuleInspector:drawSubmodule --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:drawSubmodule {c submodptr x y name dispstr isplaceholder} {
   #puts "DEBUG: ModuleInspector:drawSubmodule $c $submodptr $x $y $name $dispstr $isplaceholder"
   global icons inspectordata tkpFont canvasTextOptions

   set zoom $inspectordata($c:zoomfactor)
   set imagesizefactor $inspectordata($c:imagesizefactor)

   set alphamult 1
   if {$isplaceholder} {
       set alphamult 0.3
   }

   if [catch {
       opp_displaystring $dispstr parse tags $submodptr 1

       # scale (x,y)
       if {$zoom != ""} {
           set x [expr $zoom*$x]
           set y [expr $zoom*$y]
       }

       # set sx and sy (and look up image)
       set isx 0
       set isy 0
       set bsx 0
       set bsy 0
       if ![info exists tags(is)] {
           set tags(is) {}
       }
       if [info exists tags(i)] {
           setvars {img isx isy} [dispstrGetImage $tags(i) $tags(is) $imagesizefactor $alphamult $icons(defaulticon)]
       }

       if [info exists tags(b)] {
           set bsx [lindex $tags(b) 0]
           set bsy [lindex $tags(b) 1]
           if {$bsx=="" && $bsy==""} {
               set bsx 40
               set bsy 24
           }
           if {$bsx==""} {set bsx $bsy}
           if {$bsy==""} {set bsy $bsx}
           if {$zoom != ""} {
               set bsx [expr $zoom*$bsx]
               set bsy [expr $zoom*$bsy]
           }
       } elseif ![info exists tags(i)] {
           setvars {img isx isy} [dispstrGetImage "" "" $imagesizefactor $alphamult $icons(defaulticon)]
       }

       set sx [expr {$isx<$bsx ? $bsx : $isx}]
       set sy [expr {$isy<$bsy ? $bsy : $isy}]

       if [info exists tags(b)] {

           set width [lindex $tags(b) 5]
           if {$width == ""} {set width 2}

           set rx [expr $bsx/2 - $width/2]
           set ry [expr $bsy/2 - $width/2]
           set x1 [expr $x - $rx]
           set y1 [expr $y - $ry]
           set x2 [expr $x + $rx]
           set y2 [expr $y + $ry]

           set fill [lindex $tags(b) 3]
           if {$fill == ""} {set fill #8080ff}
           if {$fill == "-"} {set fill ""}
           if {[string index $fill 0]== "@"} {set fill [opp_hsb_to_rgb $fill]}
           set outline [lindex $tags(b) 4]
           if {$outline == ""} {set outline black}
           if {$outline == "-"} {set outline ""}
           if {[string index $outline 0]== "@"} {set outline [opp_hsb_to_rgb $outline]}
           if {$isplaceholder} {set dash "1 1"} else {set dash ""}

           switch -regexp [lindex $tags(b) 2] {
              "o.*"   {set what [list ellipse $x $y -rx $rx -ry $ry]}
              "l.*"   {set what [list pline $x1 $y1 $x2 $y2]}
              default {set what [list prect $x1 $y1 $x2 $y2 -strokelinejoin miter]}
           }

           $c create {*}$what \
               -fill $fill -strokewidth $width -stroke $outline -strokedasharray $dash \
               -tags "dx tooltip submod submodext $submodptr"

           if [info exists tags(i)] {
               $c create pimage $x $y {*}$img -anchor c -tags "dx tooltip submod submodext $submodptr"
           }
           if {$inspectordata($c:showlabels)} {
               $c create ptext $x [expr $y2+$width/2+3] -text $name -textanchor n {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx submodext"
           }

       } else {
           # draw an icon when no shape is present (only i tag, or neither i nor b tag)
           $c create pimage $x $y {*}$img -anchor c -tags "dx tooltip submod submodext $submodptr"
           if {$inspectordata($c:showlabels)} {
               $c create ptext $x [expr $y+$sy/2+3] -text $name -textanchor n {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx submodext"
           }
       }

       # queue length
       if {[info exists tags(q)]} {
           set r [ModuleInspector:getSubmodCoords $c $submodptr]
           set qx [expr [lindex $r 2]+1]
           set qy [lindex $r 1]
           $c create ptext $qx $qy -text "q:?" -textanchor nw {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx tooltip qlen qlen-$submodptr submodext"
       }

       # modifier icon (i2 tag)
       if {[info exists tags(i2)] && [lindex $tags(i2) 0]!=""} {
           if ![info exists tags(is2)] {
               set tags(is2) {}
           }
           set r [ModuleInspector:getSubmodCoords $c $submodptr]
           set mx [expr [lindex $r 2]+2]
           set my [expr [lindex $r 1]-2]
           setvars {img2 dummy dummy} [dispstrGetImage $tags(i2) $tags(is2) $imagesizefactor $alphamult]
           $c create pimage $mx $my {*}$img2 -anchor ne -tags "dx tooltip submod submodext $submodptr"
       }

       # text (t=<text>,<position>,<color>); multiple t tags supported (t1,t2,etc)
       foreach {ttag} [array names tags -regexp {^t\d*$} ] {
           set txt [lindex $tags($ttag) 0]
           set pos [lindex $tags($ttag) 1]
           if {$pos == ""} {set pos "t"}
           set color [lindex $tags($ttag) 2]
           if {$color == ""} {set color "#0000ff"}
           if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}

           set r [ModuleInspector:getSubmodCoords $c $submodptr]
           if {$pos=="l"} {
               set tx [lindex $r 0]
               set ty [lindex $r 1]
               set anch "ne"
               set just "right"
           } elseif {$pos=="r"} {
               set tx [lindex $r 2]
               set ty [lindex $r 1]
               set anch "nw"
               set just "left"
           } elseif {$pos=="t"} {
               set tx [expr ([lindex $r 0]+[lindex $r 2])/2]
               set ty [expr [lindex $r 1]+2]
               set anch "s"
               set just "center"
           } else {
               error "wrong position in t= tag, should be `l', `r' or `t'"
           }
           $c create ptext $tx $ty -text $txt -fill $color -textanchor $anch {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx submodext"
       }

       # r=<radius>,<fillcolor>,<color>,<width>; multiple r tags supported (r1,r2,etc)
       foreach {rtag} [array names tags -regexp {^r\d*$} ] {
           set radius [lindex $tags($rtag) 0]
           if {$radius == ""} {set radius 0}
           set rfill [lindex $tags($rtag) 1]
           if {$rfill == "-"} {set rfill ""}
           if {[string index $rfill 0]== "@"} {set rfill [opp_hsb_to_rgb $rfill]}
           # if rfill=="" --> not filled
           set routline [lindex $tags($rtag) 2]
           if {$routline == ""} {set routline black}
           if {$routline == "-"} {set routline ""}
           if {[string index $routline 0]== "@"} {set routline [opp_hsb_to_rgb $routline]}
           set rwidth [lindex $tags($rtag) 3]
           if {$rwidth == ""} {set rwidth 1}
           if {$zoom != ""} {
               set radius [expr $zoom*$radius]
           }
           set radius [expr $radius-$rwidth/2]

           set x1 [expr $x - $radius]
           set y1 [expr $y - $radius]
           set x2 [expr $x + $radius]
           set y2 [expr $y + $radius]

           set circle [$c create circle $x $y -r $radius -fillopacity 0.5 \
               -fill $rfill -strokewidth $rwidth -stroke $routline -tags "dx range submodext"]
           # has been moved to the beginning of ModuleInspector:drawEnclosingModule to maintain relative z order of range indicators
           # $c lower $circle
       }

   } errmsg] {
       tk_messageBox -type ok -title "Error" -icon error -parent [winfo toplevel [focusOrRoot]] \
                     -message "Error in display string of $name: $errmsg"
   }
}


# ModuleInspector:drawEnclosingModule --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:drawEnclosingModule {c ptr name dispstr} {
   global icons bitmaps inspectordata tkpFont canvasTextOptions
   # puts "DEBUG: ModuleInspector:drawEnclosingModule $c $ptr $name $dispstr"

   set zoom $inspectordata($c:zoomfactor)

   if [catch {

       # lower all range indicators below the icons
       $c lower "range"

       # parse display string. note: we need "1" as last parameter (search
       # for $params in parent module too), because all tags get resolved
       # not only bg* ones.
       opp_displaystring $dispstr parse tags $ptr 1

       # determine top-left origin (bgp tag; currently not supported)
       #if {![info exists tags(bgp)]} {set tags(bgp) {}}
       #set bx [lindex $tags(bgp) 0]
       #set by [lindex $tags(bgp) 1]
       #if {$bx==""} {set bx 0}
       #if {$by==""} {set by 0}
       #if {$zoom != ""} {
       #    set bx [expr $zoom*$bx]
       #    set by [expr $zoom*$by]
       #}
       set bx 0
       set by 0

       # determine size
       if {![info exists tags(bgb)]} {set tags(bgb) {{} {} {}}}
       set sx [lindex $tags(bgb) 0]
       set sy [lindex $tags(bgb) 1]
       if {$zoom != ""} {
           if {$sx!=""} {set sx [expr $zoom*$sx]}
           if {$sy!=""} {set sy [expr $zoom*$sy]}
       }

       if {$sx=="" || $sy==""} {
           set bb [$c bbox submod]
           if {$bb==""} {
               if {$zoom==""} {
                   set bb [list $bx $by 300 200]
               } else {
                   set bb [list $bx $by [expr $zoom*300] [expr $zoom*200]]
               }
           }
           if {$sx==""} {set sx [expr [lindex $bb 2]+[lindex $bb 0]-2*$bx]}
           if {$sy==""} {set sy [expr [lindex $bb 3]+[lindex $bb 1]-2*$by]}
       }

       # determine colors and line width
       set fill [lindex $tags(bgb) 2]
       if {$fill == ""} {set fill grey82}
       if {$fill == "-"} {set fill ""}
       if {[string index $fill 0]== "@"} {set fill [opp_hsb_to_rgb $fill]}
       set outline [lindex $tags(bgb) 3]
       if {$outline == ""} {set outline black}
       if {$outline == "-"} {set outline ""}
       if {[string index $outline 0]== "@"} {set outline [opp_hsb_to_rgb $outline]}
       set width [lindex $tags(bgb) 4]
       if {$width == ""} {set width 2}

       # draw (note: width should grow *outside* the $sx-by-$sy inner rectangle)
       $c create prect [expr $bx-$width/2] [expr $by-$width/2] [expr $bx+$sx+$width/2] [expr $by+$sy+$width/2] \
           -fill $fill -strokewidth $width -stroke $outline -strokelinejoin miter -tags "dx mod $ptr"
       $c create ptext [expr $bx+3] [expr $by+3] -text $name -textanchor nw {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx tooltip modname $ptr"

       # background image
       if {![info exists tags(bgi)]} {set tags(bgi) {}}
       set imgname [lindex $tags(bgi) 0]
       set imgmode [lindex $tags(bgi) 1]
       if {$imgname!=""} {
          if {[catch {set img $bitmaps($imgname)}]} {
              set img $icons(unknown)
          }
          set iwidth [image width $img]
          set iheight [image height $img]
          set isx [expr $iwidth*$zoom]
          set isy [expr $iheight*$zoom]
          if {[string index $imgmode 0]== "c"} {
              # image centered
              set imgx [expr $bx+$sx/2]
              set imgy [expr $by+$sy/2]
              set croppedsx [mathMin $sx $isx]
              set croppedsy [mathMin $sy $isy]
              if {$sx >= $isx} {
                  set isrcx1 0
                  set isrcx2 $iwidth
              } else {
                  set isrcx1 [expr ($isx-$sx)/2/$zoom]
                  set isrcx2 [expr $iwidth - $isrcx1]
              }
              if {$sy >= $isy} {
                  set isrcy1 0
                  set isrcy2 $iheight
              } else {
                  set isrcy1 [expr ($isy-$sy)/2/$zoom]
                  set isrcy2 [expr $iheight - $isrcy1]
              }
              $c create pimage $imgx $imgy -image $img -anchor c -width $croppedsx -height $croppedsy -srcregion [list $isrcx1 $isrcy1 $isrcx2 $isrcy2] -tags "dx mod $ptr"

          } elseif {[string index $imgmode 0]== "s"} {
              # image stretched to fill the background area
              $c create pimage $bx $by -image $img -anchor nw -width $sx -height $sy -tags "dx mod $ptr"
          } elseif {[string index $imgmode 0]== "t"} {
              # image "tile" mode
              set tx [expr $sx / $zoom]
              set ty [expr $sy / $zoom]
              $c create pimage $bx $by -image $img -anchor nw -width $sx -height $sy -srcregion [list 0 0 $tx $ty] -tags "dx mod $ptr"
          } else {
              # default mode: image top-left corner gets aligned to background top-left corner
              # we need cropping
              set croppedsx [mathMin $sx $isx]
              set croppedsy [mathMin $sy $isy]
              set tx [expr $croppedsx / $zoom]
              set ty [expr $croppedsy / $zoom]
              $c create pimage $bx $by -image $img -anchor nw -width $croppedsx -height $croppedsy -srcregion [list 0 0 $tx $ty] -tags "dx mod $ptr"
          }
       }

       # grid display
       if {![info exists tags(bgg)]} {set tags(bgg) {}}
       set gdist [lindex $tags(bgg) 0]
       set gminor [lindex $tags(bgg) 1]
       set gcolor [lindex $tags(bgg) 2]
       if {$gcolor == ""} {set gcolor grey}
       if {$gcolor == "-"} {set gcolor ""}
       if {[string index $gcolor 0]== "@"} {set gcolor [opp_hsb_to_rgb $gcolor]}
       if {$gdist!=""} {
           if {$zoom != ""} {
               set gdist [expr $zoom*$gdist]
           }
           if {$gminor=="" || $gminor < 1} {set gminor 1}
           for {set x $bx} {$x < $bx+$sx} {set x [expr $x+$gdist]} {
               set coords [list $x $by $x [expr $by+$sy]]
               $c create pline $coords -strokewidth 1 -stroke $gcolor -tags "dx mod $ptr"
               # create minor ticks
               set i 1
               for {set minorx [expr int($x+$gdist/$gminor)]} {$i < $gminor && $minorx < $bx+$sx} {
                                             set i [expr $i+1]} {
                   set minorx [expr int($x+$i*$gdist/$gminor)]
                   if {$minorx < $bx+$sx} {
                       set coords [list $minorx $by $minorx [expr $by+$sy]]
                       $c create pline $coords -strokewidth 1 -strokedasharray {2 3} -stroke $gcolor -tags "dx mod $ptr"
                   }
               }
           }
           for {set y $by} {$y < $by+$sy} {set y [expr $y+$gdist]} {
               set coords [list $bx $y [expr $bx+$sx] $y]
               $c create pline $coords -strokewidth 1 -stroke $gcolor -tags "dx mod $ptr"
               # create minor ticks
               set i 1
               for {set minory [expr int($y+$gdist/$gminor)]} {$i < $gminor && $minory < $by+$sy} {
                                             set i [expr $i+1]} {
                   set minory [expr int($y+$i*$gdist/$gminor)]
                   if {$minory < $by+$sy} {
                       set coords [list $bx $minory [expr $bx+$sx] $minory]
                       $c create pline $coords -strokewidth 1 -strokedasharray {2 3} -stroke $gcolor -tags "dx mod $ptr"
                   }
               }
           }
       }

       # text: bgt=<x>,<y>,<text>,<color>; multiple bgt tags supported (bgt1,bgt2,etc)
       foreach {bgttag} [array names tags -regexp {^bgt\d*$} ] {
           set x [lindex $tags($bgttag) 0]
           set y [lindex $tags($bgttag) 1]
           if {$x==""} {set x 0}
           if {$y==""} {set y 0}
           set txt [lindex $tags($bgttag) 2]
           set color [lindex $tags($bgttag) 3]
           if {$color == ""} {set color black}
           if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}
           $c create ptext $x $y -text $txt -fill $color -textanchor nw {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx"
       }

       $c lower mod

   } errmsg] {
       tk_messageBox -type ok -title "Error" -icon error -parent [winfo toplevel [focusOrRoot]] \
                     -message "Error in display string of $name: $errmsg"
   }
}


# ModuleInspector:drawConnection --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:drawConnection {c gateptr dispstr srcptr destptr chanptr src_i src_n dest_i dest_n two_way} {
    global inspectordata tkpFont canvasTextOptions

    # puts "DEBUG: ModuleInspector:drawConnection $c $gateptr $dispstr $srcptr $destptr $src_i $src_n $dest_i $dest_n $two_way"

    if [catch {
       set src_rect [ModuleInspector:getSubmodCoords $c $srcptr]
       set dest_rect [ModuleInspector:getSubmodCoords $c $destptr]
    } errmsg] {
       # skip this connection if source or destination of the arrow cannot be found
       return
    }

    if [catch {

       opp_displaystring $dispstr parse tags $chanptr 1

       if {![info exists tags(m)]} {set tags(m) {a}}

       set mode [lindex $tags(m) 0]
       if {$mode==""} {set mode "a"}
       set src_anch  [list [lindex $tags(m) 1] [lindex $tags(m) 2]]
       set dest_anch [list [lindex $tags(m) 3] [lindex $tags(m) 4]]

       # puts "DEBUG: src_rect=($src_rect) dest_rect=($dest_rect)"
       # puts "DEBUG: src_anch=($src_anch) dest_anch=($dest_anch)"

       regexp -- {^.[^.]*} $c win

       # switch off the connection arrangement if the option is not enabled
       # all connection are treated as the first one in an array with size 1
       if {![opp_getsimoption arrangevectorconnections]} {
           set src_n "1"
           set dest_n "1"
           set src_i "0"
           set dest_i "0"
       }

       set arrow_coords [eval [concat opp_inspectorcommand $win arrowcoords \
                  $src_rect $dest_rect $src_i $src_n $dest_i $dest_n \
                  $mode $src_anch $dest_anch]]

       # puts "DEBUG: arrow=($arrow_coords)"

       if {![info exists tags(ls)]} {set tags(ls) {}}
       set fill [lindex $tags(ls) 0]
       if {$fill == ""} {set fill black}
       if {$fill == "-"} {set fill ""}
       set width [lindex $tags(ls) 1]
       if {$width == ""} {set width 1}
       if {$width == "0"} {set fill ""}
       set style [lindex $tags(ls) 2]
       switch -glob $style {
           "da*"   {set pattern "2 2"}
           "d*"    {set pattern "1 1"}
           default {set pattern ""}
       }

       set state "normal"
       if {$inspectordata($c:showarrowheads) && !$two_way} {
           set arrow {-endarrow 1}
       } else {
           set arrow {}
       }

       $c create pline $arrow_coords {*}$arrow -stroke $fill -strokedasharray $pattern -strokewidth $width -tags "dx tooltip conn submodext $gateptr"

       # if we have a two way connection we should draw only in one direction
       # the other line will be hidden (lowered under anything else)
       if {[string compare $srcptr $destptr] >=0 && $two_way} {
           $c lower $gateptr "dx"
       }

       if {[info exists tags(t)]} {
           set txt [lindex $tags(t) 0]
           set pos [lindex $tags(t) 1]
           if {$pos == ""} {set pos "t"}
           set color [lindex $tags(t) 2]
           if {$color == ""} {set color "#005030"}
           if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}
           set x1 [lindex $arrow_coords 0]
           set y1 [lindex $arrow_coords 1]
           set x2 [lindex $arrow_coords 2]
           set y2 [lindex $arrow_coords 3]
           set anch "c"
           set just "center"
           if {$pos=="l"} {
               # "beginning"
               set x [expr (3*$x1+$x2)/4]
               set y [expr (3*$y1+$y2)/4]
           } elseif {$pos=="r"} {
               # "end"
               set x [expr ($x1+3*$x2)/4]
               set y [expr ($y1+3*$y2)/4]
           } elseif {$pos=="t"} {
               # "middle"
               set x [expr ($x1+$x2)/2]
               set y [expr ($y1+$y2)/2]
               if {($x1==$x2)?($y1<$y2):($x1<$x2)} {set anch "n"} else {set anch "s"}
           } else {
               error "wrong position \"$pos\" in connection t= tag, should be `l', `r' or `t' (for beginning, end, or middle, respectively)"
           }
           $c create ptext $x $y -text $txt -fill $color -textanchor $anch {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx submodext"
       }

    } errmsg] {
       tk_messageBox -type ok -title "Error" -icon error -parent [winfo toplevel [focusOrRoot]] \
                     -message "Error in display string of a connection: $errmsg"
    }
}


# ModuleInspector:drawMessage --
#
# This function is invoked from the message animation code.
#
proc ModuleInspector:drawMessage {c msgptr x y} {
    global inspectordata anim_msg tkpFont canvasTextOptions

    set zoomfactor $inspectordata($c:zoomfactor)
    set imagesizefactor $inspectordata($c:imagesizefactor)

    if {[info exists anim_msg($msgptr:name)]} {
        set msgname $anim_msg($msgptr:name)
        set msgtype $anim_msg($msgptr:type)
        set msgkind $anim_msg($msgptr:kind)
        set dispstr $anim_msg($msgptr:disp)
    } else {
        set msgname [opp_getobjectfullname $msgptr]
        set msgtype [opp_getobjectshorttypename $msgptr]
        set msgkind [opp_getobjectfield $msgptr kind]
        set dispstr [opp_getobjectfield $msgptr displayString]
    }

    # The following lines were used for testing only
    #set dispstr "b=15,15,rect,white,kind,5"
    #set dispstr "b="
    #set dispstr "b=,,,kind"
    #set dispstr "b=15,15,oval,yellow,green,6"
    #set dispstr "i=handset2_s"

    if {$dispstr=="" && [opp_getsimoption penguin_mode]} {
        set dispstr "i=old/penguin"
    }

    if {$dispstr==""} {

        # default presentation: red or msgkind%8-colored ball
        if [opp_getsimoption animation_msgcolors] {
            set color [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
        } else {
            set color red
        }
        set ball [$c create circle 0 0 -r 5 -fill $color -stroke $color -tags "dx tooltip msg $msgptr"]
        $c move $ball $x $y

        set labelx $x
        set labely $y

    } else {
        # use display string

        # supports "b","i" and "o" tags, they work just as with submodules only default
        # is different (small red ball), plus special color "kind" is supported which
        # gives the original, message kind dependent colors
        opp_displaystring $dispstr parse tags [opp_null] 1

        # set sx and sy
        if ![info exists tags(is)] {
            set tags(is) {}
        }
        if [info exists tags(i)] {
            if {[lindex $tags(i) 1] == "kind"} {
                set kindcolor [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
                set tags(i) [lreplace $tags(i) 1 1 $kindcolor]
            }
            setvars {img sx sy} [dispstrGetImage $tags(i) $tags(is) $imagesizefactor]
        } elseif [info exists tags(b)] {
            set sx [lindex $tags(b) 0]
            if {$sx==""} {set sx 10}
            set sy [lindex $tags(b) 1]
            if {$sy==""} {set sy $sx}
        } else {
            set tags(b) {10 10 oval}
            set sx 10
            set sy 10
        }

        if [info exists tags(i)] {

            $c create pimage $x $y {*}$img -anchor c -tags "dx tooltip msg $msgptr"

            set labelx $x
            set labely [expr $y+$sy/2+3]

        } elseif [info exists tags(b)] {

            set rx [expr $sx/2]
            set ry [expr $sy/2]
            set x1 [expr $x - $rx]
            set y1 [expr $y - $ry]
            set x2 [expr $x + $rx]
            set y2 [expr $y + $ry]

            set fill [lindex $tags(b) 3]
            if {$fill == ""} {set fill red}
            if {$fill == "kind"} {
                set fill [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
            }
            set outline [lindex $tags(b) 4]
            if {$outline == ""} {set outline ""}
            if {$outline == "kind"} {
                set outline [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
            }
            set width [lindex $tags(b) 5]
            if {$width == ""} {set width 1}

            switch -regexp [lindex $tags(b) 2] {
               "p.*"   {set what [list prect $x1 $y1 $x2 $y2 -strokelinejoin miter]}
               default {set what [list ellipse $x $y -rx $rx -ry $ry]}
            }

            $c create {*}$what -fill $fill -strokewidth $width -stroke $outline -tags "dx tooltip msg $msgptr"

            set labelx $x
            set labely [expr $y2+$width/2+3]
        }
    }

    # display message label: "(classname)name"
    set msglabel ""
    if [opp_getsimoption animation_msgclassnames] {
        set msglabel "($msgtype)"
    }
    if [opp_getsimoption animation_msgnames] {
        append msglabel $msgname
    }
    if {$msglabel!=""} {
        $c create ptext $labelx $labely -text $msglabel -textanchor n {*}$tkpFont(CanvasFont) {*}$canvasTextOptions -tags "dx msgname $msgptr"
    }

}

proc mathMin {a b} {
    return [expr ($a < $b) ? $a : $b]
}

proc mathMax {a b} {
    return [expr ($a > $b) ? $a : $b]
}

proc ModuleInspector:zoomIn {insp {x ""} {y ""}} {
    global config
    ModuleInspector:zoomBy $insp $config(zoomby-factor) 1 $x $y
}

proc ModuleInspector:zoomOut {insp {x ""} {y ""}} {
    global config
    ModuleInspector:zoomBy $insp [expr 1.0 / $config(zoomby-factor)] 1 $x $y
}

proc ModuleInspector:zoomBy {insp mult {snaptoone 0}  {x ""} {y ""}} {
    global inspectordata
    set c $insp.c
    if {($mult<1 && $inspectordata($c:zoomfactor)>0.001) || ($mult>1 && $inspectordata($c:zoomfactor)<1000)} {
        # remember canvas scroll position, we'll need it to zoom in/out around ($x,$y)
        if {$x == ""} {set x [expr [winfo width $c] / 2]}
        if {$y == ""} {set y [expr [winfo height $c] / 2]}
        set origCanvasX [$c canvasx $x]
        set origCanvasY [$c canvasy $y]
        set origZoom $inspectordata($c:zoomfactor)

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

        # clear scrollregion so that it will be set up afresh
        $c config -scrollregion ""

        # redraw
        opp_inspectorcommand $insp redraw
        ModuleInspector:setScrollRegion $insp 0

        ModuleInspector:updateZoomLabel $insp

        # pan the canvas so that we zoom in/out around ($x, $y)
        set actualMult [expr $inspectordata($c:zoomfactor) / $origZoom]
        set canvasX [expr $origCanvasX * $actualMult]
        set canvasY [expr $origCanvasY * $actualMult]
        set dx [expr int($canvasX - $origCanvasX)]
        set dy [expr int($canvasY - $origCanvasY)]
        $c scan mark 0 0
        $c scan dragto $dx $dy -1
    }

    ModuleInspector:updatePreferences $insp

    ModuleInspector:popOutToolbarButtons $insp
}

proc ModuleInspector:updateZoomLabel {insp} {
    global inspectordata
    set c $insp.c
    set value [format "%.2f" $inspectordata($c:zoomfactor)]
    $insp.zoominfo config -text "Zoom: ${value}x"
}

proc ModuleInspector:popOutToolbarButtons {insp} {
    # in Run or Fast mode with dynamic module creation, toolbar buttons may get stuck
    # after clicking in "sunken" or "raised" state instead of returning to "flat",
    # likely because events get lost during the grab during incremental layouting.
    # No idea how this can be fixed properly.
    # This is a weak attempt to fix it for the most commonly clicked buttons.
    # This could be called from many more places for better results.
    if [opp_inspector_istoplevel $insp] {
        $insp.toolbar.parent config -relief flat
        $insp.toolbar.stop config -relief flat
        $insp.toolbar.redraw config -relief flat
        $insp.toolbar.zoomin config -relief flat
        $insp.toolbar.zoomout config -relief flat
    } else {
        #TODO list actual buttons here
        $insp.toolbar.stop config -relief flat
        $insp.toolbar.redraw config -relief flat
        $insp.toolbar.zoomin config -relief flat
        $insp.toolbar.zoomout config -relief flat

    }
}

proc ModuleInspector:zoomIconsBy {insp mult} {
    global inspectordata
    set c $insp.c
    if {($mult<1 && $inspectordata($c:imagesizefactor)>0.1) || ($mult>1 && $inspectordata($c:imagesizefactor)<5)} {
        set inspectordata($c:imagesizefactor) [expr $inspectordata($c:imagesizefactor) * $mult]
        if {abs($inspectordata($c:imagesizefactor)-1.0) < 0.1} {set inspectordata($c:imagesizefactor) 1}
        opp_inspectorcommand $insp redraw
    }

    ModuleInspector:updatePreferences $insp
}

proc ModuleInspector:toggleLabels {insp} {
    global inspectordata
    set c $insp.c
    set inspectordata($c:showlabels) [expr !$inspectordata($c:showlabels)]
    opp_inspectorcommand $insp redraw

    ModuleInspector:updatePreferences $insp
}

proc ModuleInspector:toggleArrowheads {insp} {
    global inspectordata
    set c $insp.c
    set inspectordata($c:showarrowheads) [expr !$inspectordata($c:showarrowheads)]
    opp_inspectorcommand $insp redraw

    ModuleInspector:updatePreferences $insp
}

proc ModuleInspector:click insp {
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

proc ModuleInspector:dblClick insp {
   set c $insp.c
   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }

   if {$ptr!=""} {
      inspector:dblClick $insp $ptr
   }
}

# get the pointers of all objects under the mouse. If more than 1 ptr is returned
# then bgrptr is removed from the list. x and y must be widget relative coordinate
# (of the canvas object). The background module pointer is removed automatically
# if more that 1 pointer is present. I.e. background is returned ONLY if the mouse
# is directly over the background module.
proc ModuleInspector:getPtrsUnderMouse {c x y} {
   set ptrs {}
   # convert widget coordinates to canvas coordinates
   set x [$c canvasx $x]
   set y [$c canvasy $y]
   set items [$c find overlapping [expr $x-2] [expr $y-2] [expr $x+2] [expr $y+2]]
   foreach item $items {
       set tags [$c gettags $item]
       foreach tag $tags {
           if [string match "ptr*" $tag] {
               lappend ptrs $tag
           }
       }
   }

   set ptrs2 {}
   if {$ptrs != {}} {
      # remove duplicate pointers and reverse the order
      # so the topmost element will be the first in the list
      set ptrs2 [lreverse [uniq $ptrs]]

      set bgptr ""
      regexp {\.(ptr.*)-([0-9]+)} $c match bgptr dummy
      # if more than one ptr present delete the background module's pointer
      if { [llength $ptrs2] > 1 && $bgptr != "" } {
          set bgindex [lsearch $ptrs2 $bgptr]
          if { $bgindex >= 0 } {
              set ptrs2 [lreplace $ptrs2 $bgindex $bgindex]
          }
      }
   }
   return $ptrs2
}

proc ModuleInspector:rightClick {insp X Y x y} {
   global inspectordata tmp CTRL

   ModuleInspector:zoomMarqueeCancel $insp ;# just in case

   set c $insp.c
   set ptrs [ModuleInspector:getPtrsUnderMouse $c $x $y]

   if {$ptrs != {}} {

      set popup [createInspectorContextMenu $insp $ptrs]

      set tmp($c:showlabels) $inspectordata($c:showlabels)
      set tmp($c:showarrowheads) $inspectordata($c:showarrowheads)

      $popup add separator
      $popup add command     -label "Show/Hide Canvas Layers..." -command "ModuleInspector:layers $insp"

      $popup add separator
      $popup add checkbutton -label "Show Module Names" -command "ModuleInspector:toggleLabels $insp" -accel "$CTRL+L" -variable tmp($c:showlabels)
      $popup add checkbutton -label "Show Arrowheads" -command "ModuleInspector:toggleArrowheads $insp" -accel "$CTRL+A" -variable tmp($c:showarrowheads)

      $popup add separator
      $popup add command -label "Increase Icon Size" -accel "$CTRL+I" -command "ModuleInspector:zoomIconsBy $insp 1.25"
      $popup add command -label "Decrease Icon Size" -accel "$CTRL+O" -command "ModuleInspector:zoomIconsBy $insp 0.8"

      $popup add separator
      $popup add command -label "Zoom In"  -accel "$CTRL+M" -command "ModuleInspector:zoomIn $insp"
      $popup add command -label "Zoom Out" -accel "$CTRL+N" -command "ModuleInspector:zoomOut $insp"
      $popup add command -label "Re-Layout" -accel "$CTRL+R" -command "opp_inspectorcommand $insp relayout"

      $popup add separator
      $popup add command -label "Layouting Settings..." -command "preferencesDialog $insp l"
      $popup add command -label "Animation Settings..." -command "preferencesDialog $insp a"
      $popup add command -label "Animation Filter..." -command "preferencesDialog $insp t"

      tk_popup $popup $X $Y
   }
}

# ModuleInspector:relayout --
#
# Relayout the compound module, and resize the window accordingly.
#
proc ModuleInspector:relayout {insp} {
    global config inspectordata

    set c $insp.c
    incr inspectordata($c:layoutseed)
    opp_inspectorcommand $insp setlayoutseed $inspectordata($c:layoutseed)

    opp_inspectorcommand $insp relayout

    if {[opp_inspector_istoplevel $insp] && $config(layout-may-resize-window)} {
        wm geometry $insp ""
    }

    ModuleInspector:adjustWindowSizeAndZoom $insp
}

# ModuleInspector:drawMessageOnGate --
#
# Draw message near the head of the connection arrow.
# Called from inspector C++ code.
#
proc ModuleInspector:drawMessageOnGate {c gateptr msgptr} {
    #debug "ModuleInspector:drawMessageOnGate $msgptr"

    # gate pointer + conn are the tags of the connection arrow
    set conn_id ""
    foreach id [$c find withtag $gateptr] {
       if {[lsearch -exact [$c gettags $id] "conn"] != -1} {
          set conn_id $id
       }
    }

    if {$conn_id == ""} {
        # connection arrow not (no longer?) on canvas: forget animation
        $c delete $msgptr;  # this also works if msg is not (yet) on canvas
        return;
    }

    set coords [$c coords $conn_id]
    setvars {x1 y1 x2 y2} $coords
    set endpos [ModuleInspector:getMessageEndPos $x1 $y1 $x2 $y2]
    setvars {xx yy} $endpos

    ModuleInspector:drawMessage $c $msgptr $xx $yy
}

#
# Calculates the position where a sent message ball should rest until its event
# comes and it gets processed by the module
#
proc ModuleInspector:getMessageEndPos {x1 y1 x2 y2} {
    set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]
    if {$len==0} {set len 1}
    set dx [expr ($x2-$x1)/$len]
    set dy [expr ($y2-$y1)/$len]

    set len2 [expr $len - 6]
    if {$len2 < 1} {set len2 1}
    set xx [expr $x1+$dx*$len2]
    set yy [expr $y1+$dy*$len2]
    return [list $xx $yy]
}

# ModuleInspector:drawMessageOnModule --
#
# Draw message on submodule rectangle.
# Called from inspector C++ code.
#
proc ModuleInspector:drawMessageOnModule {c modptr msgptr} {

    #debug "ModuleInspector:drawMessageOnModule $msgptr"
    set r  [ModuleInspector:getSubmodCoords $c $modptr]
    set x [expr ([lindex $r 0]+[lindex $r 2])/2]
    set y [expr ([lindex $r 1]+[lindex $r 3])/2]

    ModuleInspector:drawMessage $c $msgptr $x $y
}

# ModuleInspector:drawNextEventMarker --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:drawNextEventMarker {c modptr type} {
    set src  [ModuleInspector:getSubmodCoords $c $modptr]
    set x1 [expr [lindex $src 0]-2]
    set y1 [expr [lindex $src 1]-2]
    set x2 [expr [lindex $src 2]+2]
    set y2 [expr [lindex $src 3]+2]
    # $type==1 compound module, $type==2 simple module
    if {$type==1} {
        $c create prect $x1 $y1 $x2 $y2 -tags {nexteventmarker} -stroke red -strokewidth 1
    } else {
        $c create prect $x1 $y1 $x2 $y2 -tags {nexteventmarker} -stroke red -strokewidth 2
    }
}

# ModuleInspector:refreshSubmodule --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:refreshSubmodule {insp modptr} {
    # currently the only thing to be updated is the number of elements in queue
    set c $insp.c
    set dispstr [opp_getobjectfield $modptr displayString]
    set qname [opp_displaystring $dispstr getTagArg "q" 0 $modptr 1]
    if {$qname!=""} {
        #set qptr [opp_inspectorcommand $win getsubmodq $modptr $qname]
        #set qlen [opp_getobjectfield $qptr length]
        # TBD optimize -- maybe store and remember q pointer?
        set qlen [opp_inspectorcommand $insp getsubmodqlen $modptr $qname]
        $c itemconfig "qlen-$modptr" -text "q:$qlen"
    }
}

#
# Helper proc.
#
proc ModuleInspector:qlenGetQptrCurrent {insp} {
   set c $insp.c
   set item [$c find withtag current]
   set tags [$c gettags $item]

   set modptr ""
   if {[lsearch $tags "qlen-ptr*"] != -1} {
       regexp "ptr.*" $tags modptr
   }
   if {$modptr==""} {return}

   return [ModuleInspector:qlenGetQptr $insp $modptr]

}

proc ModuleInspector:qlenGetQptr {insp modptr} {
   set dispstr [opp_getobjectfield $modptr displayString]
   set qname [opp_displaystring $dispstr getTagArg "q" 0 $modptr 1]
   if {$qname!=""} {
       set qptr [opp_inspectorcommand $insp getsubmodq $modptr $qname]
       return $qptr
   }
   return ""
}

proc ModuleInspector:qlenClick insp {
   set qptr [ModuleInspector:qlenGetQptrCurrent $insp]
   if [opp_isnotnull $qptr] {
       mainWindow:selectionChanged $insp $qptr
   }
}

proc ModuleInspector:qlenDblclick insp {
   set qptr [ModuleInspector:qlenGetQptrCurrent $insp]
   if [opp_isnotnull $qptr] {
       opp_inspect $qptr
   }
}

proc ModuleInspector:qlenRightClick {insp X Y x y} {
   set qptr [ModuleInspector:qlenGetQptrCurrent $insp]
   if [opp_isnotnull $qptr] {
       set popup [createInspectorContextMenu $insp $qptr]
       tk_popup $popup $X $Y
   }
}

# ModuleInspector:bubble --
#
# This function is invoked from the module inspector C++ code.
#
proc ModuleInspector:bubble {c x y txt} {
    global inspectordata tkpFont

    set zoom $inspectordata($c:zoomfactor)

    set x [expr $x*$zoom]
    set y [expr $y*$zoom]

    while {[string length $txt]<5} {set txt " $txt "}
    set txtid  [$c create ptext $x $y -text " $txt " -textanchor c {*}$tkpFont(CanvasFont) -tags "bubble"]
    set color #F8F8D8
    set bb [$c bbox $txtid]

    set x1 [lindex $bb 0]
    set y1 [lindex $bb 1]
    set x2 [lindex $bb 2]
    set y2 [lindex $bb 3]

    set x1o [expr $x1-2]
    set y1o [expr $y1-2]
    set x2o [expr $x2+2]
    set y2o [expr $y2+2]

    set xm [expr ($x1+$x2)/2]
    set ym [expr ($y1+$y2)/2]
    set xme [expr $xm-10]
    set yme [expr $y2o+15]

    set pp [list $x1o $y1  \
                 $x1  $y1o \
                 $xm  $y1o \
                 $xm  $y1o \
                 $x2  $y1o \
                 $x2o $y1  \
                 $x2o $ym  \
                 $x2o $ym  \
                 $x2o $y2  \
                 $x2  $y2o \
                 $xm  $y2o \
                 $xm  $y2o \
                 \
                 $xme $yme \
                 $xme $yme \
                 $xme $y2o \
                 $xme $y2o \
                 \
                 $x1  $y2o \
                 $x1o $y2  \
                 $x1o $ym  \
                 $x1o $ym ]

    set bubbleid [$c create ppolygon $pp -stroke black -fill $color -strokewidth 1 -tags "bubble"]
    $c lower $bubbleid $txtid

    set dx [expr $x-$xme]
    set dy [expr $y-$yme]

    $c move $bubbleid $dx $dy
    $c move $txtid $dx $dy

    set sp [opp_getsimoption animation_speed]
    set ad [expr int(1000 / (0.1+$sp))]
    after $ad [list catch [list $c delete $txtid $bubbleid]]
}


