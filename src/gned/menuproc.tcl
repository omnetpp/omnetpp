#==========================================================================
#  MENUPROC.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#   By Andras Varga
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992,99 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc fileNewNedfile {} {
   global gned

   # create a new module in a new file
   set nedfilekey [addItem nedfile 0]
   set modkey [addItem module $nedfilekey]

   openModuleOnNewCanvas $modkey

   updateTreeManager
}

proc fileNewModule {} {
   global gned ned canvas

   # get nedfile of module on current canvas,
   # and create a new module in that file
   set canv_id $gned(canvas_id)
   set curmodkey $canvas($canv_id,module-key)
   set nedfilekey $ned($curmodkey,parentkey)

   set modkey [addItem module $nedfilekey]

   openModuleOnNewCanvas $modkey

   updateTreeManager
}

proc fileOpen {{fname ""}} {
   global gned env canvas ned

   catch {cd [file dirname $fname]}
   set fname [file tail $fname]
   set fname [tk_getOpenFile -defaultextension ".ned" \
              -initialdir [pwd] -initialfile $fname \
              -filetypes {{{NED files} {*.ned}} {{All files} {*}}}]

   if {$fname!=""} {
      # regsub "^$env(HOME)/" $fname "~/" fname

      set type [file extension $fname]
      if {$type==".ned"} {
         loadNED $fname
      } else {
         tk_messageBox -icon warning -type ok \
                       -message "Don't know how to open $type files."
      }
   }
}

proc fileSave {} {
   global ned gned canvas

   set canv_id $gned(canvas_id)
   set modkey $canvas($canv_id,module-key)
   set nedfilekey $ned($modkey,parentkey)

   if {$ned($nedfilekey,filename)!=""} {
      saveNED $nedfilekey
   } else {
      fileSaveAs
   }
}

proc fileSaveAs {} {
   global gned canvas ned env

   set canv_id $gned(canvas_id)
   set modkey $canvas($canv_id,module-key)
   set nedfilekey $ned($modkey,parentkey)

   if {$ned($nedfilekey,filename)!=""} {
      set fname $ned($nedfilekey,filename)
   } else {
      set fname "$ned($modkey,name).ned"
   }

   catch {cd [file dirname $fname]}
   set fname [file tail $fname]
   set fname [tk_getSaveFile -defaultextension ".ned" \
              -initialdir [pwd] -initialfile $fname \
              -filetypes {{{NED files} {*.ned}} {{All files} {*}}}]

   if {$fname!=""} {
      # regsub "^$env(HOME)/" $fname "~/" fname

      set ned($nedfilekey,filename) $fname
      adjustWindowTitle
      updateTreeManager

      saveNED $nedfilekey
   }
}

proc fileCloseNedfile {} {
   global canvas gned ned

   set canv_id $gned(canvas_id)
   set modkey $canvas($canv_id,module-key)
   set fkey $ned($modkey,parentkey)
   set fname $ned($fkey,name)

   if {$canvas($canv_id,changed)} {
       if {$ned($fkey,filename)==""} {
          set reply [tk_messageBox -title "Last chance" -icon warning -type yesno \
                -message "File not saved yet. Save it now?"]
          if {$reply=="yes"} fileSave
       } else {
          set fname $ned($fkey,filename)
          set fname [file tail $fname]
          set reply [tk_messageBox -title "Last chance" -icon warning -type yesno \
                -message "File $fname modified. Save?"]
          if {$reply=="yes"} fileSave
       }
   }

puts "proc fileCloseNedfile: to be checked!!!"
   MFileClose $fname

   updateTreeManager
}

proc fileCloseModule {} {
   closeCurrentCanvas
   updateTreeManager
}

proc fileExit {} {
   global gned

   # scan for unsaved canvases and call fileClose for them
   for {set canv_id 1} {$canv_id<=$gned(canvas_lastid)} {incr canv_id} {
      if {[info exist canvas($canv_id,canvas)]} {
         if {$canvas($canv_id,changed)} {
            if {$gned(canvas_id)!=$canv_id} {switchToCanvas $canv_id}
            fileClose
         }
      }
   }
   opp_exit
}

proc editCut {} {
   editCopy
   deleteSelected
}

proc editCopy {} {
   global clipboard_ned ned

   set selection [selectedItems]

   # accept only submodules and connections whose both end will be copied
   set keys {}
   foreach key $selection {
      if {$ned($key,type)=="submod"} {
         lappend keys $key
      } elseif {$ned($key,type)=="conn"} {
         if {[lsearch $selection $ned($key,src-ownerkey)]!=-1 &&
             [lsearch $selection $ned($key,dest-ownerkey)]!=-1} \
         {
             lappend keys $key
         }
      }
   }

   # copy out items to clipboard
   copyArrayFromNed clipboard_ned $keys
   return $keys
}

