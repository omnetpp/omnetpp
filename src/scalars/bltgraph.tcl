#==========================================================================
#  BLTGRAPH.TCL -
#            part of OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 2004 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


#FIXME add "close" icon to plove incons in _icons!!!!!

set graphnumber 0

proc createBltGraph {graphtype {graphname ""}} {

    global help_tips icons
    global graphnumber

    if {$graphtype!="graph" && $graphtype!="barchart"} {error "invalid graphtype: must be graph of barchart"}

    # create container window if doesn't exist yet
    set w .bltwin
    if ![winfo exist $w] {
        toplevel $w
        wm deiconify $w
        wm title $w "Charts"
        wm protocol $w WM_DELETE_WINDOW {bltGraph_CloseWindow}
        wm geometry $w "640x540"

        # toolbar
        set toolbar $w.toolbar
        frame $toolbar -relief raised -borderwidth 1
        foreach i {
          {sep0    -separator}
          {copy    -image $icons(copy)     -command bltGraph_Copy}
          {savepic -image $icons(savepic)  -command bltGraph_SavePicture}
          {saveps  -image $icons(savepic)  -command bltGraph_SavePostscript}
          {sep1    -separator}
          {zoomout -image $icons(find)     -command bltGraph_ZoomOut}
          {sep2    -separator}
          {config  -image $icons(config)   -command bltGraph_Properties}
        } {
          set b [eval iconbutton $toolbar.$i]
          pack $b -anchor n -expand 0 -fill none -side left -padx 0 -pady 2
        }

        # close button
        set b [button $toolbar.close -image $icons(close) -command bltGraph_Close -relief flat]
        pack $b -anchor n -expand 0 -fill none -side right -padx 0 -pady 2

        set help_tips($toolbar.copy) {Copy to clipboard}
        set help_tips($toolbar.savepic) {Save picture...}
        set help_tips($toolbar.saveps) {Save Postscript...}
        set help_tips($toolbar.zoomout) {Zoom out}
        set help_tips($toolbar.config) {Properties...}

        set help_tips($toolbar.close)   {Close}

        pack $toolbar -side top -fill x

        # tabnotebook
        blt::tabnotebook $w.nb -tearoff no -relief flat
        pack $w.nb -expand 1 -fill both

        bind $w <1>      {.popup unpost}
        bind $w <Escape> {.popup unpost}
        bind $w.nb <3>   {%W select active; .popup post %X %Y}
    }

    # create graph on a page
    incr graphnumber
    if {$graphname==""} {set graphname "Chart $graphnumber"}
    set tabId [$w.nb insert end -text $graphname]
    set page $w.nb.page$tabId
    frame $page
    $w.nb tab configure $tabId -window $page -fill both
    $w.nb select $tabId

    set graph $page.g

    if {$graphtype=="graph"} {

        blt::graph $graph

        $graph pen configure "activeLine" -color navy -linewidth 1 -symbol none
        $graph crosshairs configure -dashes {1 1}
        $graph legend configure -position right
        #$graph legend configure -position bottom
        $graph legend configure -font {"Small Fonts" 7}

    } elseif {$graphtype=="barchart"} {

        blt::barchart $graph

        $graph crosshairs configure -dashes {1 1}
        $graph legend configure -position right
        #$graph legend configure -position bottom
        $graph legend configure -font {"Small Fonts" 7}
        $graph configure -barmode aligned
    }
    pack $graph -expand 1 -fill both

    Blt_ActiveLegend $graph
    blt::ZoomStack $graph ButtonPress-1 ButtonPress-2
    blt::Crosshairs $graph
    blt::ClosestPoint $graph

    bind $graph <3>  {.popup post %X %Y}
    return $graph
}

