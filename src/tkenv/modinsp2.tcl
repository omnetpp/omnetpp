#==========================================================================
#  MODINSP.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#-----------------------------------------------------------------
#  Graphical compound module window stuff
#-----------------------------------------------------------------

image create bitmap defaulticon -data {
   #define box2_width 32
   #define box2_height 32
   static unsigned char box2_bits[] = {
    0xf8, 0xff, 0xff, 0x1f, 0xfc, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0x7f,
    0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0,
    0xcf, 0xff, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xf3, 0xcf, 0x00, 0x00, 0xf3,
    0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3,
    0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3,
    0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3,
    0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3,
    0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3, 0xcf, 0x00, 0x00, 0xf3,
    0xcf, 0xff, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xf3, 0x0f, 0x00, 0x00, 0xf0,
    0x0f, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0x7f,
    0xfc, 0xff, 0xff, 0x3f, 0xf8, 0xff, 0xff, 0x1f};
}


proc split_dispstr {str array} {
   upvar $array arr

   #
   # BUG: module parameters not handled
   #

   foreach tag [split $str {;}] {
      set tag [split $tag {=}]
      set key [lindex $tag 0]
      set val [split [lindex $tag 1] {,}]

      # set i 0
      # foreach v $val {
      #    if {[string range $v 0 0]=={$}} {
      #       set v [opp_inspectorcommand $ptr modpar $modptr [string range $v 1 end]]
      #       lreplace val $i $i $v
      #     }
      #     incr i
      # }

      if {$key != ""} {
         set arr($key) $val
      }
   }
}

