# ven.tcl --
#     Simple demo using gradients and transparency
#
#     Note:
#     gradients do not mix with transparency, so use a transparent
#     rectangle with a uniform colour. A nice feature is, though,
#     that the gradients move and rotate with the canvas item.
#
package require tkpath

pack [::tkp::canvas .c -width 300 -height 300] -expand 1 -fill both

set coords {M 150 150 Q 200 110 250 140 L 240 200 Q 200 160 150 150} 

#.c create path $coords

set gradient [.c gradient create linear \
    -stops {{0 blue} {0.2 green} {0.4 yellow} {0.7 orange} {1.0 red}}]

.c create prect 50 50 400 100 -fill blue

.c create text 10 150 -anchor nw -text "Simple demo using gradients and transparency\nNote: gradients do not mix with transparency" -font "Arial 14"

set wings {}
foreach angle {0 60 120 180 240 300} {
    set angle [expr {acos(-1.0)*$angle/180.0}]
    
    set matrix [::tkp::transform rotate $angle 150 150]
    
    lappend wings [.c create path $coords -tag ven -matrix $matrix -fill $gradient]
}

.c create prect 200 150 400 400 -fillopacity 0.3 -fill blue ;#-fill $gradient 
.c create prect 250 200 450 450 -fillopacity 0.3 -fill green ;#-fill $gradient 

proc rotate {dangle} {
    global wings
    foreach item $wings {
        set angle  [expr {60*($item-1) + $dangle}]
        set angle  [expr {acos(-1.0)*$angle/180.0}]

        set matrix [::tkp::transform rotate $angle 150 150]
        .c itemconfigure $item -matrix $matrix
    }
    
    after 10 [list rotate [expr {$dangle + 1}]]
}

rotate 100
