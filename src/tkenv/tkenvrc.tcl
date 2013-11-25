#=================================================================
#  TKENVRC.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2008 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#



# saveTkenvrc --
#
#
proc saveTkenvrc {fname savesettings saveinspectors atexit {comment ""}} {
    global config fonts defaultfonts

    if [catch {
        set fout [open $fname w]
        puts $fout $comment
        if {$savesettings} {
            foreach key {
                updatefreq_fast_ms
                updatefreq_express_ms
                stepdelay
                use_mainwindow
                event_banners
                init_banners
                short_banners
                animation_enabled
                nexteventmarkers
                senddirect_arrows
                anim_methodcalls
                methodcalls_delay
                animation_msgnames
                animation_msgclassnames
                animation_msgcolors
                silent_event_filters
                penguin_mode
                showlayouting
                layouterchoice
                arrangevectorconnections
                iconminsize
                bubbles
                animation_speed
                expressmode_autoupdate
                stoponmsgcancel
            } {
                set value [opp_getsimoption $key]
                set value [string map {"\n" "\x2"} $value]
                puts $fout "option $key\t{$value}"
            }

            storeMainwinGeometry
            foreach key [lsort [array names config]] {
                set value $config($key)
                set value [string map {"\n" "\x2"} $value]
                puts $fout "config $key\t{$value}"
            }

            foreach key [lsort [array names fonts]] {
                if {[info exists defaultfonts($key)] && $fonts($key)!=$defaultfonts($key)} {
                    set value $fonts($key)
                    puts $fout "fonts $key\t{$value}"
                }
            }
        }

        if {$saveinspectors} {
            puts $fout [inspectorList:tkenvrcGetContents $atexit]
        }

        close $fout

    } err] {
       tk_messageBox -icon error -type ok -message "Error: $err" -title {Error}
       return
    }
}

proc storeMainwinGeometry {} {
    global config

    set state [wm state .]
    set geom [wm geometry .]
    if {$state=="zoomed"} {
        # Workaround: with zoomed (maximized) windows, wm geometry returns the
        # the original (unzoomed) position but the zoomed size! We want to store
        # unzoomed size+position so that the window can be restored (unzoomed)
        # properly. Unfortunately the original size is lost forever, so we just
        # put in the default size instead.
        # Note: another command, "winfo geometry", would return the zoomed size
        # and the zoomed position, but it's even less use to us here.
        regsub {[0-9]+x[0-9]+(.*)} $geom {640x480\1} geom
    }
    set config(mainwin-state) $state
    set config(mainwin-geom) $geom
}

# loadTkenvrc --
#
#
proc loadTkenvrc {fname} {
    global config fonts

    if [catch {open $fname r} fin] {
        return
    }

    inspectorList:tkenvrcReset

    set lineno 1
    while {[gets $fin line] >= 0} {
        if {$line == ""} {incr lineno; continue}
        if [string match {#*} $line] {incr lineno; continue}
        set line [string map {"\x2" "\n"} $line]

        if [catch {
            set cat [lindex $line 0]
            if {$cat == "inspector"} {
                inspectorList:tkenvrcProcessLine $line
            } elseif {$cat == "option"} {
                set key [lindex $line 1]
                set value [lindex $line 2]
                catch {opp_setsimoption $key $value}  ;# catch: ignore obsolete entries
            } elseif {$cat == "config"} {
                set key [lindex $line 1]
                set value [lindex $line 2]
                set config($key) $value
            } elseif {$cat == "fonts"} {
                set key [lindex $line 1]
                set value [lindex $line 2]
                set value [actualFont [fixupFontName $value]] ;# some validation
                if {$value!=""} {
                    set fonts($key) $value
                }
            } elseif {[llength $line]==4} {
                # old tkenvrc, patch it up
                inspectorList:tkenvrcProcessLine [concat "inspector" $line]
            }
        }] {
            tk_messageBox -icon warning -type ok -title {Error reading .tkenvrc} -message "$fname line $lineno is invalid, ignoring"
        }
        incr lineno
    }
    close $fin

    set fonts(bold)     $fonts(normal)
    set fonts(balloon)  $fonts(normal)
    set fonts(timeline) $fonts(canvas)

    inspectorList:openInspectors
    reflectSettingsInGui
}


# reflectSettingsInGui --
#
#
proc reflectSettingsInGui {} {
   global config fonts help_tips

   catch {wm state . $config(mainwin-state)}
   catch {wm geometry . $config(mainwin-geom)}

   catch {.main.text config -wrap $config(editor-wrap)}

   applyFont Menubutton  $fonts(normal)
   applyFont Menu        $fonts(normal)
   applyFont Label       $fonts(normal)
   applyFont Message     $fonts(normal)
   applyFont Entry       $fonts(normal)
   applyFont Button      $fonts(normal)
   applyFont Checkbutton $fonts(normal)
   applyFont Radiobutton $fonts(normal)
   applyFont Scale       $fonts(normal)
   applyFont Labelframe  $fonts(normal)
   applyFont Canvas      $fonts(normal)
   applyFont Combobox    $fonts(normal)
   applyFont Listbox     $fonts(normal)
   applyFont Tabset      $fonts(normal)
   applyFont TreeView    $fonts(normal)
   applyFont Text        $fonts(text)

   option add *Menubutton.font  $fonts(normal)
   option add *Menu.font        $fonts(normal)
   option add *Label.font       $fonts(normal)
   option add *Message.font     $fonts(normal)
   option add *Entry.font       $fonts(normal)
   option add *Button.font      $fonts(normal)
   option add *Checkbutton.font $fonts(normal)
   option add *Radiobutton.font $fonts(normal)
   option add *Scale.font       $fonts(normal)
   option add *Labelframe       $fonts(normal)
   option add *Canvas.font      $fonts(normal)
   option add *Combobox.font    $fonts(normal)
   option add *ComboboxListbox.font $fonts(normal)
   option add *Listbox.font     $fonts(normal)
   option add *Tabset.font      $fonts(normal)
   option add *TreeView.font    $fonts(normal)
   option add *Text.font        $fonts(text)

   set help_tips(font)  $fonts(balloon)

   toggleTreeView
   toggleTreeView
   toggleTimeline
   toggleTimeline

   redrawTimeline

   opp_redrawinspectors
}

