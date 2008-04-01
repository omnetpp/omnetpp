#=================================================================
#  MODINSP2.TCL - part of
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

#-----------------------------------------------------------------
#  Graphical compound module window stuff
#-----------------------------------------------------------------

#
# Note: tooltips on canvas come from the proc whose name is stored in
# $help_tips(helptip_proc). This is currently get_help_tip.
#

# get_parsed_display_string --
#
# Return parsed display string in an array.
#    str:     display string
#    array:   dest array name
#    w:       inspector window name
#    modptr:  pointer of module whose parameters should be used for "$x" style
#             parameters in the display string
#    parent:  if nonzero, parameter is searched in the parent module too;
#             otherwise, only that very module is considered
# Example:
#   with "p=50,$y_pos;i=cloud", and if "y_pos" module parameter is 99, the result:
#      $a(p) = {50 99}
#      $a(i) = {cloud}
#
proc get_parsed_display_string {str array w modptr parent} {
   upvar $array arr

   opp_displaystring $str parse arr

   foreach key [array names arr] {
      set i 0
      foreach v $arr($key) {
         if {[string range $v 0 0]=={$}} {
            if {$modptr==""} {error "Cannot substitute parameters into this display string"}
            set v [opp_inspectorcommand $w dispstrpar $modptr [string range $v 1 end] $parent]
            set arr($key) [lreplace $arr($key) $i $i $v]
         }
         incr i
      }
   }
}

proc lookup_image {imgname {imgsize ""}} {
    global bitmaps icons

    if {[catch {set img $bitmaps($imgname,$imgsize)}] && \
        [catch {set img $bitmaps($imgname)}] && \
        [catch {set img $bitmaps(old/$imgname,$imgsize)}] && \
        [catch {set img $bitmaps(old/$imgname)}]} {
       set img $icons(unknown)
    }

    return $img
}


#
# helper function
#
proc dispstr_getimage {tags_i tags_is zoomfactor} {
    global icons bitmaps imagecache

    set key "[join $tags_i ,]:[join $tags_is ,]:$zoomfactor"
    if {![info exist imagecache($key)]} {
        # look up base image
        set imgsize [lindex $tags_is 0]
        if {$imgsize==""} {set imgsize "n"}
        set imgname [lindex $tags_i 0]
        if {$imgname=="" || ([catch {set img $bitmaps($imgname,$imgsize)}] && \
                             [catch {set img $bitmaps($imgname)}] && \
                             [catch {set img $bitmaps(old/$imgname,$imgsize)}] && \
                             [catch {set img $bitmaps(old/$imgname)}])} {
            set img $icons(unknown)
        }
        if {[catch {image type $img}]} {
            error "internal error: image referred to in bitmaps() doesn't exist"
        }

        # colorize if needed
        if {[llength $tags_i]>1} {
            # check destcolor and weight for icon colorizing
            # if destcolor=="", don't colorize at all
            set destc [lindex $tags_i 1]
            set cweight [lindex $tags_i 2]
            if {$destc!=""} {
                if {[string index $destc 0]== "@"} {set destc [opp_hsb_to_rgb $destc]}
                if {$cweight==""} {set cweight 30}

                set img2 [image create photo]
                $img2 copy $img
                opp_colorizeimage $img2 $destc $cweight
                set img $img2
            }
        }

        # rescale if needed
        if {$zoomfactor!=1} {
            set isx [image width $img]
            set isy [image height $img]
            set newisx [expr int($zoomfactor * $isx)]
            set newisy [expr int($zoomfactor * $isy)]
            if {$newisx<1} {set newisx 1}
            if {$newisy<1} {set newisy 1}
            if {$newisx>500 || $newisy>500} {
                set img $icons(imagetoobig)
            } else {
                set img [resizeimage $img $newisx $newisy]
            }
        }

        set imagecache($key) $img
    }
    return $imagecache($key)
}

