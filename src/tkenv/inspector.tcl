#=================================================================
#  INSPECTOR.TCL - part of
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
# General procs for inspectors
#


#===================================================================
#    INSPECTOR TOPLEVEL WINDOWS
#===================================================================

#
# gets called from concrete inspector subtypes
#
proc create_inspector_toplevel {w geom} {

    # create toplevel inspector window

    global fonts icons help_tips

    toplevel $w -class Toplevel
    wm focusmodel $w passive
    if {$geom != ""} {wm geometry $w $geom}
    #wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm protocol $w WM_DELETE_WINDOW "close_inspector_toplevel $w"

    inspectorlist_remove $w
    inspectorlist_storename $w

    # add the "Inspect As.." icon at the top
    frame $w.toolbar -relief raised -bd 1
    pack $w.toolbar -anchor w -side top -fill x -expand 0

    pack_iconbutton $w.toolbar.sep0 -separator
    pack_iconbutton $w.toolbar.owner -image $icons(parent) ;#command assigned from C++
    pack_iconbutton $w.toolbar.sep01 -separator

    set help_tips($w.toolbar.owner) {Inspect owner object}

    # add object type-and-name bar with color codes
    regexp {\.(ptr.*)-[0-9]+} $w match ptr
    set colorcode [choosecolorcode $ptr]

    frame $w.infobar -relief raised -bd 1
    button $w.infobar.color -anchor w -relief raised -bd 1 -bg $colorcode -activebackground $colorcode -image $icons(1pixtransp) -width 8 -command "inspect_this_as $w"
    label $w.infobar.name -anchor w -relief flat -justify left
    pack $w.infobar.color -anchor n -side left -expand 0 -fill y -pady 1
    pack $w.infobar.name -anchor n -side left -expand 1 -fill both -pady 1
    pack $w.infobar -anchor w -side top -fill x -expand 0

    set help_tips($w.infobar.color) {Different inspectors of the same object have the same color}
    set help_tips($w.infobar.name)  {Right-click for copying to clipboard}

    # Keyboard bindings
    bind $w <Escape>     "catch {.popup unpost}"
    bind $w <Button-1>   "catch {.popup unpost}"
    bind $w <Key-Return> "opp_writebackinspector $w; opp_updateinspectors"

    bind $w.infobar.name <Button-3> [list inspectorNamePopup $ptr %X %Y]
    bind $w.infobar.color <Button-3> [list inspectorNamePopup $ptr %X %Y]

    bind_runcommands $w
    bind_othercommands $w
}

