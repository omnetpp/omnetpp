#==========================================================================
#  MENUPROC.TCL -
#            part of the GNED, the Tcl/Tk graphical topology editor of
#                            OMNeT++
#   By Andras Varga
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2003 Andras Varga
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

proc fileNewComponent {type} {
   global gned ned canvas

   # get nedfile of module on current canvas,
   # and create a new module in that file
   set canv_id $gned(canvas_id)
   set curmodkey $canvas($canv_id,module-key)
   set nedfilekey $ned($curmodkey,parentkey)

   set key [addItemWithUniqueName $type $nedfilekey]
   markNedfileOfItemDirty $key

   if {$type=="module"} {
       openModuleOnNewCanvas $key
   }

   updateTreeManager
}

proc fileOpen {{fname ""}} {
   global gned env canvas ned config

   if {[string compare [file tail $fname] $fname]==0} {
       set dir $config(default-dir)
   } else {
       set dir [file dirname $fname]
   }

   set fname [file tail $fname]
   set fname [tk_getOpenFile -defaultextension ".ned" \
              -initialdir $dir -initialfile $fname \
              -filetypes {{{NED files} {*.ned}} {{All files} {*}}}]

   if {$fname!=""} {
      set config(default-dir) [file dirname $fname]
      # regsub "^$env(HOME)/" $fname "~/" fname
      loadNED $fname
   }
}

proc fileSave {{nedfilekey {}}} {
   global ned gned canvas

   if {$nedfilekey==""} {
      # default: current canvas
      set canv_id $gned(canvas_id)
      set modkey $canvas($canv_id,module-key)
      set nedfilekey $ned($modkey,parentkey)

      if {$ned($nedfilekey,type)!="nedfile"} {error "internal error in fileSave"}
   }

   if {!$ned($nedfilekey,aux-isunnamed)} {
      saveNED $nedfilekey
      nedfileClearDirty $nedfilekey
   } else {
      fileSaveAs
   }
}

proc fileSaveAs {{nedfilekey {}}} {
   global gned canvas ned env config

   if {$nedfilekey==""} {
      # default: current canvas
      set canv_id $gned(canvas_id)
      set modkey $canvas($canv_id,module-key)
      set nedfilekey $ned($modkey,parentkey)
   }

   if {$ned($nedfilekey,filename)!=""} {
      set fname $ned($nedfilekey,filename)
   } elseif [info exist modkey] {
      set fname "$ned($modkey,name).ned"
   } else {
      set fname "unnamed.ned"
   }

   if {[string compare [file tail $fname] $fname]==0} {
       set dir $config(default-dir)
   } else {
       set dir [file dirname $fname]
   }

   set fname [file tail $fname]
   set fname [tk_getSaveFile -defaultextension ".ned" \
              -initialdir $dir -initialfile $fname \
              -filetypes {{{NED files} {*.ned}} {{All files} {*}}}]

   if {$fname!=""} {
      set config(default-dir) [file dirname $fname]
      # regsub "^$env(HOME)/" $fname "~/" fname

      set ned($nedfilekey,aux-isunnamed) 0
      set ned($nedfilekey,name) [makeFancyName $fname]
      set ned($nedfilekey,filename) $fname

      adjustWindowTitle
      updateTreeManager

      saveNED $nedfilekey
      nedfileClearDirty $nedfilekey
   }
}

proc fileSaveAll {} {
   global ned

   # save all ned files
   foreach key $ned(0,childrenkeys) {
       fileSave $key
   }
}

proc fileCloseNedfile {{nedfilekey {}}} {
   global canvas gned ned

   if {$nedfilekey==""} {
       # default: current canvas
       set canv_id $gned(canvas_id)
       set modkey $canvas($canv_id,module-key)
       set nedfilekey $ned($modkey,parentkey)
   }

   # all canvases belonging to this file must be in graphics mode
   # (or must be unchanged)
   foreach i [array names canvas "*,canvas"] {
       regsub -- ",canvas" $i "" loop_canvid
       set loop_modkey $canvas($loop_canvid,module-key)
       set loop_nedfilekey $ned($loop_modkey,parentkey)
       # does this canvas belong to the file to be closed?
       if {$loop_nedfilekey==$nedfilekey} {
           # does canvas contain unsaved text edits?
           if {$canvas($loop_canvid,mode)=="textedit" && [textCanvasContainsChanges $loop_canvid]}  {
               switchToCanvas $loop_canvid
               tk_messageBox -icon warning -type ok -title GNED \
                   -message "Switch back '$ned($loop_modkey,name)' to graphics mode first! \
                      The NED source has been edited, and your changes must be backparsed before the canvas can be closed."
               return
           }
       }
   }

   # offer saving it
   set canclose 1
   if [nedfileIsDirty $nedfilekey] {
       if {$ned($nedfilekey,aux-isunnamed)} {
          set reply [tk_messageBox -title "Last chance" -icon warning -type yesnocancel \
                -message "Unnamed file not saved yet. Save it now?"]
          if {$reply=="yes"} fileSave
       } else {
          set fname $ned($nedfilekey,filename)
          set fname [file tail $fname]
          set reply [tk_messageBox -title "Last chance" -icon warning -type yesnocancel \
                -message "File $fname contains unsaved changes. Save?"]
          if {$reply=="yes"} fileSave
       }
       if {$reply=="cancel"} {
          set canclose 0
       }
   }

   if {$canclose} {
       # delete from memory
       deleteNedfile $nedfilekey
       updateTreeManager
   }
}