proc draw_submod {c ptr name dispstr i n default_layout} {

    # puts "DEBUG: draw_submod $c $ptr $name $dispstr $i $n $default_layout"

    split_dispstr $dispstr tags

    set bx 10
    set by 10

    if [catch {
       # set sx and sy
       if [info exists tags(i)] {
           set img [lindex $tags(i) 0]
           if {$img=="" || [catch {image type $img}]} {
               tk_messageBox -type ok -title Error -icon error \
                 -message "Error in display string of $name: icon \"$img\" does not exist, using default."
               set img defaulticon
           }
           set sx [image width $img]
           set sy [image height $img]
       } elseif [info exists tags(b)] {
           set sx [lindex $tags(b) 0]
           if {$sx==""} {set sx 40}
           set sy [lindex $tags(b) 1]
           if {$sy==""} {set sy [expr 0.6*$sx]}
       } else {
           set tags(b) {40 24 rect}
           set sx 40
           set sy 24
       }

       # determine position
       if {![info exists tags(p)]} {set tags(p) {}}

       set x [lindex $tags(p) 0]
       set y [lindex $tags(p) 1]
       set layout [lindex $tags(p) 2]
       if {$layout=="" && $n>1} {set layout $default_layout}

       if {$x=="" || $y==""} {
           global hack_y

           if {$i==0} {set hack_y [expr $hack_y + 2*$sy]}
           if {$n==1} {
              if {$x==""} {set x [expr $sx+150*rand()]}
              if {$y==""} {set y $hack_y}
           } else {
              if {$x==""} {set x $sx}
              if {$y==""} {set y $hack_y}
           }
           if {$hack_y<$y} {set hack_y $y}
       }
       set x [expr $x + $bx]
       set y [expr $y + $by]

       # handle auto layouts for submod vectors
       switch $layout {
           {} {
           }
           row {
               set spc [lindex $tags(p) 3]
               if {$spc==""} {set spc [expr 2*$sx]}
               set x [expr $x + $i*$spc]
           }
           col {
               set spc [lindex $tags(p) 3]
               if {$spc==""} {set spc [expr 2*$sy]}
               set y [expr $y + $i*$spc]
           }
           matrix {
               # perrow: how many submodules we want in a row
               set perrow [lindex $tags(p) 3]
               if {$perrow=="" || $perrow==0} {set perrow 5}
               set dx [lindex $tags(p) 4]
               if {$dx==""} {set dx [expr 2*$sx]}
               set dy [lindex $tags(p) 5]
               if {$dy==""} {set dy [expr 2*$sy]}
               set x [expr $x + ($i % $perrow) * $dx]
               set y [expr $y + int($i / $perrow) * $dy]
           }
           ring {
               set rx [lindex $tags(p) 3]
               if {$rx==""} {set rx [expr ($sx+$sy)*$n/4]}
               set ry [lindex $tags(p) 4]
               if {$ry==""} {set ry $rx}
               set x [expr $x + $rx + $rx*sin($i*6.2832/$n)]
               set y [expr $y + $ry - $ry*cos($i*6.2832/$n)]
           }
       }

       if [info exists tags(i)] {

           $c create image $x $y -image $img -anchor center -tags "submod $ptr"
           $c create text $x [expr $y+$sy/2+3] -text $name -anchor n

       } elseif [info exists tags(b)] {

           set x1 [expr $x - $sx/2]
           set y1 [expr $y - $sy/2]
           set x2 [expr $x + $sx/2]
           set y2 [expr $y + $sy/2]

           set sh [lindex $tags(b) 2]
           if {$sh == ""} {set sh rect}

           if {![info exists tags(o)]} {set tags(o) {}}
           set fill [lindex $tags(o) 0]
           if {$fill == ""} {set fill #8080ff}
           set outl [lindex $tags(o) 1]
           if {$outl == ""} {set outl black}
           set width [lindex $tags(o) 2]
           if {$width == ""} {set width 2}

           $c create $sh $x1 $y1 $x2 $y2 \
               -fill $fill -width $width -outline $outl \
               -tags "submod $ptr"
           $c create text $x [expr $y2+$width/2+3] -text $name -anchor n

       }
   } errmsg] {
       tk_messageBox -type ok -title Error -icon error \
                     -message "Error in display string of $name: $errmsg"
   }
}

proc draw_enclosingmod {c ptr name dispstr} {

    # puts "DEBUG: draw_enclosingmod $c $ptr $name $dispstr"

    split_dispstr $dispstr tags

    set bx 10
    set by 10

    if [catch {
       if {![info exists tags(b)]} {set tags(b) {{} {} {}}}

       set sx [lindex $tags(b) 0]
       set sy [lindex $tags(b) 1]
       if {$sx=="" || $sy==""} {
           set bb [$c bbox submod]
           if {$bb==""} {set bb "$bx $by 300 200"}
           if {$sx==""} {set sx [expr [lindex $bb 2]+[lindex $bb 0]-2*$bx]}
           if {$sy==""} {set sy [expr [lindex $bb 3]+[lindex $bb 1]-2*$by]}
       }

       set sh [lindex $tags(b) 2]
       if {$sh == ""} {set sh rect}

       if {![info exists tags(o)]} {set tags(o) {}}
       set fill [lindex $tags(o) 0]
       if {$fill == ""} {set fill #c0c0c0}
       set outl [lindex $tags(o) 1]
       if {$outl == ""} {set outl black}
       set width [lindex $tags(o) 2]
       if {$width == ""} {set width 2}

       $c create $sh $bx $by [expr $bx+$sx] [expr $by+$sy] \
           -fill $fill -width $width -outline $outl \
           -tags "mod $ptr"
       $c create text [expr $bx+3] [expr $by+3] -text $name -anchor nw
       $c lower mod

       set cwidth [expr $bx+$sx+$bx]
       set cheight [expr $by+$sy+$by]
       $c config -width [expr $cwidth<500 ? $cwidth : 500]
       $c config -height [expr $cheight<400 ? $cheight : 400]
       $c config -scrollregion "0 0 $cwidth $cheight"


   } errmsg] {
       tk_messageBox -type ok -title Error -icon error \
                     -message "Error in display string of $name: $errmsg"
   }
}

proc _getcoords {c tag} {

    set id [$c find withtag $tag]
    if {$id==""} {error "$tag not found"}

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

proc draw_connection {c ptr dispstr srcptr destptr src_i src_n dest_i dest_n} {

    # puts "DEBUG: draw_connection $c $ptr $dispstr $srcptr $destptr \
                 $src_i $src_n $dest_i $dest_n"

    split_dispstr $dispstr tags

    if {![info exists tags(m)]} {set tags(m) {a}}

    set src_rect [_getcoords $c $srcptr]
    set dest_rect [_getcoords $c $destptr]

    set mode [lindex $tags(m) 0]
    set src_anch  [list [lindex $tags(m) 1] [lindex $tags(m) 2]]
    set dest_anch [list [lindex $tags(m) 3] [lindex $tags(m) 4]]

    # puts "DEBUG: src_rect=($src_rect) dest_rect=($dest_rect)"
    # puts "DEBUG: src_anch=($src_anch) dest_anch=($dest_anch)"

    regexp -- {^.[^.]*} $c win

    set arrow_coords [eval [concat opp_inspectorcommand $win arrowcoords \
               $src_rect $dest_rect $src_i $src_n $dest_i $dest_n \
               $mode $src_anch $dest_anch]]

    # puts "DEBUG: arrow=($arrow_coords)"

    if [catch {
       if {![info exists tags(o)]} {set tags(o) {}}
       set fill [lindex $tags(o) 0]
       if {$fill == ""} {set fill black}
       set width [lindex $tags(o) 1]
       if {$width == ""} {set width 1}

       eval $c create line $arrow_coords -arrow last \
           -fill $fill -width $width -tags "\"conn $ptr\""

    } errmsg] {
       tk_messageBox -type ok -title Error -icon error \
                     -message "Error in display string of a connection: $errmsg"
    }
}

proc create_graphicalmodwindow {name} {
    global icons help_tips

    set w $name
    create_inspector_toplevel $w

    # create toolbar
    iconbutton $w.toolbar.ascont -image $icons(asobject) -command "inspect_this $w {As Object}"
    iconbutton $w.toolbar.sep1   -separator
    iconbutton $w.toolbar.parent -image $icons(parent) ;#command assigned from C++
    iconbutton $w.toolbar.params -image $icons(params) ;#command assigned from C++
    iconbutton $w.toolbar.gates  -image $icons(gates)  ;#command assigned from C++
    iconbutton $w.toolbar.sep2   -separator
    iconbutton $w.toolbar.redraw -image $icons(redraw) -command "graphmodwin_redraw $w"
    foreach i {ascont sep1 parent params gates sep2 redraw} {
       pack $w.toolbar.$i -anchor n -side left -padx 0 -pady 2
    }

    set help_tips($w.toolbar.ascont)  {Inspect as object}
    set help_tips($w.toolbar.parent)  {Inspect parent module}
    set help_tips($w.toolbar.params)  {Inspect parameters}
    set help_tips($w.toolbar.gates)   {Inspect gates}
    set help_tips($w.toolbar.redraw)  {Rearrange randomly placed submodules}

    # create canvas
    set c $w.c

    frame $w.grid
    scrollbar $w.hsb -orient horiz -command "$c xview"
    scrollbar $w.vsb -command "$c yview"
    canvas $c -background #a0e0a0 -relief raised \
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

    $c bind submod <3> "graphmodwin_rightclick $c %X %Y"
    $c bind conn <3> "graphmodwin_rightclick $c %X %Y"
    $c bind msg <3> "graphmodwin_rightclick $c %X %Y"
    $c bind msgname <3> "graphmodwin_rightclick $c %X %Y"

    graphmodwin_redraw $w
}

proc graphmodwin_redraw w {

    $w.c delete all

    global hack_y
    set hack_y 0

    opp_inspectorcommand $w redraw
}

proc graphmodwin_dblclick c {

   set item [$c find withtag current]
   set tags [$c gettags $item]

   set ptr ""
   if {[lsearch $tags "ptr*"] != -1} {
      regexp "ptr.*" $tags ptr
   }

   if {$ptr!=""} {
      opp_inspect $ptr (default)
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
      popup_insp_menu $ptr $X $Y
   }
}

proc draw_message {c gateptr msgptr msgname {msgkind {}}} {

    # draw message near the head of the connection arrow
    global fonts

    # gate pointer + conn are the tags of the connection arrow
    foreach id [$c find withtag $gateptr] {
       if {[lsearch -exact [$c gettags $id] "conn"] != -1} {
          set conn_id $id
       }
    }

    set coords [$c coords $conn_id]

    set x1 [lindex $coords 0]
    set y1 [lindex $coords 1]
    set x2 [lindex $coords 2]
    set y2 [lindex $coords 3]
    set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]

    set steps [expr int($len/2)]
    if {$steps==0} {set steps 1}

    set dx [expr ($x2-$x1)/$steps]
    set dy [expr ($y2-$y1)/$steps]

    if [opp_getsimoption animation_msgcolors] {
        set color [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
    } else {
        set color red
    }

    set steps [expr $steps<6 ? 0 : $steps-6]
    set xx [expr $x1+$dx*$steps]
    set yy [expr $y1+$dy*$steps]
    set ball [$c create oval -5 -5 5 5 -fill $color -outline $color -tags "msg $msgptr"]
    $c move $ball $xx $yy
    if [opp_getsimoption animation_msgnames] {
        $c create text $xx $yy -text $msgname -anchor n -font $fonts(msgname) -tags "msgname $msgptr"
    }
}

proc calibrate_animdelay {} {

   # animdelay: holds upper limit for delay `for' loop
   global animdelay

   # 100ms should be 100000 ticks on a PC -- verify that
   set t0 [clock clicks]
   after 100
   set t1 [clock clicks]
   set ms100 [expr $t1-$t0]
   if {$ms100>80000 && $ms100<120000} {set ms100 100000}


   # test proc speed: how many "clock clicks" does an empty 4000 loop take?
   set repeatcount 10
   while {1} {
      #puts "dbg: repeatcount=$repeatcount"
      set ad [expr 4000*$repeatcount]
      set t0 [clock clicks]
      for {set i 0} {$i<$ad} {incr i} {}
      set t1 [clock clicks]
      #puts "dbg: t0=$t0, t1=$t1"
      if {$t1!=$t0} break
      set repeatcount [expr 5*$repeatcount]
   }
   set loopticks [expr ($t1-$t0)/$repeatcount]
   #puts "dbg: loopticks=$loopticks"

   # calc preliminary animdelay from loopticks
   set animdelay [expr 100*$ms100/$loopticks]

   # empirical correction
   set animdelay [expr int($animdelay * sqrt($animdelay/100))]

   puts ""
   puts "Anim-speed calibrated: $animdelay"
}

proc graphmodwin_animate {win gateptr msgptr msgname msgkind {mode {}}} {

    global animdelay fonts
    set c $win.c

    # gate pointer string is the tag of the connection arrow
    set coords [$c coords $gateptr]

    set x1 [lindex $coords 0]
    set y1 [lindex $coords 1]
    set x2 [lindex $coords 2]
    set y2 [lindex $coords 3]
    set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]

    set steps [expr int($len/2)]
    if {$steps==0} {set steps 1}

    set dx [expr ($x2-$x1)/$steps]
    set dy [expr ($y2-$y1)/$steps]

    if [opp_getsimoption animation_msgcolors] {
        set color [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]
    } else {
        set color red
    }

    switch $mode {
       beg - {} {
          set ball [$c create oval -5 -5 5 5 -fill $color -outline $color -tags "msg $msgptr"]
          $c move $ball $x1 $y1
          if [opp_getsimoption animation_msgnames] {
              $c create text $x1 $y1 -text $msgname -anchor n -font $fonts(msgname) -tags "msgname $msgptr"
          }
       }
       end {}
    }
    switch $mode {
       beg {set steps [expr $steps<6 ? 0 : $steps-6]}
       end {set steps 6}
    }

    set sp [opp_getsimoption animation_speed]
    set ad [expr $animdelay / (0.1+$sp*$sp)]

    for {set i 0} {$i<$steps} {incr i} {
       update idletasks
       $c move $msgptr $dx $dy
       for {set j 0} {$j<$ad} {incr j} {}
    }

    if {$mode!="beg"} {
       $c delete $msgptr
    }
}

# animate2 is not currently used
proc animate2 {c tag} {
    #
    # Animate2 has the advantage that several animations
    # can be started concurrently; one can synchronize on the
    # end of an animation by vwaiting on the returned variable name:
    #   set var [animate2 $c $arrow]; vwait $var
    #
    # The disadvantage is that the granularity of the after command
    # is big on a PC, so the animation will be slow.
    #
    set coords [$c coords $tag]

    set x1 [lindex $coords 0]
    set y1 [lindex $coords 1]
    set x2 [lindex $coords 2]
    set y2 [lindex $coords 3]
    set len [expr sqrt(($x2-$x1)*($x2-$x1)+($y2-$y1)*($y2-$y1))]

    set steps [expr int($len/5)]
    if {$steps==0} {set steps 1}

    set dx [expr ($x2-$x1)/$steps]
    set dy [expr ($y2-$y1)/$steps]

    set color [lindex {red green blue white yellow cyan magenta black} [expr $msgkind % 8]]

    set ball [$c create oval -5 -5 5 5 -fill $color -outline $color]
    $c move $ball $x1 $y1

    global done$c$ball
    update idletasks
    after 10 "animate2:move $c $ball $dx $dy $steps"

    return done$c$ball
}

proc animate2:move {c ball dx dy i} {
    $c move $ball $dx $dy
    update idletasks
    incr i -1
    if {$i>0} {
       after 10 "animate2:move $c $ball $dx $dy $i"
    } else {
       $c delete $ball
       global done$c$ball
       set done$c$ball 1
    }
}