proc choosecolorcode {ptr} {
    set colors {
    gray60 gray70 gray80 gray85 gray90 gray95 \
    snow1 snow2 snow3 snow4 seashell1 seashell2 \
    seashell3 seashell4 AntiqueWhite1 AntiqueWhite2 AntiqueWhite3 \
    AntiqueWhite4 bisque1 bisque2 bisque3 bisque4 PeachPuff1 \
    PeachPuff2 PeachPuff3 PeachPuff4 NavajoWhite1 NavajoWhite2 \
    NavajoWhite3 NavajoWhite4 LemonChiffon1 LemonChiffon2 \
    LemonChiffon3 LemonChiffon4 cornsilk1 cornsilk2 cornsilk3 \
    cornsilk4 ivory1 ivory2 ivory3 ivory4 honeydew1 honeydew2 \
    honeydew3 honeydew4 LavenderBlush1 LavenderBlush2 \
    LavenderBlush3 LavenderBlush4 MistyRose1 MistyRose2 \
    MistyRose3 MistyRose4 azure1 azure2 azure3 azure4 \
    SlateBlue1 SlateBlue2 SlateBlue3 SlateBlue4 RoyalBlue1 \
    RoyalBlue2 RoyalBlue3 RoyalBlue4 blue1 blue2 blue3 blue4 \
    DodgerBlue1 DodgerBlue2 DodgerBlue3 DodgerBlue4 SteelBlue1 \
    SteelBlue2 SteelBlue3 SteelBlue4 DeepSkyBlue1 DeepSkyBlue2 \
    DeepSkyBlue3 DeepSkyBlue4 SkyBlue1 SkyBlue2 SkyBlue3 \
    SkyBlue4 LightSkyBlue1 LightSkyBlue2 LightSkyBlue3 \
    LightSkyBlue4 SlateGray1 SlateGray2 SlateGray3 SlateGray4 \
    LightSteelBlue1 LightSteelBlue2 LightSteelBlue3 \
    LightSteelBlue4 LightBlue1 LightBlue2 LightBlue3 \
    LightBlue4 LightCyan1 LightCyan2 LightCyan3 LightCyan4 \
    PaleTurquoise1 PaleTurquoise2 PaleTurquoise3 PaleTurquoise4 \
    CadetBlue1 CadetBlue2 CadetBlue3 CadetBlue4 turquoise1 \
    turquoise2 turquoise3 turquoise4 cyan1 cyan2 cyan3 cyan4 \
    DarkSlateGray1 DarkSlateGray2 DarkSlateGray3 \
    DarkSlateGray4 aquamarine1 aquamarine2 aquamarine3 \
    aquamarine4 DarkSeaGreen1 DarkSeaGreen2 DarkSeaGreen3 \
    DarkSeaGreen4 SeaGreen1 SeaGreen2 SeaGreen3 SeaGreen4 \
    PaleGreen1 PaleGreen2 PaleGreen3 PaleGreen4 SpringGreen1 \
    SpringGreen2 SpringGreen3 SpringGreen4 green1 green2 \
    green3 green4 chartreuse1 chartreuse2 chartreuse3 \
    chartreuse4 OliveDrab1 OliveDrab2 OliveDrab3 OliveDrab4 \
    DarkOliveGreen1 DarkOliveGreen2 DarkOliveGreen3 \
    DarkOliveGreen4 khaki1 khaki2 khaki3 khaki4 \
    LightGoldenrod1 LightGoldenrod2 LightGoldenrod3 \
    LightGoldenrod4 LightYellow1 LightYellow2 LightYellow3 \
    LightYellow4 yellow1 yellow2 yellow3 yellow4 gold1 gold2 \
    gold3 gold4 goldenrod1 goldenrod2 goldenrod3 goldenrod4 \
    DarkGoldenrod1 DarkGoldenrod2 DarkGoldenrod3 DarkGoldenrod4 \
    RosyBrown1 RosyBrown2 RosyBrown3 RosyBrown4 IndianRed1 \
    IndianRed2 IndianRed3 IndianRed4 sienna1 sienna2 sienna3 \
    sienna4 burlywood1 burlywood2 burlywood3 burlywood4 wheat1 \
    wheat2 wheat3 wheat4 tan1 tan2 tan3 tan4 chocolate1 \
    chocolate2 chocolate3 chocolate4 firebrick1 firebrick2 \
    firebrick3 firebrick4 brown1 brown2 brown3 brown4 salmon1 \
    salmon2 salmon3 salmon4 LightSalmon1 LightSalmon2 \
    LightSalmon3 LightSalmon4 orange1 orange2 orange3 orange4 \
    DarkOrange1 DarkOrange2 DarkOrange3 DarkOrange4 coral1 \
    coral2 coral3 coral4 tomato1 tomato2 tomato3 tomato4 \
    OrangeRed1 OrangeRed2 OrangeRed3 OrangeRed4 red1 red2 red3 \
    red4 PaleVioletRed1 PaleVioletRed2 PaleVioletRed3 PaleVioletRed4 \
    maroon1 maroon2 maroon3 maroon4 VioletRed1 VioletRed2 VioletRed3 \
    VioletRed4 magenta1 magenta2 magenta3 magenta4 orchid1 \
    orchid2 orchid3 orchid4 plum1 plum2 plum3 plum4 \
    MediumOrchid1 MediumOrchid2 MediumOrchid3 MediumOrchid4 \
    DarkOrchid1 DarkOrchid2 DarkOrchid3 DarkOrchid4 purple1 \
    purple2 purple3 purple4 MediumPurple1 MediumPurple2 \
    MediumPurple3 MediumPurple4 thistle1 thistle2 thistle3 \
    thistle4
    }
    set i [opp_getstringhashcode $ptr]
    set i [expr $i % [llength $colors]]
    return [lindex $colors $i]
}

