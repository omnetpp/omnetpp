//==========================================================================
//  TIMELINEINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_TIMELINEINSPECTOR_H
#define __OMNETPP_QTENV_TIMELINEINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace qtenv {

class TimeLineGraphicsView;

class QTENV_API TimeLineInspector : public Inspector
{
    Q_OBJECT
private:
    TimeLineGraphicsView *timeLine;

private Q_SLOTS:
    void runPreferencesDialog();
    void onFontChanged();

public Q_SLOTS:
    void createContextMenu(QVector<cObject*> objects, QPoint globalPos);
    void setObjectToObjectInspector(cObject* object);
    void openInspector(cObject *object);

public:
    TimeLineInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

    virtual void refresh();
    double getInitHeight();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_TIMELINEINSPECTOR_H