proc editPaste {} {
   global clipboard_ned ned gned canvas

   deselectAllItems

   # offset x-pos and y-pos
   foreach i [array names clipboard_ned "*,?-pos"] {
      set clipboard_ned($i) [expr $clipboard_ned($i)+8]
   }

   set new_keys [pasteArrayIntoNed clipboard_ned]

   # make items owned by the module on this canvas
   set modkey $canvas($gned(canvas_id),module-key)
   foreach key $new_keys {
      set ned($key,module-ownerkey) $modkey
      selectItem $key
   }

   # draw pasted items on canvas
   drawItems $new_keys
}

proc editDelete {} {
   deleteSelected
}

proc editCheck {} {
   tk_messageBox -title "GNED" -icon warning -type ok \
                 -message "Not implemented yet. Here we should check that \
                           submodule parameters and gates are consistent \
                           with earlier module declarations, and offer
                           to make automatic corrections."
}

proc viewManager {} {
  BringUpManager
}

#proc optionsLoadBackground {} {
#   global gned
#
#   tk_messageBox -title "Background files" -icon info -type ok \
#      -message "Background files are Tcl scripts that draw on canvas \$c. The file you specify will simply be sourced. You can load maps, floorplans etc."
#
#   set fname [tk_getOpenFile -defaultextension "tcl" -title "Load background" \
#              -filetypes {{{Tcl files} {*.tcl}}  {{All files} {*}}}]
#
#   if {$fname!=""} {
#       set c $gned(canvas)
#       if [catch {source $fname} errmsg] {
#          tk_messageBox -title GNED -icon warning -type ok -message "Error: $errmsg"
#       }
#   }
#}

proc toggleGrid {setvar} {
    global gned

    if {$setvar} {
        if {$gned(snaptogrid)} {
            set gned(snaptogrid) 0
        } else {
            set gned(snaptogrid) 1
        }
    }

    if {$gned(snaptogrid)} {
        $gned(toolbar).grid config -relief sunken
    } else {
        $gned(toolbar).grid config -relief raised
    }
}

proc optionsViewFile {} {
    global env

    set fname [tk_getOpenFile -defaultextension "" \
              -filetypes {{{All files} {*}}}]

    if {$fname!=""} {
       # regsub "^$env(HOME)/" $fname "~/" fname
       createFileViewer $fname
    }
}

proc optionsTCLConsole {} {
    set w .con
    if {[winfo exists .con]} {
        wm deiconify .con; return
    }

    toplevel .con
    wm minsize .con 375 160
    wm title .con "Tcl Console"
    frame .con.fra5 \
        -height 30 -width 30
    pack .con.fra5 \
        -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 2 -pady 2 \
        -side top
    text .con.fra5.tex7 \
        -highlightthickness 0 -state disabled -width 50 -height 6 \
        -yscrollcommand {.con.fra5.scr8 set}
    .con.fra5.tex7 tag configure command -underline 1
    .con.fra5.tex7 tag configure error -foreground red
    .con.fra5.tex7 tag configure output
    pack .con.fra5.tex7 \
        -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 0 -pady 0 \
        -side left
    scrollbar .con.fra5.scr8 \
        -command {.con.fra5.tex7 yview} -highlightthickness 0
    pack .con.fra5.scr8 \
        -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 \
        -side right
    frame .con.fra6 \
        -height 30 -width 30
    pack .con.fra6 \
        -anchor center -expand 0 -fill both -ipadx 0 -ipady 0 -padx 0 -pady 0 \
        -side top
    entry .con.fra6.ent10 \
        -highlightthickness 0
    pack .con.fra6.ent10 \
        -anchor center -expand 0 -fill x -ipadx 0 -ipady 0 -padx 2 -pady 2 \
        -side top
    bind .con.fra6.ent10 <Key-Return> {
        .con.fra5.tex7 conf -state normal
        .con.fra5.tex7 insert end \n[.con.fra6.ent10 get] command
        if { [catch [.con.fra6.ent10 get] output] == 1 } {
            .con.fra5.tex7 insert end "\n$output" error
        } else {
            .con.fra5.tex7 insert end "\n$output" output
        }
        .con.fra5.tex7 conf -state disabled
        .con.fra5.tex7 yview end
        .con.fra6.ent10 delete 0 end
    }
    focus .con.fra6.ent10
}

proc helpAbout {} {
    createOkCancelDialog .about "About OMNeT++/GNED"

    label .about.f.l -text \
{
GNED 1.2 ALPHA 1
Part of the OMNeT++ Discrete Event Simulator

(C) 1997,99 Andras Varga

NO WARRANTY. See Help|Release notes and the 'license' file for details.
}

    pack .about.f.l -anchor center -expand 0 -fill x -side top

    execOkCancelDialog .about
    destroy .about
}

proc helpRelNotes {} {
    global OMNETPP_GNED_DIR
    createFileViewer [file join $OMNETPP_GNED_DIR "readme"]
}