proc createBltGraphPopup {} {
    menu .popup -tearoff 0
    foreach i {
      {command -label {Zoom out} -underline 0 -command bltGraph_ZoomOut}
      {separator}
      {command -label {Titles...} -underline 0    -command {bltGraph_Properties titles} }
      {command -label {Axes...} -underline 0      -command {bltGraph_Properties axes} }
      {command -label {Gridlines...} -underline 0 -command {bltGraph_Properties gridlines} }
      {command -label {Lines...} -underline 0     -command {bltGraph_Properties lines} }
      {command -label {Legend...} -underline 1    -command {bltGraph_Properties legend} }
      {separator}
      {command -label {Copy to clipboard} -underline 0  -command bltGraph_Copy}
      {command -label {Save picture...} -underline 0    -command bltGraph_SavePicture}
      {command -label {Save Postscript...} -underline 0 -command bltGraph_SavePostscript}
      {separator}
      {command -label {Close} -underline 2 -command bltGraph_Close}
    } {
       eval .popup add $i
    }
}

proc bltGraph_Close {} {
    # delete tab
    set w .bltwin
    set f [$w.nb tab cget select -window]

    # delete vectors used by the graph
    set graph $f.g
    foreach e [$graph element names] {
       # note "catch": not sure -x is was given as BLT vector name -- Tcl list also possible
       catch {[$graph element cget $e -x] delete}
       catch {[$graph element cget $e -y] delete}
    }

    # delete tab page (and graph)
    $w.nb delete select
    destroy $f

    # with the last tab, close whole window
    if {[$w.nb size]==0} {
        destroy $w
    }
}

proc bltGraph_CloseWindow {} {
    # delete all graphs (last graph will remove window)
    set w .bltwin
    while {[winfo exist $w]} {
        bltGraph_Close
    }
}

proc bltGraph_Copy {} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g

    if [catch {$graph snap -format emf CLIPBOARD}] {
        tk_messageBox -icon error -type ok -title Error
             -message "Sorry, copying the graph to the clipboard only works on Windows."
    }
}

proc bltGraph_ZoomOut {} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g
    blt::ResetZoom $graph
}

proc bltGraph_SavePicture {} {

}

proc bltGraph_SavePostscript {} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g
    # FIXME
    $graph postscript output "graph.ps"
}

proc bltGraph_Properties {{what "lines"}} {
    set w .bltwin
    set graph [$w.nb tab cget select -window].g
    bltGraph_PropertiesDialog $graph $what
}

proc bltGraph_PropertiesDialog {graph {what "lines"}} {
    set w .graphprops
    createOkCancelDialog $w "Graph Properties"
    wm geometry $w 360x300

    # tabnotebook
    set nb $w.f.nb
    blt::tabnotebook $nb -tearoff no -relief flat
    pack $nb -expand 1 -fill both
    foreach {tab title} {titles "Titles" axes "Axes" gridlines "Gridlines" lines "Lines" legend "Legend"} {
        frame $nb.$tab
        set tabs($tab) [$nb insert end -text $title -window $nb.$tab  -fill both]
    }
    $nb select $tabs($what)

    # Titles page
    set f $nb.titles
    label-entry $f.title "Graph title"
    pack $f.title -side top -anchor w -expand 0 -fill x
    #...

    # Axes page
    set f $nb.axes
    label-entry $f.xlabel "X axis label"
    label-entry $f.ylabel "Y axis label"
    pack $f.xlabel $f.ylabel -side top -anchor w -expand 0 -fill x
    #...

    # Gridlines page
    set f $nb.gridlines
    label-combo $f.display "Grid lines" {off {at major ticks} {at all ticks}}
    pack $f.display -side top -anchor w -expand 0 -fill x

    # Lines page
    set f $nb.lines
    #...

    # Legend page
    set f $nb.legend
    label-combo $f.pos "Position" {left right top bottom}
    label-combo $f.anchor "Anchor" {n s e w ne nw se sw}
    pack $f.pos $f.anchor -side top -anchor w -expand 0 -fill x
    #...

    # execute dialog
    if {[execOkCancelDialog $w] == 1} {
        #...
        puts "OK!"
    }
    destroy $w
}


