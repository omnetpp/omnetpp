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

#ifndef __OMNETPP_QTENV_INSPECTORUTIL_H
#define __OMNETPP_QTENV_INSPECTORUTIL_H

#include <QVector>
#include "qtenvdefs.h"

class QMenu;
class QActionGroup;

namespace omnetpp {

class cObject;

namespace qtenv {

class Inspector;

class InspectorUtil
{
private:
    static void addLoglevel(LogLevel level, QString levelInStr, cComponent *comp, QActionGroup *logLevelActionGroup, QMenu *subMenu);

public:
    enum eCopy { COPY_PTR, COPY_PTRWITHCAST, COPY_FULLPATH, COPY_FULLNAME, COPY_CLASSNAME };
    enum eTab { TAB_GENERAL, TAB_LOGS, TAB_LAYOUTING, TAB_ANIMATION, TAB_FILTERING, TAB_FONTS, TAB_NOTDEFINED };

    static void fillInspectorContextMenu(QMenu *menu, cObject *object, Inspector *insp);
    static QVector<int> supportedInspTypes(cObject *object);
    static Inspector *getContainingInspector(QWidget *widget);
    static QMenu *createInspectorContextMenu(QVector<cObject *> objects, Inspector *insp = nullptr);
    static QMenu *createInspectorContextMenu(cObject* object, Inspector *insp = nullptr);
    static void copyToClipboard(cObject *object, int what);
    static void setClipboard(QString str);
    static void preferencesDialog(eTab defaultPage = TAB_NOTDEFINED);
    static QString getInspectMenuLabel(int typeCode);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_INSPECTORUTIL_H
