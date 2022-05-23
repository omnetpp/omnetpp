//==========================================================================
//  WATCHINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_WATCHINSPECTOR_H
#define __OMNETPP_QTENV_WATCHINSPECTOR_H

#include "inspector.h"

#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

namespace omnetpp {
namespace qtenv {

class QTENV_API WatchInspector: public Inspector
{
    Q_OBJECT

protected:
    QLabel *label;
    QLineEdit *editor;

protected Q_SLOTS:
    virtual void commit();

public:
    WatchInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

    virtual void refresh() override;
};

}  // namespace qtenv qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_WATCHINSPECTOR_H



