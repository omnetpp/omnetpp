#==========================================================================
#  INSPLIST.TCL -
#            part of the Tcl/Tk windowing environment of
#                            OMNeT++
#==========================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2001 Andras Varga
#  Technical University of Budapest, Dept. of Telecommunications,
#  Stoczek u.2, H-1111 Budapest, Hungary.
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#

#
# Inspector list handling
#

#
# THIS PROC IS CALLED FROM C++ CODE, at each inspector display update.
#
proc inspectorupdate_callback {} {
    # experimental code:
    opp_inspectbyname {token.comp[0].sink.local-objects.queuing-time} {cOutVector} {As Graphics}

    #global inspectorlist inspectorkeys
    #foreach key $inspectorkeys {
    #    # objname, classname, insptype, geometry
    #}
}
