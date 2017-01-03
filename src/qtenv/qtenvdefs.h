//==========================================================================
//  QTENVDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Qtenv library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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

// Require QT_NO_EMIT to prevent the definition of "emit" as a macro. It is needed
// because OMNeT++ uses "emit" as a function name. If you want to use "emit" the
// Qt way, define "emit" as empty, like in <qobjectdefs.h>.
#ifndef QT_NO_EMIT
#error "Qtenv must be compiled with -DQT_NO_EMIT, because the simkernel uses 'emit' as function name"
#endif

#include <QMetaType>
#include <QPair>

namespace omnetpp {
namespace qtenv {
class Inspector;
}
}

// for Q_DECLARE_METATYPE in Qt5, omnetpp::qtenv::Inspector would have to be fully defined, which we don't want here
Q_DECLARE_OPAQUE_POINTER(omnetpp::qtenv::Inspector*)

// and for Q_DECLARE_OPAQUE_POINTER these classes would have to be subclasses of QObject which is not gonna happen...
Q_DECLARE_METATYPE(omnetpp::cObject*)
Q_DECLARE_METATYPE(omnetpp::cMessage*)

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

