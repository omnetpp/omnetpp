#=================================================================
#  INSPECTORUTIL.TCL - part of
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


proc textWindowAddIcons {insp {wintype ""}} {
    global icons help_tips CTRL_

    packToolbutton $insp.toolbar.copy   -image $icons(copy) -command "editCopy $insp.main.text"
    packToolbutton $insp.toolbar.find   -image $icons(find) -command "findDialog $insp.main.text"
    packToolbutton $insp.toolbar.save   -image $icons(save) -command "saveFile $insp"
    if {$wintype=="modulewindow"} {
        packToolbutton $insp.toolbar.filter -image $icons(filter) -command "editFilterWindowContents $insp"
    }
    packToolbutton $insp.toolbar.sep21  -separator

    set help_tips($insp.toolbar.copy)   "Copy selected text to clipboard (${CTRL_}C)"
    set help_tips($insp.toolbar.find)   "Find string in window (${CTRL_}F)"
    set help_tips($insp.toolbar.save)   "Save window contents to file"
    set help_tips($insp.toolbar.filter) "Filter window contents (${CTRL_}H)"
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
           tk_messageBox -type ok -icon info -title "Info" -message "Context menu inspector rule $rulename should contain 4 items, ignoring."
       } else {
           set contextmenurules($i,label)   [lindex $line 0]
           set contextmenurules($i,context) [lindex $line 1]
           set contextmenurules($i,name)    [lindex $line 2]
           set contextmenurules($i,class)   [lindex $line 3]
       }
       incr i
    }
}

proc fillInspectorContextMenu {menu insp ptr} {
    global contextmenurules

    # ptr should never be null, but check it anyway
    if [opp_isnull $ptr] {return $menu}

    # add "Go Into" if applicable
    set name [opp_getobjectfullname $ptr]
    if {$insp!="" && $ptr!=[opp_inspector_getobject $insp] && [opp_inspector_supportsobject $insp $ptr]} {
        $menu add command -label "Go Into '$name'" -command [list opp_inspector_setobject $insp $ptr]
        $menu add separator
    }

    # add inspector types supported by the object
    set insptypes [opp_supported_insp_types $ptr]
    foreach type $insptypes {
       set label "[getInspectMenuLabel $type] for '$name'"
       $menu add command -label $label -command [list opp_inspect $ptr $type]
    }

    # add "run until" menu items
    set baseclass [opp_getobjectbaseclass $ptr]
    if {$baseclass=="cSimpleModule" || $baseclass=="cModule"} {
        $menu add separator
        $menu add command -label "Run Until Next Event in Module '$name'" -command [list runSimulationLocal $insp normal $ptr]
        $menu add command -label "Fast Run Until Next Event in Module '$name'" -command [list runSimulationLocal $insp fast $ptr]
    }

    if {$baseclass=="cMessage"} {
        $menu add separator
        $menu add command -label "Run Until Delivery of Message '$name'" -command [list runUntilMsg $ptr normal]
        $menu add command -label "Fast Run Until Delivery of Message '$name'" -command [list runUntilMsg $ptr fast]
        $menu add command -label "Express Run Until Delivery of Message '$name'" -command [list runUntilMsg $ptr express]
        $menu add separator
        $menu add command -label "Exclude Messages Like '$name' From Animation" -command [list excludeMessageFromAnimation $ptr]
    }

    # add utilities menu
    set submenu .copymenu$ptr
    catch {destroy $submenu}
    menu $submenu -tearoff 0
    $menu add separator
    $menu add cascade -label "Utilities for '$name'" -menu $submenu
    $submenu add command -label "Copy Pointer With Cast (for Debugger)" -command [list copyToClipboard $ptr ptrWithCast]
    $submenu add command -label "Copy Pointer Value (for Debugger)" -command [list copyToClipboard $ptr ptr]
    $submenu add separator
    $submenu add command -label "Copy Full Path" -command [list copyToClipboard $ptr fullPath]
    $submenu add command -label "Copy Name" -command [list copyToClipboard $ptr fullName]
    $submenu add command -label "Copy Class Name" -command [list copyToClipboard $ptr className]

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
       # Note: we have to quote the pattern for the object matcher, because $name might contain spaces
       set objlist [opp_getsubobjectsfilt $ptr $allcategories $contextmenurules($key,class) "\"$name.$contextmenurules($key,name)\"" 1 ""]
       if {$objlist!={}} {
           if {$first} {
               set first 0
               $menu add separator
           }
           $menu add command -label "$contextmenurules($key,label)..." -command "inspectContextMenuRules $ptr $key"
       }
    }
}