#
# helper function
#
proc get_submod_coords {c tag} {

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


# draw_submod --
#
# This function is invoked from the module inspector C++ code.
#
proc draw_submod {c submodptr x y name dispstr scaling} {
   #puts "DEBUG: draw_submod $c $submodptr $x $y $name $dispstr $scaling"
   global icons inspectordata

   set zoomfactor $inspectordata($c:zoomfactor)
   if {$scaling!="" || $zoomfactor!=1} {
       if {$scaling==""} {set scaling 1.0}
       set scaling [expr $scaling*$zoomfactor]
   }

   if [catch {
       get_parsed_display_string $dispstr tags [winfo toplevel $c] $submodptr 1

       # scale (x,y)
       if {$scaling != ""} {
           set x [expr $scaling*$x]
           set y [expr $scaling*$y]
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
           set img [dispstr_getimage $tags(i) $tags(is) $zoomfactor]
           set isx [image width $img]
           set isy [image height $img]
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
           if {$scaling != ""} {
               set bsx [expr $scaling*$bsx]
               set bsy [expr $scaling*$bsy]
           }
       } elseif ![info exists tags(i)] {
           set img $icons(defaulticon)
           set isx [image width $img]
           set isy [image height $img]
       }

       set sx [expr {$isx<$bsx ? $bsx : $isx}]
       set sy [expr {$isy<$bsy ? $bsy : $isy}]

       if [info exists tags(b)] {

           set width [lindex $tags(b) 5]
           if {$width == ""} {set width 2}

           set x1 [expr $x - $bsx/2 + $width/2]
           set y1 [expr $y - $bsy/2 + $width/2]
           set x2 [expr $x + $bsx/2 - $width/2]
           set y2 [expr $y + $bsy/2 - $width/2]

           set sh [lindex $tags(b) 2]
           if {$sh == ""} {set sh rect}

           set fill [lindex $tags(b) 3]
           if {$fill == ""} {set fill #8080ff}
           if {$fill == "-"} {set fill ""}
           if {[string index $fill 0]== "@"} {set fill [opp_hsb_to_rgb $fill]}
           set outline [lindex $tags(b) 4]
           if {$outline == ""} {set outline black}
           if {$outline == "-"} {set outline ""}
           if {[string index $outline 0]== "@"} {set outline [opp_hsb_to_rgb $outline]}

           $c create $sh $x1 $y1 $x2 $y2 \
               -fill $fill -width $width -outline $outline \
               -tags "dx tooltip submod $submodptr"

           if [info exists tags(i)] {
               $c create image $x $y -image $img -anchor center -tags "dx tooltip submod $submodptr"
           }
           if {$inspectordata($c:showlabels)} {
               $c create text $x [expr $y2+$width/2+3] -text $name -anchor n -tags "dx"
           }

       } else {
           # draw an icon when no shape is present (only i tag, or neither i nor b tag)
           $c create image $x $y -image $img -anchor center -tags "dx tooltip submod $submodptr"
           if {$inspectordata($c:showlabels)} {
               $c create text $x [expr $y+$sy/2+3] -text $name -anchor n -tags "dx"
           }
       }

       # queue length
       if {[info exists tags(q)]} {
           set r [get_submod_coords $c $submodptr]
           set qx [expr [lindex $r 2]+1]
           set qy [lindex $r 1]
           $c create text $qx $qy -text "q:?" -anchor nw -tags "dx tooltip qlen qlen-$submodptr"
       }

       # modifier icon (i2 tag)
       if {[info exists tags(i2)]} {
           if ![info exists tags(is2)] {
               set tags(is2) {}
           }
           set r [get_submod_coords $c $submodptr]
           set mx [expr [lindex $r 2]+2]
           set my [expr [lindex $r 1]-2]
           set img2 [dispstr_getimage $tags(i2) $tags(is2) $zoomfactor]
           $c create image $mx $my -image $img2 -anchor ne -tags "dx tooltip submod $submodptr"
       }

       # text (t=<text>,<position>,<color>); multiple t tags supported (t1,t2,etc)
       foreach {ttag} [array names tags -regexp {^t\d*$} ] {
           set txt [lindex $tags($ttag) 0]
           set pos [lindex $tags($ttag) 1]
           if {$pos == ""} {set pos "t"}
           set color [lindex $tags($ttag) 2]
           if {$color == ""} {set color "#0000ff"}
           if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}

           set r [get_submod_coords $c $submodptr]
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
           $c create text $tx $ty -text $txt -fill $color -anchor $anch -justify $just -tags "dx"
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
           if {$scaling != ""} {
               set radius [expr $scaling*$radius]
           }
           set radius [expr $radius-$rwidth/2]

           set x1 [expr $x - $radius]
           set y1 [expr $y - $radius]
           set x2 [expr $x + $radius]
           set y2 [expr $y + $radius]

           set circle [$c create oval $x1 $y1 $x2 $y2 \
               -fill $rfill -width $rwidth -outline $routline -tags "dx range"]
           # has been moved to the beginning of draw_enclosingmod to maintain relative z order of range indicators
           # $c lower $circle
       }

   } errmsg] {
       tk_messageBox -type ok -title Error -icon error -parent [winfo toplevel [focus]] \
                     -message "Error in display string of $name: $errmsg"
   }
}


# draw_enclosingmod --
#
# This function is invoked from the module inspector C++ code.
#
proc draw_enclosingmod {c ptr name dispstr scaling} {
   global icons bitmaps inspectordata
   # puts "DEBUG: draw_enclosingmod $c $ptr $name $dispstr $scaling"

   set zoomfactor $inspectordata($c:zoomfactor)
   if {$scaling!="" || $zoomfactor!=1} {
       if {$scaling==""} {set scaling 1.0}
       set scaling [expr $scaling*$zoomfactor]
   }

   if [catch {

       # lower all range indicators below the icons
       $c lower "range"

       # parse display string. note: we need "1" as last parameter (search
       # for $params in parent module too), because all tags get resolved
       # not only bg* ones.
       get_parsed_display_string $dispstr tags [winfo toplevel $c] $ptr 1

       # determine top-left origin
       if {![info exists tags(bgp)]} {set tags(bgp) {}}
       set bx [lindex $tags(bgp) 0]
       set by [lindex $tags(bgp) 1]
       if {$bx==""} {set bx 0}
       if {$by==""} {set by 0}
       if {$scaling != ""} {
           set bx [expr $scaling*$bx]
           set by [expr $scaling*$by]
       }

       # determine size
       if {![info exists tags(bgb)]} {set tags(bgb) {{} {} {}}}
       set sx [lindex $tags(bgb) 0]
       set sy [lindex $tags(bgb) 1]
       if {$scaling != ""} {
           if {$sx!=""} {set sx [expr $scaling*$sx]}
           if {$sy!=""} {set sy [expr $scaling*$sy]}
       }

       if {$sx=="" || $sy==""} {
           set bb [$c bbox submod]
           if {$bb==""} {
               if {$scaling==""} {
                   set bb [list $bx $by 300 200]
               } else {
                   set bb [list $bx $by [expr $scaling*300] [expr $scaling*200]]
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
       $c create rect [expr $bx-$width/2] [expr $by-$width/2] [expr $bx+$sx+$width/2] [expr $by+$sy+$width/2] \
           -fill $fill -width $width -outline $outline \
           -tags "dx mod $ptr"
       $c create text [expr $bx+3] [expr $by+3] -text $name -anchor nw -tags "dx tooltip modname $ptr"

       # background image
       if {![info exists tags(bgi)]} {set tags(bgi) {}}
       set imgname [lindex $tags(bgi) 0]
       set imgmode [lindex $tags(bgi) 1]
       if {$imgname!=""} {
          if {[catch {set img $bitmaps($imgname)}] && \
              [catch {set img $bitmaps(old/$imgname)}]} {
              set img $icons(unknown)
          }
          set isx [expr [image width $img]*$zoomfactor]
          set isy [expr [image height $img]*$zoomfactor]
          set imgx $bx
          set imgy $by
          set anchor nw
          if {[string index $imgmode 0]== "c"} {
              # image centered
              set imgx [expr $bx+$sx/2]
              set imgy [expr $by+$sy/2]
              set anchor center
              if {$sx < $isx || $sy < $isy} {
                 # image must be clipped. a new image created with new dimensions
                 if {$sx < $isx} {set minx $sx} else {set minx $isx}
                 if {$sy < $isy} {set miny $sy} else {set miny $isy}
                 set img [get_cached_image $img $zoomfactor [expr ($isx-$minx)/2] [expr ($isy-$miny)/2] [expr ($isx+$minx)/2] [expr ($isy+$miny)/2] $minx $miny 0]
              }
          } elseif {[string index $imgmode 0]== "s"} {
              # image stretched to fill the background area
              set img [get_cached_image $img $zoomfactor 0 0 $isx $isy $sx $sy 1]
          } elseif {[string index $imgmode 0]== "t"} {
              # image "tile" mode
              set img [get_cached_image $img $zoomfactor 0 0 $isx $isy $sx $sy 0]
          } else {
              # default mode: image top-left corner gets aligned to background top-left corner
              if {$sx < $isx || $sy < $isy || $zoomfactor != 1} {
                 # image must be cropped
                 if {$sx < $isx} {set minx $sx} else {set minx $isx}
                 if {$sy < $isy} {set miny $sy} else {set miny $isy}
                 set img [get_cached_image $img $zoomfactor 0 0 $minx $miny $minx $miny 0]
              }
          }
          $c create image $imgx $imgy -image $img -anchor $anchor -tags "dx mod $ptr"
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
           if {$scaling != ""} {
               set gdist [expr $scaling*$gdist]
           }
           if {$gminor=="" || $gminor < 1} {set gminor 1}
           for {set x $bx} {$x < $bx+$sx} {set x [expr $x+$gdist]} {
               set coords [list $x $by $x [expr $by+$sy]]
               $c create line $coords -width 1 -fill $gcolor -tags "dx mod $ptr"
               # create minor ticks
               set i 1
               for {set minorx [expr int($x+$gdist/$gminor)]} {$i < $gminor && $minorx < $bx+$sx} {
                                             set i [expr $i+1]} {
                   set minorx [expr int($x+$i*$gdist/$gminor)]
                   if {$minorx < $bx+$sx} {
                       set coords [list $minorx $by $minorx [expr $by+$sy]]
                       $c create line $coords -width 1 -dash . -fill $gcolor -tags "dx mod $ptr"
                   }
               }
           }
           for {set y $by} {$y < $by+$sy} {set y [expr $y+$gdist]} {
               set coords [list $bx $y [expr $bx+$sx] $y]
               $c create line $coords -width 1 -fill $gcolor -tags "dx mod $ptr"
               # create minor ticks
               set i 1
               for {set minory [expr int($y+$gdist/$gminor)]} {$i < $gminor && $minory < $by+$sy} {
                                             set i [expr $i+1]} {
                   set minory [expr int($y+$i*$gdist/$gminor)]
                   if {$minory < $by+$sy} {
                       set coords [list $bx $minory [expr $bx+$sx] $minory]
                       $c create line $coords -width 1 -dash . -fill $gcolor -tags "dx mod $ptr"
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
           if {$scaling != ""} {
               set x [expr $scaling*$x]
               set y [expr $scaling*$y]
           }
           set txt [lindex $tags($bgttag) 2]
           set color [lindex $tags($bgttag) 3]
           if {$color == ""} {set color black}
           if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}
           $c create text $x $y -text $txt -fill $color -anchor nw -justify left -tags "dx"
       }

       $c lower mod

       # scrolling region
       set bb [$c bbox all]
       $c config -scrollregion [list [expr [lindex $bb 0]-10] [expr [lindex $bb 1]-10] \
                                     [expr [lindex $bb 2]+10] [expr [lindex $bb 3]+10]]


   } errmsg] {
       tk_messageBox -type ok -title Error -icon error -parent [winfo toplevel [focus]] \
                     -message "Error in display string of $name: $errmsg"
   }
}


# get_cached_image --
#
# Performs the following steps:
#  - first zooms the image by zoomfactor
#  - then takes the area (x1,y1,x2,y2) in the new (zoomed) coordinate system
#  - then either stretches or tiles it to (targetWidth,targetHeight) size
#  - result gets cached and returned
# NOTE:  (x1,y1,x2,y2) cliprect does NOT WORK for stretch mode! always the
# full image will be streched to the (targetWidth,targetHeight) size
#
proc get_cached_image {img zoomfactor x1 y1 x2 y2 targetWidth targetHeight doStretch} {
    global icons img_cache

    set x1 [expr int($x1)]
    set y1 [expr int($y1)]
    set x2 [expr int($x2)]
    set y2 [expr int($y2)]
    set targetWidth [expr int($targetWidth)]
    set targetHeight [expr int($targetHeight)]
    if {$targetWidth<1} {set targetWidth 1}
    if {$targetHeight<1} {set targetHeight 1}
    if {$targetWidth>2500 || $targetHeight>2000} {return $icons(imagetoobig)}

    set key "$img:$zoomfactor:$x1:$y1:$x2:$y2:$targetWidth:$targetHeight:$doStretch"

    if {![info exists img_cache($key)]} {
        if {!$doStretch} {
            # "tile" mode: implementation relies on Tk "image copy" command's behavior
            # to tile the image if dest area is larger than source area
            # NOTE: "image copy" is incredibly slow! need to reimplement it ourselves in C++!
            if {$zoomfactor!=1} {
                set zoomedisx [expr int([image width $img]*$zoomfactor)]
                set zoomedisy [expr int([image height $img]*$zoomfactor)]
                set img [resizeimage $img $zoomedisx $zoomedisy]
            }
            set newimg [image create photo -width $targetWidth -height $targetHeight]
            $newimg copy $img -from $x1 $y1 $x2 $y2 -to 0 0 $targetWidth $targetHeight
        } else {
            # stretch
            # IMPORTANT: (x1,y1,x2,y2) gets ignored -- this proc may only be invoked with the full image!
            set newimg [resizeimage $img $targetWidth $targetHeight]
        }

        set img_cache($key) $newimg
    }
    return $img_cache($key)
}


#
# creates and returns a new image, resized to the given size
#
proc resizeimage {img sx sy} {
    set destimg [image create photo -width $sx -height $sy]
    opp_resizeimage $destimg $img
    return $destimg
}


# draw_connection --
#
# This function is invoked from the module inspector C++ code.
#
proc draw_connection {c gateptr dispstr srcptr destptr src_i src_n dest_i dest_n} {

    # puts "DEBUG: draw_connection $c $gateptr $dispstr $srcptr $destptr $src_i $src_n $dest_i $dest_n"

    if [catch {
       set src_rect [get_submod_coords $c $srcptr]
       set dest_rect [get_submod_coords $c $destptr]
    } errmsg] {
       # skip this connection if source or destination of the arrow cannot be found
       return
    }

    if [catch {

       get_parsed_display_string $dispstr tags [winfo toplevel $c] {} 0

       if {![info exists tags(m)]} {set tags(m) {a}}

       set mode [lindex $tags(m) 0]
       if {$mode==""} {set mode "a"}
       set src_anch  [list [lindex $tags(m) 1] [lindex $tags(m) 2]]
       set dest_anch [list [lindex $tags(m) 3] [lindex $tags(m) 4]]

       # puts "DEBUG: src_rect=($src_rect) dest_rect=($dest_rect)"
       # puts "DEBUG: src_anch=($src_anch) dest_anch=($dest_anch)"

       regexp -- {^.[^.]*} $c win

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
       if {[string match "da*" $style]} {
           set pattern "-"
       } elseif {[string match "d*" $style]} {
           set pattern "."
       } else {
           set pattern ""
       }

       $c create line $arrow_coords -arrow last -fill $fill -dash $pattern -width $width -tags "dx tooltip conn $gateptr"

       if {[info exists tags(t)]} {
           set txt [lindex $tags(t) 0]
           # TODO implement: second par is text position
           set color [lindex $tags(t) 2]
           if {$color == ""} {set color "#005030"}
           if {[string index $color 0]== "@"} {set color [opp_hsb_to_rgb $color]}
           set x1 [lindex $arrow_coords 0]
           set y1 [lindex $arrow_coords 1]
           set x2 [lindex $arrow_coords 2]
           set y2 [lindex $arrow_coords 3]
           set x [expr ($x1+$x2+$x2)/3]
           set y [expr ($y1+$y2+$y2)/3]
           if {($x1<$x2) == ($y1<=$y2)} {
              set anch "n"
           } else {
              set anch "s"
           }
           set just "center"
           $c create text $x $y -text $txt -fill $color -anchor $anch -justify $just -tags "dx"
       }

    } errmsg] {
       tk_messageBox -type ok -title Error -icon error -parent [winfo toplevel [focus]] \
                     -message "Error in display string of a connection: $errmsg"
    }
}


# draw_message --
#
# This function is invoked from the message animation code.
#
proc draw_message {c msgptr x y} {
    global fonts inspectordata

    set zoomfactor $inspectordata($c:zoomfactor)

    set dispstr [opp_getobjectfield $msgptr displayString]
    set msgkind [opp_getobjectfield $msgptr kind]

    if {$dispstr=="" && [opp_getsimoption penguin_mode]} {
        # following lines were used for testing only...
        #set dispstr "b=15,15,rect;o=white,kind,5"
        #set dispstr "b="
        #set dispstr "o=kind"
        #set dispstr "b=15,15,oval;o=kind,white,6"
        #set dispstr "i=handset2_s"
        set dispstr "i=penguin"
    }

    if {$dispstr==""} {

        # default presentation: red or msgkind%8-colored ball
        if [opp_getsimoption animation_msgcolors] {
            set color [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
        } else {
            set color red
        }
        set ball [$c create oval -5 -5 5 5 -fill $color -outline $color -tags "dx tooltip msg $msgptr"]
        $c move $ball $x $y

        set labelx $x
        set labely $y

    } else {
        # use display string

        # supports "b","i" and "o" tags, they work just as with submodules only default
        # is different (small red ball), plus special color "kind" is supported which
        # gives the original, message kind dependent colors
        get_parsed_display_string $dispstr tags [winfo toplevel $c] {} 1

        # set sx and sy
        if ![info exists tags(is)] {
            set tags(is) {}
        }
        if [info exists tags(i)] {
            set img [dispstr_getimage $tags(i) $tags(is) $zoomfactor]
            set sx [image width $img]
            set sy [image height $img]
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

            $c create image $x $y -image $img -anchor center -tags "dx tooltip msg $msgptr"

            set labelx $x
            set labely [expr $y+$sy/2+3]

        } elseif [info exists tags(b)] {

            set x1 [expr $x - $sx/2]
            set y1 [expr $y - $sy/2]
            set x2 [expr $x + $sx/2]
            set y2 [expr $y + $sy/2]

            set sh [lindex $tags(b) 2]
            if {$sh == ""} {set sh oval}

            if {![info exists tags(o)]} {set tags(o) {}}
            set fill [lindex $tags(o) 0]
            if {$fill == ""} {set fill red}
            if {$fill == "kind"} {
                set fill [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
            }
            set outline [lindex $tags(o) 1]
            if {$outline == ""} {set outline ""}
            if {$outline == "kind"} {
                set outline [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
            }
            set width [lindex $tags(o) 2]
            if {$width == ""} {set width 1}

            $c create $sh $x1 $y1 $x2 $y2 -fill $fill -width $width -outline $outline -tags "dx tooltip msg $msgptr"

            set labelx $x
            set labely [expr $y2+$width/2+3]
        }
    }

    # display message label: "(classname)name"
    set msglabel ""
    if [opp_getsimoption animation_msgclassnames] {
        set msglabel "([opp_getobjectclassname $msgptr])"
    }
    if [opp_getsimoption animation_msgnames] {
        append msglabel "[opp_getobjectfullname $msgptr]"
    }
    if {$msglabel!=""} {
        $c create text $labelx $labely -text $msglabel -anchor n -font $fonts(msgname) -tags "dx tooltip msgname $msgptr"
    }

}

proc animcontrol {w} {
    global priv

    scale $w -orient horizontal -length 50 -sliderlength 8 -showvalue 0 -bd 1
    $w config -from .5 -to 3 -resolution 0.01 -variable priv(animspeed)

    # following line is too new (Tcl8.4) -- not understood by Tcl8.3
    #trace add variable priv(animspeed) write animSpeedChanged
    trace variable priv(animspeed) w animSpeedChanged
}

proc create_graphicalmodwindow {name geom} {
    global icons help_tips inspectordata
    global B2 B3

    set w $name
    create_inspector_toplevel $w $geom

    # create toolbar
    pack_iconbutton $w.toolbar.ascont  -image $icons(asobject) -command "inspect_this $w {As Object}"
    pack_iconbutton $w.toolbar.win     -image $icons(asoutput) -command "inspect_this $w {Module output}"
    pack_iconbutton $w.toolbar.sep1    -separator

    moduleinspector_add_run_buttons $w

    pack_iconbutton $w.toolbar.redraw  -image $icons(redraw) -command "opp_inspectorcommand $w relayout"

    animcontrol $w.toolbar.animspeed
    pack $w.toolbar.animspeed -anchor c -expand 0 -fill none -side left -padx 5 -pady 0

    pack_iconbutton $w.toolbar.zoomin  -image $icons(zoomin)  -command "graphmodwin_zoomby $w 1.25"
    pack_iconbutton $w.toolbar.zoomout -image $icons(zoomout) -command "graphmodwin_zoomby $w 0.8"
    pack_iconbutton $w.toolbar.sep2    -separator
    pack_iconbutton $w.toolbar.showlabels -image $icons(modnames) -command "graphmodwin_togglelabels $w"

    set help_tips($w.toolbar.owner)   {Inspect parent module}
    set help_tips($w.toolbar.ascont)  {Inspect as object}
    set help_tips($w.toolbar.win)     {See module output}
    set help_tips($w.toolbar.redraw)  {Re-layout}
    set help_tips($w.toolbar.animspeed) {Animation speed -- see Options dialog}
    set help_tips($w.toolbar.zoomin)  {Zoom in}
    set help_tips($w.toolbar.zoomout) {Zoom out}

    # create canvas
    set c $w.c

    # init some state vars
    set inspectordata($c:zoomfactor) 1
    set inspectordata($c:showlabels) 1

    frame $w.grid
    scrollbar $w.hsb -orient horiz -command "$c xview"
    scrollbar $w.vsb -command "$c yview"
    canvas $c -background "#a0e0a0" -relief raised -closeenough 2 \
        -xscrollcommand "$w.hsb set" \
        -yscrollcommand "$w.vsb set"
    pack $w.grid -expand yes -fill both -padx 1 -pady 1
    grid rowconfig    $w.grid 0 -weight 1 -minsize 0
    grid columnconfig $w.grid 0 -weight 1 -minsize 0

    grid $c -in $w.grid -row 0 -column 0 -rowspan 1 -columnspan 1 -sticky news
    grid $w.vsb -in $w.grid -row 0 -column 1 -rowspan 1 -columnspan 1 -sticky news
    grid $w.hsb -in $w.grid -row 1 -column 0 -rowspan 1 -columnspan 1 -sticky news

    # mouse bindings
    $c bind submod <Double-1> "graphmodwin_dblclick $c"
    $c bind conn <Double-1> "graphmodwin_dblclick $c"
    $c bind msg <Double-1> "graphmodwin_dblclick $c"
    $c bind msgname <Double-1> "graphmodwin_dblclick $c"
    $c bind qlen <Double-1> "graphmodwin_qlen_dblclick $c"

    $c bind submod <$B3> "graphmodwin_rightclick $c %X %Y"
    $c bind conn <$B3> "graphmodwin_rightclick $c %X %Y"
    $c bind msg <$B3> "graphmodwin_rightclick $c %X %Y"
    $c bind msgname <$B3> "graphmodwin_rightclick $c %X %Y"
    $c bind mod <$B3> "graphmodwin_rightclick $c %X %Y"
    $c bind modname <$B3> "graphmodwin_rightclick $c %X %Y"
    $c bind qlen <$B3> "graphmodwin_qlen_rightclick $c %X %Y"

    if {$inspectordata($c:showlabels)} {
        $w.toolbar.showlabels config -relief sunken
    }

    #update idletasks
    update
    if [catch {
       opp_inspectorcommand $w relayout
    } errmsg] {
       tk_messageBox -type ok -title Error -icon error -parent [winfo toplevel [focus]] \
                     -message "Error displaying network graphics: $errmsg"
    }

    if {$geom==""} {
        # adjust window size to size of graphics
        set bb [$c cget -scrollregion]
        set w [expr [lindex $bb 2]-[lindex $bb 0]]
        set h [expr [lindex $bb 3]-[lindex $bb 1]]
        if {$w>900} {set w 900}
        if {$h>600} {set h 600}
        $c config -width $w
        $c config -height $h
    }
}

proc graphmodwin_zoomby {w mult} {
    global inspectordata
    set c $w.c
    if {($mult<1 && $inspectordata($c:zoomfactor)>0.001) || ($mult>1 && $inspectordata($c:zoomfactor)<1000)} {
        set inspectordata($c:zoomfactor) [expr $inspectordata($c:zoomfactor) * $mult]
        if {abs($inspectordata($c:zoomfactor)-1.0) < 0.1} {set inspectordata($c:zoomfactor) 1}
        opp_inspectorcommand $w redraw
    }
    #puts "zoom: $inspectordata($c:zoomfactor)"
}

proc graphmodwin_togglelabels {w} {
    global inspectordata
    set c $w.c
    set inspectordata($c:showlabels) [expr !$inspectordata($c:showlabels)]
    opp_inspectorcommand $w redraw

    if {$inspectordata($c:showlabels)} {set relief "sunken"} else {set relief "raised"}
    $w.toolbar.showlabels config -relief $relief
}

proc graphmodwin_dblclick c {

   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }

   if {$ptr!=""} {
      opp_inspect $ptr "(default)"
   }
}

proc graphmodwin_rightclick {c X Y} {
   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }
   set ptr [lindex $ptr 0]

   if {$ptr!=""} {
      popup_insp_menu $ptr $X $Y 1
   }
}


# graphmodwin_draw_message_on_gate --
#
# Draw message near the head of the connection arrow.
# Called from inspector C++ code.
#
proc graphmodwin_draw_message_on_gate {c gateptr msgptr} {

    #debug "graphmodwin_draw_message_on_gate $msgptr"

    global fonts

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
    set endpos [graphmodwin_getmessageendpos $x1 $y1 $x2 $y2]
    setvars {xx yy} $endpos

    draw_message $c $msgptr $xx $yy
}

#
# Calculates the position where a sent message ball should rest until its event
# comes and it gets processed by the module
#
proc graphmodwin_getmessageendpos {x1 y1 x2 y2} {
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

# graphmodwin_draw_message_on_module --
#
# Draw message on submodule rectangle.
# Called from inspector C++ code.
#
proc graphmodwin_draw_message_on_module {c modptr msgptr} {

    #debug "graphmodwin_draw_message_on_module $msgptr"
    set r  [get_submod_coords $c $modptr]
    set x [expr ([lindex $r 0]+[lindex $r 2])/2]
    set y [expr ([lindex $r 1]+[lindex $r 3])/2]

    draw_message $c $msgptr $x $y
}

# graphmodwin_draw_nexteventmarker --
#
# This function is invoked from the module inspector C++ code.
#
proc graphmodwin_draw_nexteventmarker {c modptr type} {
    set src  [get_submod_coords $c $modptr]
    set x1 [expr [lindex $src 0]-2]
    set y1 [expr [lindex $src 1]-2]
    set x2 [expr [lindex $src 2]+2]
    set y2 [expr [lindex $src 3]+2]
    # $type==1 compound module, $type==2 simple module
    if {$type==1} {
        #$c create rect $x1 $y1 $x2 $y2 -tags {nexteventmarker} -outline red -dash {.}
        $c create rect $x1 $y1 $x2 $y2 -tags {nexteventmarker} -outline red -width 1
    } else {
        #$c create rect $x1 $y1 $x2 $y2 -tags {nexteventmarker} -outline red
        $c create rect $x1 $y1 $x2 $y2 -tags {nexteventmarker} -outline red -width 2
    }
}

# graphmodwin_update_submod --
#
# This function is invoked from the module inspector C++ code.
#
proc graphmodwin_update_submod {c modptr} {
    # currently the only thing to be updated is the number of elements in queue
    set win [winfo toplevel $c]
    set dispstr [opp_getobjectfield $modptr displayString]
    set qname [opp_displaystring $dispstr getTagArg "q" 0]
    if {$qname!=""} {
        #set qptr [opp_inspectorcommand $win getsubmodq $modptr $qname]
        #set qlen [opp_getobjectfield $qptr length]
        # TBD optimize -- maybe store and remember q pointer?
        set qlen [opp_inspectorcommand $win getsubmodqlen $modptr $qname]
        $c itemconfig "qlen-$modptr" -text "q:$qlen"
    }
}

#
# Helper proc.
#
proc graphmodwin_qlen_getqptr_current {c} {
   set item [$c find withtag current]
   set tags [$c gettags $item]

   set modptr ""
   if {[lsearch $tags "qlen-ptr*"] != -1} {
       regexp "ptr.*" $tags modptr
   }
   if {$modptr==""} {return}

   return [graphmodwin_qlen_getqptr $c $modptr]

}

proc graphmodwin_qlen_getqptr {c modptr} {
   set win [winfo toplevel $c]
   set dispstr [opp_getobjectfield $modptr displayString]
   set qname [opp_displaystring $dispstr getTagArg "q" 0]
   if {$qname!=""} {
       set qptr [opp_inspectorcommand $win getsubmodq $modptr $qname]
       return $qptr
   }
   return ""
}

proc graphmodwin_qlen_dblclick c {
   set qptr [graphmodwin_qlen_getqptr_current $c]
   if {$qptr!="" && $qptr!=[opp_null]} {
       opp_inspect $qptr "(default)"
   }
}

proc graphmodwin_qlen_rightclick {c X Y} {
   set qptr [graphmodwin_qlen_getqptr_current $c]
   if {$qptr!="" && $qptr!=[opp_null]} {
       popup_insp_menu $qptr $X $Y
   }
}

# graphmodwin_bubble --
#
# This function is invoked from the module inspector C++ code.
#
proc graphmodwin_bubble {c modptr txt} {
    set r  [get_submod_coords $c $modptr]
    set x [expr ([lindex $r 0]+[lindex $r 2])/2]
    set y [expr [lindex $r 1]+4]

    while {[string length $txt]<5} {set txt " $txt "}
    set txtid  [$c create text $x $y -text " $txt " -anchor c -tags "bubble"]
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

    set bubbleid [$c create polygon $pp -outline black -fill $color -width 1 -smooth 1 -tags "bubble"]
    $c lower $bubbleid $txtid

    set dx [expr $x-$xme]
    set dy [expr $y-$yme]

    $c move $bubbleid $dx $dy
    $c move $txtid $dx $dy

    set sp [opp_getsimoption animation_speed]
    set ad [expr int(1000 / (0.1+$sp))]
    after $ad [list catch [list $c delete $txtid $bubbleid]]
}


#
# Called from Layouter::debugDraw()
#
proc layouter_debugDraw_finish {c msg} {
    # create label
    set bb [$c bbox bbox]
    $c create text [lindex $bb 0] [lindex $bb 1] -tag bbox -anchor sw -text $msg
    set bb [$c bbox bbox]

    # rescale to fit canvas
    set w [expr [lindex $bb 2]-[lindex $bb 0]]
    set h [expr [lindex $bb 3]-[lindex $bb 1]]
    set cw [winfo width $c]
    set ch [winfo height $c]
    set fx [expr $cw/double($w)]
    set fy [expr $ch/double($h)]
    if {$fx>1} {set fx 1}
    if {$fy>1} {set fy 1}
    $c scale all 0 0 $fx $fy

    $c config -scrollregion [$c bbox bbox]
    $c xview moveto 0
    $c yview moveto 0
    update idletasks
}