proc fileCloseCanvas {} {
   closeCurrentCanvas
   updateTreeManager
}

proc fileImportXML {{fname ""}} {
   global gned env canvas ned config

   if {[string compare [file tail $fname] $fname]==0} {
       set dir $config(default-dir)
   } else {
       set dir [file dirname $fname]
   }

   set fname [file tail $fname]
   set fname [tk_getOpenFile -defaultextension ".xml" \
              -initialdir $dir -initialfile $fname \
              -filetypes {{{XML files} {*.xml}} {{All files} {*}}}]

   if {$fname!=""} {
      set config(default-dir) [file dirname $fname]
      # regsub "^$env(HOME)/" $fname "~/" fname
      loadXML $fname
   }
}

proc fileExportXML {{nedfilekey {}}} {
   global gned canvas ned env config

   if {$nedfilekey==""} {
      # default: current canvas
      set canv_id $gned(canvas_id)
      set modkey $canvas($canv_id,module-key)
      set nedfilekey $ned($modkey,parentkey)
   }

   if {$ned($nedfilekey,filename)!=""} {
      set fname $ned($nedfilekey,filename)
      regsub "\.ned$" $fname ".xml" fname
   } elseif [info exist modkey] {
      set fname "$ned($modkey,name).xml"
   } else {
      set fname "unnamed.xml"
   }

   if {[string compare [file tail $fname] $fname]==0} {
       set dir $config(default-dir)
   } else {
       set dir [file dirname $fname]
   }

   set fname [file tail $fname]
   set fname [tk_getSaveFile -defaultextension ".xml" \
              -initialdir $dir -initialfile $fname \
              -filetypes {{{XML files} {*.xml}} {{All files} {*}}}]

   if {$fname!=""} {
      set config(default-dir) [file dirname $fname]
      # regsub "^$env(HOME)/" $fname "~/" fname

      saveXML $nedfilekey $fname
   }
}

proc fileExit {} {
   global ned

   # close all ned files
   foreach key $ned(0,childrenkeys) {
#       if $ned($key,aux-isdirty) {
           fileCloseNedfile $key
           if [info exist ned($key,type)] {
                # don't exit if user cancelled closing a canvas
                return
           }
#       }
   }

   # ok, exit now
   saveConfig "~/.gnedrc"
   exit
}

proc editFind {} {
   global gned canvas

   set canv_id $gned(canvas_id)
   if {$canvas($canv_id,mode)=="graphics"} return
   findReplaceDialog $canvas($canv_id,textedit) find
}

proc editReplace {} {
   global gned canvas

   set canv_id $gned(canvas_id)
   if {$canvas($canv_id,mode)=="graphics"} return
   findReplaceDialog $canvas($canv_id,textedit) replace
}

proc editFindNext {} {
   global gned canvas

   set canv_id $gned(canvas_id)
   if {$canvas($canv_id,mode)=="graphics"} return
   findNext $canvas($canv_id,textedit)
}

proc editCut {} {

   tk_messageBox -title "GNED" -icon info -type ok -message "NED clipboard doesn't work yet. Sorry."
   return

   editCopy
   deleteSelected
}

proc editCopy {} {

   tk_messageBox -title "GNED" -icon info -type ok -message "NED clipboard doesn't work yet. Sorry."
   return

   global clipboard_ned ned

   set selection [selectedItems]

   # accept only submodules and connections whose both ends will be copied
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

   tk_messageBox -title "GNED" -icon info -type ok -message "NED clipboard doesn't work yet. Sorry."
   return

   global clipboard_ned ned gned canvas

   deselectAllItems

   # offset disp-xpos and disp-ypos
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
                 -message "Consistency Check not implemented yet.\
                           It should check that submodule parameters and gates\
                           are consistent with earlier module declarations and offer\
                           making automatic corrections."
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
    global gned config

    if {$setvar} {
        if {$config(snaptogrid)} {
            set config(snaptogrid) 0
        } else {
            set config(snaptogrid) 1
        }
    }

    if {$config(snaptogrid)} {
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

proc helpAbout {} {
    aboutDialog "About OMNeT++/GNED" {
OMNeT++/GNED

(C) 1997-2003 Andras Varga

NO WARRANTY -- see license for details.}
}


proc notImplemented {args} {
    # the most versatile function :-)
    set parent [winfo toplevel [focus]]
    tk_messageBox -type ok -icon info -parent $parent \
          -title "GNED" -message "Not implemented yet."
}
