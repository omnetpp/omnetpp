# holes.tcl --
#     A path element seems to be very flexible. Can we use it to create polygons with holes?
#
package require tkpath

pack [::tkp::canvas .c -width 300 -height 300]

.c create text 150 150 -text "This is a long text\nin a large font" -font "Helvetica 24 bold"

#
# Note: the fillrule does the trick - with that the second square defines the hole
#
.c create path {M 40 40 L 260 40 L 260 260 L 40 260 L 40 40 M 100 100 L 200 100 L 200 200 L 100 200 L 100 100} \
    -fill blue -fillrule evenodd

.c create path {M 15 15 L 55 15 L 55 55 L 15 55 L 15 15 M 35 10 L 65 35 L 35 65 L 10 35 L 35 10} \
    -fill green -fillrule evenodd