# icons used in the tree view and listboxes
set treeicons(cCompoundModule) {compound_vs}
set treeicons(cSimpleModule)   {simple_vs}
set treeicons(cPlaceHolderModule) {placeholder_vs}
set treeicons(cGate)           {gate_vs}
set treeicons(cPar)            {param_vs}
set treeicons(cMessage)        {message_vs}
set treeicons(cQueue)          {queue_vs}
set treeicons(cLinkedList)     {queue_vs}
set treeicons(cArray)          {container_vs}
set treeicons(cMessageHeap)    {container_vs}
set treeicons(cChannel)        {chan_vs}
set treeicons(cStatistic)      {stat_vs}
set treeicons(cOutVector)      {outvect_vs}

#
# Returns the icon of an object (for tree view / listbox)
#
proc get_icon_for_object {ptr} {
    global icons treeicons
    set class [opp_getobjectbaseclass $ptr]
    if [info exists treeicons($class)] {
       return $icons($treeicons($class))
    } else {
       return $icons(cogwheel_vs)
    }
}

#
# gets called by the WM (Window Manager)
#
proc close_inspector_toplevel {w} {
    # invokes app->deleteInspector(insp)
    opp_deleteinspector $w
}


#
# gets called from C++
#
proc inspector_hostobjectdeleted {w} {
    # if the insp window is destroyed because the object no longer exists,
    # prepare to reopen it with same geometry if the object reappears
    inspectorlist_add $w
}

#
# gets called from C++
#
proc destroy_inspector_toplevel {w} {
    destroy $w
}

#
# gets called from C++
#
proc inspector_show {w} {
    show_window $w
}

#
# Brings the window to front, and gives it focus
#
proc show_window {w} {
    global tcl_platform
    if {$tcl_platform(platform) != "windows"} {
        # looks like some X servers ignore the "raise" command unless we
        # kick them by "wm withdraw" plus "wm deiconify"...
        wm withdraw $w
        wm deiconify $w
    }
    raise $w
    focus $w
}

#
# invoked on right-clicking object name inspectors
#
proc inspectorNamePopup {ptr x y} {
    catch {destroy .popup}
    menu .popup -tearoff 0

    regsub {^ptr} $ptr {0x} p

    .popup add command -label "Copy pointer with cast (for debugger)" -command [list setClipboard "(([opp_getobjectclassname $ptr] *)$p)"]
    .popup add command -label "Copy pointer value (for debugger)" -command [list setClipboard $p]
    .popup add separator
    .popup add command -label "Copy full path" -command [list setClipboard [opp_getobjectfullpath $ptr]]
    .popup add command -label "Copy name" -command [list setClipboard [opp_getobjectfullname $ptr]]
    .popup add command -label "Copy class name" -command [list setClipboard [opp_getobjectclassname $ptr]]

    .popup post $x $y
}

proc setClipboard {str} {
    clipboard clear
    clipboard append -- $str
}

#===================================================================
#    UTILITY FUNCTIONS FOR INSPECTOR WINDOWS
#===================================================================

