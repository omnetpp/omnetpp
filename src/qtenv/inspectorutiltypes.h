//==========================================================================
//  INSPECTORUTILTYPES.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_INSPECTORUTILTYPES_H
#define __OMNETPP_QTENV_INSPECTORUTILTYPES_H

namespace omnetpp {
namespace qtenv {

/**
 * Inspector types
 */
enum InspectorType {
    INSP_DEFAULT,
    INSP_OBJECT,
    INSP_GRAPHICAL,
    INSP_LOG,
    INSP_OBJECTTREE,
    NUM_INSPECTORTYPES   // this must be the last one
};


enum eCopy
{
    COPY_PTR,
    COPY_PTRWITHCAST,
    COPY_FULLPATH,
    COPY_FULLNAME,
    COPY_CLASSNAME
};

enum eTab
{
    TAB_GENERAL = 0,
    TAB_LOGS = 1,
    TAB_LAYOUTING = 2,
    TAB_ANIMATION = 3,
    TAB_FILTERING = 4,
    TAB_FONTS = 5,
    TAB_NOTDEFINED = -1
};

}  // namespace qtenv
}  // namespace omnetpp

#endif

