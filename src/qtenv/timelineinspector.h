//==========================================================================
//  TIMELINEINSPECTOR.H - part of
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

#ifndef TIMELINEINSPECTOR_H
#define TIMELINEINSPECTOR_H

#include "inspector.h"

namespace omnetpp {
namespace qtenv {

class TimeLineGraphicsView;

class TimeLineInspector : public Inspector
{
    Q_OBJECT
private:

    TimeLineGraphicsView *timeLine;

private slots:
    void runPreferencesDialog();

public slots:
    void createContextMenu(QVector<cObject*> objects, QPoint globalPos);
    void setObjectToObjectInspector(cObject* object);
    void openInspector(cObject *object);

public:
    TimeLineInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

    virtual void refresh();
};

} // namespace qtenv
} // namespace omnetpp

#endif // TIMELINEINSPECTOR_H