proc textwindow_add_icons {w} {
    global icons help_tips

    pack_iconbutton $w.toolbar.copy   -image $icons(copy) -command "edit_copy $w.main.text"
    pack_iconbutton $w.toolbar.find   -image $icons(find) -command "findDialog $w.main.text"
    pack_iconbutton $w.toolbar.save   -image $icons(save) -command "savefile $w"
    pack_iconbutton $w.toolbar.sep21  -separator

    set help_tips($w.toolbar.copy)   {Copy selected text to clipboard (Ctrl+C)}
    set help_tips($w.toolbar.find)   {Find string in window (Ctrl+F}
    set help_tips($w.toolbar.save)   {Save window contents to file}
}

proc create_inspector_listbox {w} {
    # create a listbox

    label $w.label -text "# objects:"
    pack $w.label -side top -anchor w

    frame $w.main
    pack $w.main -expand 1 -fill both -side top

    multicolumnlistbox $w.main.list {
       {class  Class   80}
       {name   Name   120}
       {info   Info}
       {ptr    Pointer}
    } -width 400 -yscrollcommand "$w.main.vsb set" -xscrollcommand "$w.main.hsb set"
    scrollbar $w.main.hsb  -command "$w.main.list xview" -orient horiz
    scrollbar $w.main.vsb  -command "$w.main.list yview"
    grid $w.main.list $w.main.vsb -sticky news
    grid $w.main.hsb  x           -sticky news
    grid rowconfig    $w.main 0 -weight 1 -minsize 0
    grid columnconfig $w.main 0 -weight 1 -minsize 0

    bind $w.main.list <Double-Button-1> "inspect_item_in $w.main.list"
    bind $w.main.list <Button-3> "popup_insp_menu \[lindex \[multicolumnlistbox_curselection $w.main.list\] 0\] %X %Y"
    bind $w.main.list <Key-Return> "inspect_item_in $w.main.list"

    focus $w.main.list
}

#
# Extends context menu with extra items. Example config for the INET Framework:
#
# extendContextMenu {
#    {"INET: Interfaces"             "**"  "**interfaceTable.interfaces"     "*vector*"}
#    {"INET: IP Routing Table"       "**"  "**routingTable.routes"           "*vector*"}
#    {"INET: IP Multicast Routes"    "**"  "**routingTable.multicastRoutes"  "*vector*"}
#    {"INET: IPv6 Routing Table"     "**"  "**routingTable6.routeList"       "*vector*"}
#    {"INET: IPv6 Destination Cache" "**"  "**routingTable6.destCache"       "*map*"   }
#    {"INET: ARP cache"              "**"  "**arp.arpCache"                  "*map*"   }
#    {"INET: TCP connections"        "**"  "**tcp.tcpAppConnMap"             "*map*"   }
#
#    {"INET: Interfaces"             "**.interfaceTable"  "interfaces"      "*vector*"}
#    {"INET: IP Routing Table"       "**.routingTable"    "routes"          "*vector*"}
#    {"INET: IP Multicast Routes"    "**.routingTable"    "multicastRoutes" "*vector*"}
#    {"INET: IPv6 Routing Table"     "**.routingTable6"   "routeList"       "*vector*"}
#    {"INET: IPv6 Destination Cache" "**.routingTable6"   "destCache"       "*map*"   }
#    {"INET: ARP cache"              "**.arp"             "arpCache"        "*map*"   }
#    {"INET: TCP connections"        "**.tcp"             "tcpAppConnMap"   "*map*"   }
# }
#
proc extendContextMenu {rules} {
    global contextmenurules

    set i [llength $contextmenurules(keys)]
    foreach line $rules {
       lappend contextmenurules(keys) $i
       if {[llength $line]!=4} {
           set rulename "\"[lindex $line 0]\""
           tk_messageBox -type ok -icon info -title Info -message "Context menu inspector rule $rulename should contain 4 items, ignoring."
       } else {
           set contextmenurules($i,label)   [lindex $line 0]
           set contextmenurules($i,context) [lindex $line 1]
           set contextmenurules($i,name)    [lindex $line 2]
           set contextmenurules($i,class)   [lindex $line 3]
       }
       incr i
    }
}

proc popup_insp_menu {ptr X Y {wantanimoptions 0}} {
    global contextmenurules

    if {$ptr=="" || $ptr==[opp_null]} return

    # create popup menu
    catch {destroy .popup}
    menu .popup -tearoff 0

    # add inspector types supported by the object
    set insptypes [opp_supported_insp_types $ptr]
    foreach type $insptypes {
       .popup add command -label "$type..." -command "opp_inspect $ptr \{$type\}"
    }

    # add "run until" menu items
    set baseclass [opp_getobjectbaseclass $ptr]
    if {$baseclass=="cSimpleModule" || $baseclass=="cCompoundModule"} {
        set w ".$ptr-0"  ;#hack
        .popup add separator
        .popup add command -label "Run until next event in this module" -command "runsimulation_local $w normal"
        .popup add command -label "Fast run until next event in this module" -command "runsimulation_local $w fast"
    }

    # add further menu items
    set name [opp_getobjectfullpath $ptr]
    set allcategories "mqsgvo"
    set first 1
    foreach key $contextmenurules(keys) {
       #debug "trying $contextmenurules($key,label): opp_getsubobjectsfilt $ptr $allcategories $contextmenurules($key,class) $name.$contextmenurules($key,name) 1"
       # check context matches
       if ![opp_patmatch $name $contextmenurules($key,context)] {
           continue
       }
       # check we have such object
       set objlist [opp_getsubobjectsfilt $ptr $allcategories $contextmenurules($key,class) "$name.$contextmenurules($key,name)" 1 ""]
       if {$objlist!={}} {
           if {$first} {
               set first 0
               .popup add separator
           }
           .popup add command -label "$contextmenurules($key,label)..." -command "inspect_contextmenurules $ptr $key"
       }
    }

    if {$wantanimoptions} {
       .popup add separator
       .popup add command -label "Animation options..." -command "options_dialog a"
    }

    .popup post $X $Y
}

proc inspect_contextmenurules {ptr key} {
    global contextmenurules
    set allcategories "mqsgvo"
    set name [opp_getobjectfullpath $ptr]
    set objlist [opp_getsubobjectsfilt $ptr $allcategories $contextmenurules($key,class) "$name.$contextmenurules($key,name)" 100 ""]
    if {[llength $objlist] > 5} {
        tk_messageBox -type ok -icon info -title Info -message "This matches [llength $objlist]+ objects, opening inspectors only for the first five."
        set objlist [lrange $objlist 0 4]
    }
    foreach objptr $objlist {
        opp_inspect $objptr "(default)"
    }
}

proc ask_inspectortype {ptr parentwin {typelist {}}} {
    set w .asktype

    if {$typelist=={}} {
        set typelist [opp_supported_insp_types $ptr]
    }

    # if there's only one, use yes/no dialog instead
    if {[llength $typelist]==1} {
        set type [lindex $typelist 0]
        set ans [tk_messageBox -message "Open inspector of type '$type'?" \
                  -title "Open inspector" -icon question -type yesno -parent $parentwin]
        if {$ans == "yes"} {
            return [lindex $typelist 0]
        } else {
            return ""
        }
    }

    # chooser dialog
    createOkCancelDialog $w {Open Inspector}
    label-combo $w.f.type {Choose type:} $typelist
    pack $w.f.type -expand 0 -fill x -side top

    set type ""
    if [execOkCancelDialog $w] {
        set type [$w.f.type.e cget -value]

        if {[lsearch $typelist $type] == -1} {
            messagebox {Error} "Invalid inspector type. Please choose from the list." error ok
            set type ""
        }
    }
    destroy $w

    return $type
}

proc inspect_item_in {lb} {
    # called on double-clicking in a container inspector;
    # inspect the current item in the listbox of an inspector listwindow

    set ptr [lindex [multicolumnlistbox_curselection $lb] 0]
    if {$ptr != "" && $ptr != [opp_null]} {
        opp_inspect $ptr {(default)}
    }
}

proc inspectas_item_in {lb} {
    # called on double-clicking in a container inspector;
    # inspect the current item in the listbox of an inspector listwindow

    set ptr [lindex [multicolumnlistbox_curselection $lb] 0]
    if {$sel != ""} {
        set type [ask_inspectortype $ptr [winfo toplevel $lb]]
        if {$type != ""} {
            opp_inspect $ptr $type
        }
    }
}

proc inspect_this_as {win} {
    # called by the "Inspect As.." button at the TOP of an inspector
    # extract object pointer from window path name and create inpector
    regexp {\.(ptr.*)-([0-9]+)} $win match ptr curtype

    # do not offer the type of the inspector from which we're invoked
    set typelist [opp_supported_insp_types $ptr]
    set pos [lsearch $typelist [opp_inspectortype $curtype]]
    set typelist [lreplace $typelist $pos $pos]

    # if no choice, don't do anything
    if {[llength $typelist]==0} {return}

    # type selection dialog
    set type [ask_inspectortype $ptr $win $typelist]
    if {$type != ""} {
        opp_inspect $ptr $type
    }
}

proc inspect_this {win type} {
    # extract object pointer from window path name and create inspector
    regexp {\.(ptr.*)-[0-9]+} $win match object
    opp_inspect $object $type
}


#
# Called from balloon.tcl, supposed to return tooltip for a widget (or item
# in a widget). Installed via: set help_tips(helptip_proc) get_help_tip
#
# Here we produce help text for canvas items that represent simulation
# objects.
#
proc get_help_tip {w x y item} {
   if {[winfo class $w]=="Canvas" && $item!=""} {

       # if this is a simulation object, get its pointer
       set ptr ""
       set tags [$w gettags $item]

       if {[lsearch $tags "tooltip"] == -1} {
          return ""
       }

       if {[lsearch $tags "ptr*"] != -1} {
          regexp "ptr.*" $tags ptr
       } elseif {[lsearch $tags "qlen-ptr*"] != -1} {
          regexp "ptr.*" $tags modptr
          set ptr [graphmodwin_qlen_getqptr $w $modptr]
       } elseif {[lsearch $tags "node-ptr*"] != -1} {
          regexp "ptr.*" $tags ptr
       } elseif {[lsearch $tags "node-*"] != -1} {
          set i [lsearch $tags "node-*"]
          set tag [lindex $tags $i]
          regexp "node-(.*)" $tag match node
          return [Tree:gettooltip $w $node]
       }
       set ptr [lindex $ptr 0]

       if {$ptr!="" && $ptr!=[opp_null]} {
          set tip "([opp_getobjectclassname $ptr]) [opp_getobjectfullname $ptr] [opp_getobjectinfostring $ptr]"
          regsub {  +} $tip {  } tip
          set dispstr ""
          if {[lsearch $tags "modname"] == -1} {
             catch { set dispstr [opp_getobjectfield $ptr displayString] }
          } else {
             # if it has tag "modname", it is the enclosing module
             catch { set dispstr [opp_getobjectfield $ptr backgroundDisplayString] }
          }
          set tt_tag [opp_displaystring $dispstr getTagArg "tt" 0]
          if {$tt_tag!=""} {
             append tip "\n$tt_tag"
          }
          return $tip
       }
   }
   return ""
}


proc inspector_createnotebook {w} {
    set nb $w.nb
    notebook $nb
    $nb config -width 460 -height 260
    pack $nb -expand 1 -fill both
    return $nb
}


proc create_structpanel {w} {
    # TBD textarea is a temporary solution -- should be sth like a property sheet.
    scrollbar $w.sb -borderwidth 1 -command "$w.txt yview"
    text $w.txt  -height 12 -width 40 -yscrollcommand "$w.sb set"
    $w.txt configure -wrap word -spacing3 2
    $w.txt tag configure field -lmargin2 90

    pack $w.sb -anchor center -expand 0 -fill y -side right
    pack $w.txt -anchor center -expand 1 -fill both -side left
}

