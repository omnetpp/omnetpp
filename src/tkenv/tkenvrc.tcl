#=================================================================
#  TKENVRC.TCL - part of
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



# saveTkenvrc --
#
#
proc saveTkenvrc {fname isglobal atexit {comment ""}} {
    global config

    if [catch {
        set fout [open $fname w]
        puts $fout $comment
        if {$isglobal} {
            foreach key {
                updatefreq_fast_ms
                updatefreq_express_ms
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
                stripnamespace
                logformat
                loglevel
                scrollbacklimit
                logbuffer_maxnumevents
            } {
                set value [opp_getsimoption $key]
                set value [string map {"\n" "\x2"} $value]
                puts $fout "option $key\t{$value}"
            }

            storeMainwinGeometry
            foreach key [lsort [array names config]] {
                if {![isLocalConfigKey $key]} {
                    set value $config($key)
                    set value [string map {"\n" "\x2"} $value]
                    puts $fout "config $key\t{$value}"
                }
            }

            set fonts {TimelineFont CanvasFont LogFont BIGFont BoldFont SimtimeFont EventnumFont TkDefaultFont TkTooltipFont TkTextFont TkFixedFont}
            foreach font $fonts {
                puts $fout "font $font\t[list [font actual $font]]"
            }

        } else {
            storeMainwinGeometry
            foreach key [lsort [array names config]] {
                if {[isLocalConfigKey $key]} {
                    set value $config($key)
                    set value [string map {"\n" "\x2"} $value]
                    puts $fout "config $key\t{$value}"
                }
            }

            puts $fout [inspectorList:tkenvrcGetContents $atexit]
        }

        close $fout

    } err] {
       tk_messageBox -icon error -type ok -message "Error: $err" -title "Error"
       return
    }
}

proc isLocalConfigKey key {
    set localKeys {default-configname default-runnumber}
    return [lcontains $localKeys $key]
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

    set orient [.main.right cget -orient]
    set config(mainwin-sash-orient) $orient

    set config(mainwin-main-sashpos)  [panedwindow:getSashPosition .main]
    set config(mainwin-left-sashpos)  [panedwindow:getSashPosition .main.left]
    set config(mainwin-right-sashpos-$orient) [panedwindow:getSashPosition .main.right]

    set lb .inspector.nb.contents.main.list
    inspectorListbox:storeColumnWidths $lb "inspector:columnwidths"

    set config(mainwin-log-mode) [LogInspector:getMode .log]
}

# loadTkenvrc --
#
#
proc loadTkenvrc {fname} {
    global config

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
                if {$key=="zoomby-factor" && $value==1.1} {set value 1.3} ;# for OMNeT++ 4.5 --> 4.6 transition
                set config($key) $value
            } elseif {$cat == "font"} {
                set font [lindex $line 1]
                set attrs [lindex $line 2]
                catch {eval font configure $font $attrs}
            } elseif {[llength $line]==4} {
                # old tkenvrc, patch it up
                inspectorList:tkenvrcProcessLine [concat "inspector" $line]
            }
        }] {
            tk_messageBox -icon warning -type ok -title "Error reading .tkenvrc" -message "$fname line $lineno is invalid, ignoring"
        }
        incr lineno
    }
    close $fin

    inspectorList:openInspectors
    applyTkenvrc
}

# applyTkenvrc --
#
# Invoked on loading the tkenvrc file.
#
proc applyTkenvrc {} {
    global config

    catch {wm state . $config(mainwin-state)}
    catch {wm geometry . $config(mainwin-geom)}

    catch {.main.right config -orient $config(mainwin-sash-orient)}

    set orient [.main.right cget -orient]
    toolbutton:setsunken .toolbar.vert  [expr {$orient=="vertical"}]
    toolbutton:setsunken .toolbar.horiz [expr {$orient!="vertical"}]

    set orient [.main.right cget -orient]
    panedwindow:setSashPosition .main $config(mainwin-main-sashpos)
    panedwindow:setSashPosition .main.left $config(mainwin-left-sashpos)
    panedwindow:setSashPosition .main.right $config(mainwin-right-sashpos-$orient)

    set lb .inspector.nb.contents.main.list
    inspectorListbox:restoreColumnWidths $lb "inspector:columnwidths"

    after idle "LogInspector:setMode .log $config(mainwin-log-mode)"

    toggleStatusDetails
    toggleStatusDetails

    toggleTimeline
    toggleTimeline

    updateTkpFonts
    reflectSettingsInGui
}

# reflectSettingsInGui --
#
# Invoked whenever some preference setting is changed, e.g. on closing the
# Simulation Options dialog.
#
proc reflectSettingsInGui {} {
   global config help_tips

   catch {.log.main.text config -wrap $config(editor-wrap)}
   catch {.log.main.text tag configure "prefix" -elide $config(editor-hideprefix)}

   set h [font metrics TkDefaultFont -displayof . -linespace]
   set h [expr $h+2]
   ttk::style configure Treeview -rowheight $h

   timeline:fontChanged

   redrawTimeline

   opp_redrawinspectors

}