proc createInspectorContextMenu {insp ptrs} {

    # create popup menu
    catch {destroy .popup}
    menu .popup -tearoff 0

    if [opp_isinspector $insp] {
        # If there are more than one ptrs, remove the inspector object's own ptr:
        # when someone right-clicks a submodule icon, we don't want the compound
        # module to be in the list.
        if {[llength $ptrs] > 1} {
            set ptr [opp_inspector_getobject $insp]
            set idx [lsearch -exact $ptrs $ptr]
            set ptrs [lreplace $ptrs $idx $idx]  ;# no-op if $idx==-1
        }
    }

    if {[llength $ptrs] == 1} {
        fillInspectorContextMenu .popup $insp $ptrs
    } elseif {[llength $ptrs] > 1} {
        # then create a submenu for each object
        foreach ptr $ptrs {
            set submenu .popup.$ptr
            catch {destroy $submenu}
            menu $submenu -tearoff 0
            set name [opp_getobjectfullname $ptr]
            set shorttypename [opp_getobjectshorttypename $ptr]
            set infostr "$shorttypename, [opp_getobjectinfostring $ptr]"
            if {[string length $infostr] > 30} {
                set infostr [string range $infostr 0 29]...
            }
            set baseclass [opp_getobjectbaseclass $ptr]
            if {$baseclass == "cGate" } {
                set nextgateptr [opp_getobjectfield $ptr "nextGate"]
                set nextgatename [opp_getobjectfullname $nextgateptr]
                set ownerptr [opp_getobjectowner $ptr]
                set ownername [opp_getobjectfullname $ownerptr]
                set nextgateownerptr [opp_getobjectowner $nextgateptr]
                set nextgateownername [opp_getobjectfullname $nextgateownerptr]

                set label "$ownername.$name --> $nextgateownername.$nextgatename"
            } elseif {$baseclass == "cMessage" } {
                set shortinfo [opp_getmessageshortinfostring $ptr]
                set label "$name ($shorttypename, $shortinfo)"
            } else {
                set label "$name ($infostr)"
            }
            fillInspectorContextMenu $submenu $insp $ptr
            .popup add cascade -label $label -menu $submenu
        }
    }

    if [opp_isinspector $insp] {
       set ptr [opp_inspector_getobject $insp]
       if [opp_isnotnull $ptr] {
          set parentptr [opp_getobjectparent $ptr]
          if {[opp_isnotnull $parentptr] && [opp_inspector_supportsobject $insp $parentptr]} {
              .popup add separator
              .popup add command -label "Go Up" -command "opp_inspector_setobject $insp $parentptr"
          }
       }
    }

    return .popup
}

proc copyToClipboard {ptr what} {
    regsub {^ptr} $ptr {0x} p
    switch $what {
        ptr         {setClipboard $p}
        ptrWithCast {setClipboard "(([opp_getobjectfield $ptr className] *)$p)"}
        fullPath    {setClipboard [opp_getobjectfullpath $ptr]}
        fullName    {setClipboard [opp_getobjectfullname $ptr]}
        className   {setClipboard [opp_getobjectfield $ptr className]}
        default     {error "invalid value '$what'"}
    }
}

