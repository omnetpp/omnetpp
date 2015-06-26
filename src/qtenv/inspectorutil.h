//==========================================================================
//  INSPECTORUTIL.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef INSPECTORUTIL_H
#define INSPECTORUTIL_H

#include <QVector>
#include "qtenvdefs.h"

class QMenu;

namespace omnetpp {

class cObject;

namespace qtenv {

class Inspector;

class InspectorUtil
{
public:
    enum eCopy { COPY_PTR, COPY_PTRWITHCAST, COPY_FULLPATH, COPY_FULLNAME, COPY_CLASSNAME };

    InspectorUtil() {}

    static QMenu *fillInspectorContextMenu(cObject *object, Inspector *insp = nullptr);
    static QVector<int> supportedInspTypes(cObject *object);
    static void createContextMenu(std::vector<cObject*> objects);

};

} // namespace qtenv
} // namespace omnetpp

#endif // INSPECTORUTIL_H
