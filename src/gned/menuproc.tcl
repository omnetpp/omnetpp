#=================================================================
#  MENUPROC.TCL - part of
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


proc fileNewNedfile {{fname ""}} {
   global gned ned

   # create a new module in a new file
   set nedfilekey [addItem nedfile 0]
   if {$fname!=""} {
       set ned($nedfilekey,name) $fname
       set ned($nedfilekey,filename) $fname
       set ned($nedfilekey,aux-isdirty) 1
   }
   set modkey [addItem module $nedfilekey]
   openModuleOnNewCanvas $modkey

   updateTreeManager
}

proc fileNewComponent {type {nedfilekey ""}} {
   global gned ned canvas

   if {$nedfilekey==""} {
       # get nedfile of module on current canvas,
       # and create a new module in that file
       set canv_id $gned(canvas_id)
       set curmodkey $canvas($canv_id,module-key)
       set nedfilekey $ned($curmodkey,parentkey)
   }

   set key [addItemWithUniqueName $type $nedfilekey]
   if {$type=="imports"} {
       # move to the top
       moveItemToTop $key
   }
   markNedFileOfItemDirty $key

   updateTreeManager

   if {$type=="module"} {
       openModuleOnNewCanvas $key
   }
   showTextOnceDialog "insert-$type"
   editProps $key
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
      # filename == "" means user cancelled
      loadNEDrec $fname
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

   # can only save file if all canvases are in graphics mode (or NED source not edited)
   if [nedfileContainsTextEdits $nedfilekey] {
      return
   }

   # actually save
   if {!$ned($nedfilekey,aux-isunnamed)} {
      saveNED $nedfilekey
      nedfileClearDirty $nedfilekey
   } else {
      fileSaveAs $nedfilekey
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

   # can only save file if all canvases are in graphics mode (or NED source not edited)
   if [nedfileContainsTextEdits $nedfilekey] {
      return
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

   # can only close file if all canvases are in graphics mode (or NED source not edited)
   if [nedfileContainsTextEdits $nedfilekey] {
      return
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

proc fileExportEPS {} {
   global gned canvas ned env config

   set canv_id $gned(canvas_id)
   set modkey $canvas($canv_id,module-key)

   if [info exist modkey] {
      set fname "$ned($modkey,name).eps"
   } else {
      set fname "unnamed.eps"
   }

   if {[string compare [file tail $fname] $fname]==0} {
       set dir $config(default-dir)
   } else {
       set dir [file dirname $fname]
   }

   set fname [file tail $fname]
   set fname [tk_getSaveFile -defaultextension ".eps" \
              -initialdir $dir -initialfile $fname \
              -filetypes {{{EPS files} {*.eps}} {{All files} {*}}}]

   if {$fname!=""} {
      set config(default-dir) [file dirname $fname]
      # regsub "^$env(HOME)/" $fname "~/" fname
      doExportCanvasToEPS $gned(canvas) $fname
   }
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
   busy "Closing canvases..."
   foreach key $ned(0,childrenkeys) {
#       if $ned($key,aux-isdirty) {
           fileCloseNedfile $key
           if [info exist ned($key,type)] {
                # don't exit if user cancelled closing a canvas
                busy
                return
           }
#       }
   }

   # ok, exit now
   saveConfig "~/.gnedrc"
   exit
}

proc newComponentMenuPopup {myicon} {
    global gned
    catch {destroy .popup}
    menu .popup -tearoff 0
    foreach i {
      {command -command "fileNewComponent imports" -label {Import} -underline 0}
      {command -command "fileNewComponent channel" -label {Channel} -underline 0}
      {command -command "fileNewComponent simple"  -label {Simple module}  -underline 0}
      {command -command "fileNewComponent module"  -label {Compound module}  -underline 0}
      {command -command "fileNewComponent network" -label {Network} -underline 0}
    } {
       eval .popup add $i
    }
    set w $gned(horiz-toolbar).$myicon
    .popup post [winfo rootx $w] [expr [winfo rooty $w]+[winfo height $w]]
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

proc editUndo {} {
   global gned canvas

   set canv_id $gned(canvas_id)
   if {$canvas($canv_id,mode)=="graphics"} {
       tk_messageBox -title "GNED" -icon info -type ok -message "Undo/redo not available in graphics mode."
       return
   }
   if [catch {text .tmp -undo true; destroy .tmp}] {
       tk_messageBox -title "GNED" -icon info -type ok -message "This version of Tcl/Tk doesn't support Undo/Redo in text widgets yet."
   }
   set w $canvas($canv_id,textedit)
   catch {$w edit undo}
   syntaxHighlight $w
}

proc editRedo {} {
   global gned canvas

   set canv_id $gned(canvas_id)
   if {$canvas($canv_id,mode)=="graphics"} {
       tk_messageBox -title "GNED" -icon info -type ok -message "Undo/redo not available in graphics mode."
       return
   }
   if [catch {text .tmp -undo true; destroy .tmp}] {
       tk_messageBox -title "GNED" -icon info -type ok -message "This version of Tcl/Tk doesn't support Undo/Redo in text widgets yet."
   }
   set w $canvas($canv_id,textedit)
   catch {$w edit redo}
   syntaxHighlight $w
}

proc editCut {} {
   global gned canvas

   set canv_id $gned(canvas_id)
   if {$canvas($canv_id,mode)=="graphics"} {
       nedClipboard:cut
   } else {
       tk_textCut $canvas($canv_id,textedit)
       syntaxHighlight $canvas($canv_id,textedit)
   }
}

proc editCopy {} {
   global gned canvas

   set canv_id $gned(canvas_id)
   if {$canvas($canv_id,mode)=="graphics"} {
       nedClipboard:copy
   } else {
       tk_textCopy $canvas($canv_id,textedit)
   }
}

proc editPaste {} {
   global gned canvas

   set canv_id $gned(canvas_id)
   if {$canvas($canv_id,mode)=="graphics"} {
       nedClipboard:paste
   } else {
       tk_textPaste $canvas($canv_id,textedit)
       syntaxHighlight $canvas($canv_id,textedit)
   }
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
        $gned(graphics-toolbar).grid config -relief sunken
    } else {
        $gned(graphics-toolbar).grid config -relief raised
    }
}

proc togglePortcheck {setvar} {
    if {$setvar} {
        showTextOnceDialog "nocheckSet"
    }
}

proc toggleAutosize {setvar} {
    if {$setvar} {
        showTextOnceDialog "autosizeSet"
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
    global OMNETPP_RELEASE OMNETPP_EDITION
    aboutDialog "About GNED" "
GNED
Graphical Network Editor
" \
"Part of OMNeT++/OMNEST
(C) 1992-2005 Andras Varga
Release: $OMNETPP_RELEASE, edition: $OMNETPP_EDITION

NO WARRANTY -- see license for details."
}


proc notImplemented {args} {
    # the most versatile function :-)
    set parent [winfo toplevel [focus]]
    tk_messageBox -type ok -icon info -parent $parent \
          -title "GNED" -message "Not implemented yet."
}