proc inspectContextMenuRules {ptr key} {
    global contextmenurules
    set allcategories "mqsgvo"
    set name [opp_getobjectfullpath $ptr]
    set objlist [opp_getsubobjectsfilt $ptr $allcategories $contextmenurules($key,class) "$name.$contextmenurules($key,name)" 100 ""]
    if {[llength $objlist] > 5} {
        tk_messageBox -type ok -icon info -title "Info" -message "This matches [llength $objlist]+ objects, opening inspectors only for the first five."
        set objlist [lrange $objlist 0 4]
    }
    foreach objptr $objlist {
        opp_inspect $objptr
    }
}

proc getInspectMenuLabel {typecode} {
    array set ary {
        INSP_DEFAULT      "Open Best View"
        INSP_OBJECT       "Open Details"
        INSP_GRAPHICAL    "Open Graphical View"
        INSP_MODULEOUTPUT "Open Component Log"
    }
    return $ary($typecode)
}

proc inspectThis {insp type} {
    # extract object pointer from window path name and create inspector
    set object [opp_inspector_getobject $insp]
    opp_inspect $object $type
}

proc inspectComponentType {insp {type ""}} {
    # extract object pointer from window path name and create inspector
    set ptr [opp_inspector_getobject $insp]
    set typeptr [opp_getcomponenttypeobject $ptr]
    opp_inspect $typeptr $type
}

#
# Called from balloon.tcl, supposed to return tooltip for a widget (or items
# in a widget). Installed via: set help_tips(helptip_proc) getHelpTip
#
# Here we produce help text for canvas items that represent simulation
# objects.
#
proc getHelpTip {w x y} {
   if {![winfo exists $w]} {
       return ""
   }
   set tip ""

   if {[winfo class $w]=="Canvas" || [winfo class $w]=="PathCanvas"} {
       set canvasx [$w canvasx $x]
       set canvasy [$w canvasy $y]
       set items [$w find overlapping [expr $canvasx-2] [expr $canvasy-2] [expr $canvasx+2] [expr $canvasy+2]]

       set ptrs {}
       foreach item $items {
           # if this is a simulation object, get its pointer
           set ptr ""
           set tags [$w gettags $item]
           if {[lsearch $tags "tooltip"] == -1} {
               continue
           }

           if {[lsearch $tags "ptr*"] != -1} {
               regexp "ptr.*" $tags ptr
           } elseif {[lsearch $tags "qlen-ptr*"] != -1} {
               regexp "ptr.*" $tags modptr
               set ptr [ModuleInspector:qlenGetQptr [winfo parent $w] $modptr] ;# Khmm...
           } elseif {[lsearch $tags "node-ptr*"] != -1} {
               regexp "ptr.*" $tags ptr
           } elseif {[lsearch $tags "node-*"] != -1} {
               set i [lsearch $tags "node-*"]
               set tag [lindex $tags $i]
               regexp "node-(.*)" $tag match node
               return [Tree:gettooltip $w $node]
           }
           set ptr [lindex $ptr 0]
           if [opp_isnotnull $ptr] {
               lappend ptrs $ptr
           }
       }

       # remove duplicate pointers and reverse the order
       # so the topmost element will be the first in the list
       set ptrs [lreverse [uniq $ptrs]]

       set tip ""
       foreach ptr $ptrs {
           # append type, name, and info string
           append tip "([opp_getobjectshorttypename $ptr]) [opp_getobjectfullname $ptr]"
           set info [opp_getobjectinfostring $ptr]
           if {$info!=""} {append tip ", $info"}
           regsub {  +} $tip {  } tip

           # if it's a module, append display string tooltip tag (tt=, bgtt=)
           if {[lsearch $tags "modname"] == -1} {
               set dispstr ""
               catch { set dispstr [opp_getobjectfield $ptr displayString] }
               set tt_tag [opp_displaystring $dispstr getTagArg "tt" 0 $ptr 1]
           } else {
               # if it has tag "modname", it is the enclosing module
               set dispstr ""
               catch { set dispstr [opp_getobjectfield $ptr displayString] }
               set tt_tag [opp_displaystring $dispstr getTagArg "bgtt" 0 $ptr 0]
           }
           if {$tt_tag!=""} {
               append tip "\n  $tt_tag"
           }
           append tip "\n"
       }
   }
   return [string trim $tip \n]
}


