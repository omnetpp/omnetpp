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

// a simple "class cObject;" is not enough for Q_DECLARE_METATYPE, it has to be fully defined
#include "omnetpp/cobject.h"
#include "omnetpp/cmessage.h"

#include "inspectorutiltypes.h"
#include "omnetpp/clog.h"

// Require QT_NO_EMIT to prevent the definition of "emit" as a macro. It is needed
// because OMNeT++ uses "emit" as a function name. If you want to use "emit" the
// Qt way, define "emit" as empty, like in <qobjectdefs.h>.
#ifndef QT_NO_EMIT
#error "Qtenv must be compiled with -DQT_NO_EMIT, because the simkernel uses 'emit' as function name"
#endif

#include <QtCore/QMetaType>

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
#error Qtenv requires Qt 5.9 or later.
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#error Qtenv is not compatible with Qt 6.
#endif

namespace omnetpp {

class cObject;
class cComponent;
class cMessage;

namespace qtenv {
class Inspector;
}
}

// for Q_DECLARE_METATYPE in Qt5, omnetpp::qtenv::Inspector would have to be fully defined, which we don't want here
Q_DECLARE_OPAQUE_POINTER(omnetpp::qtenv::Inspector*)

// and for Q_DECLARE_OPAQUE_POINTER these classes would have to be subclasses of QObject which is not gonna happen...
Q_DECLARE_METATYPE(omnetpp::cObject*)
Q_DECLARE_METATYPE(omnetpp::cMessage*)


namespace omnetpp {
namespace qtenv {

enum RunMode {
    RUNMODE_NORMAL = 1,
    RUNMODE_FAST = 2,
    RUNMODE_EXPRESS = 3,
    RUNMODE_STEP = 4,
    RUNMODE_NOT_RUNNING = 5,
    // Within a pausePoint() call in a handleMessage()
    // Never actually set in Qtenv, only used to configure the GUI
    RUNMODE_PAUSED = 6,
};

// Custom types for QVariant
struct InspectActionData
{
    cObject *object;
    InspectorType type;
};

struct RunUntilActionData
{
    cObject *object;
    RunMode runMode;
};

struct RunUntilNextEventActionData
{
    cObject *object;
    RunMode runMode;
    Inspector *insp;
};

struct CopyActionData
{
    cObject *object;
    eCopy copy;
};

struct ComponentLogActionData
{
    cComponent *component;
    LogLevel logLevel;
};

}  // namespace qtenv
}  // namespace omnetpp

// In case of enum always use QVariant::fromValue method instead of implicit cast
// because it cause int cast and thus QVariant::value<enumtype> will give wrong value.
Q_DECLARE_METATYPE(omnetpp::qtenv::eTab)
Q_DECLARE_METATYPE(omnetpp::qtenv::InspectorType)
Q_DECLARE_METATYPE(omnetpp::qtenv::ComponentLogActionData)
Q_DECLARE_METATYPE(omnetpp::qtenv::InspectActionData)
Q_DECLARE_METATYPE(omnetpp::qtenv::CopyActionData)
Q_DECLARE_METATYPE(omnetpp::qtenv::RunUntilActionData)
Q_DECLARE_METATYPE(omnetpp::qtenv::RunUntilNextEventActionData)

#if defined(QTENV_EXPORT)
#  define QTENV_API OPP_DLLEXPORT
#elif defined(QTENV_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define QTENV_API OPP_DLLIMPORT
#else
#  define QTENV_API
#endif

#endif
