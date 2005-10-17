#=================================================================
#  HELP.TCL - part of
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

proc helpAbout {} {
    # implements Help|About
    global OMNETPP_RELEASE OMNETPP_EDITION
    aboutDialog "About Scalars" "
Scalars
Output Scalar Plotting Tool
" \
"Part of OMNeT++/OMNEST
(C) 1992-2005 Andras Varga
Release: $OMNETPP_RELEASE, edition: $OMNETPP_EDITION

NO WARRANTY -- see license for details."
}

