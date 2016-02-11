//==========================================================================
//  QTENVDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Qtenv library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_QTENVDEFS_H
#define __OMNETPP_QTENV_QTENVDEFS_H

#include "omnetpp/platdep/platdefs.h"
#include "omnetpp/platdep/platmisc.h"
#include "omnetpp/platdep/timeutil.h"

// a simple "class cObject;" is not enough for Q_DECLARE_METATYPE, it has to be fully defined
#include "omnetpp/cobject.h"
#include "omnetpp/cmessage.h"

// this prevents the definition of "emit" as a macro
// it is needed because OMNeT uses "emit" as a function name
// if you want to use "emit" the Qt way after including this header
// just define "emit" as empty, like in qobjectdefs.h
#define QT_NO_EMIT

#include <QMetaType>
#include <QPair>

namespace omnetpp {
namespace qtenv {
class Inspector;
}
}

Q_DECLARE_METATYPE(omnetpp::cObject*)
Q_DECLARE_METATYPE(omnetpp::cMessage*)

#if QT_VERSION >= 0x050000
// for Q_DECLARE_METATYPE in Qt5, omnetpp::qtenv::Inspector would have to be fully defined, which we don't want here
    Q_DECLARE_OPAQUE_POINTER(omnetpp::qtenv::Inspector*)
#else
    Q_DECLARE_METATYPE(omnetpp::qtenv::Inspector*)
#endif

typedef QPair<omnetpp::cObject*, int> ActionDataPair;
typedef QPair<ActionDataPair, omnetpp::qtenv::Inspector*> ActionDataTriplet;

Q_DECLARE_METATYPE(ActionDataPair)
Q_DECLARE_METATYPE(ActionDataTriplet)

#if defined(QTENV_EXPORT)
#  define QTENV_API OPP_DLLEXPORT
#elif defined(QTENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define QTENV_API OPP_DLLIMPORT
#else
#  define QTENV_API
#endif

#endif

