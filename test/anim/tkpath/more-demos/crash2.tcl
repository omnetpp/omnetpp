package require tkpath

pack [tkp::canvas .c]

# "malformed bucked chain in Tcl_DeleteHashEntry"
# - if one of the items is a polygon with the "-outline" option, it will crash (their order doesn't matter)
# - it will not crash immediately if only one item (a polygon) is created -- then it will crash only on exit
# - only "polygon" items crash, apparently no others (not fully tested)
# - see below the list of options that will also crash the canvas in addition to "-outline"
#
# crashing options: -outline, -disabledoutline, -activeoutline; -outlinestipple, -disabledoutlinestipple, -activeoutlinestipple
# NON-crashing options: -width, -dash, -dashoffset, -outlineoffset, -fill, -smooth, -tags...
#
# The -outlinestipple options (e.g. -outlinestipple gray25) cause the following error (and crash):
# "Tk_FreeBitmap received unknown bitmap argument"
#
# cause of error is likely double-deletion in Tk_PathDeleteOutline() [generic/tkpCanvUtil.c]
#
.c create polygon {177 395 179 393 274 393} -outlinestipple gray25
.c create line 85 172 87 170

# any of the following will crash, as well as exiting the program
.c delete 1 2
#destroy .c
