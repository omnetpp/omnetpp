//==========================================================================
//  INSPECTORUTIL.H - part of
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

#ifndef __OMNETPP_QTENV_INSPECTORUTIL_H
#define __OMNETPP_QTENV_INSPECTORUTIL_H

#include <QtCore/QVector>
#include "logbuffer.h"
#include "qtenvdefs.h"
#include "inspectorutiltypes.h"

class QMenu;
class QActionGroup;

namespace omnetpp {

class cObject;

namespace qtenv {

class Inspector;

class QTENV_API InspectorUtil
{
private:
    static void addLoglevel(LogLevel level, QString levelInStr, cComponent *comp, QActionGroup *logLevelActionGroup, QMenu *subMenu);

public:
    static void fillInspectorContextMenu(QMenu *menu, cObject *object, Inspector *insp);
    static QVector<InspectorType> supportedInspTypes(cObject *object);
    static Inspector *getContainingInspector(QWidget *widget);
    static QMenu *createInspectorContextMenu(QVector<cObject *> objects, Inspector *insp = nullptr);
    static QMenu *createInspectorContextMenu(cObject* object, Inspector *insp = nullptr);
    static void copyToClipboard(cObject *object, eCopy what);
    static void setClipboard(QString str);
    static void preferencesDialog(eTab defaultPage = TAB_NOTDEFINED);
    static QString getInspectMenuLabel(InspectorType type);

    // like QString::number, only better
    static QString formatDouble(double num);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_INSPECTORUTIL_H
