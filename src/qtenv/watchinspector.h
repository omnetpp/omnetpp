//==========================================================================
//  WATCHINSPECTOR.H - part of
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

#ifndef __OMNETPP_QTENV_WATCHINSPECTOR_H
#define __OMNETPP_QTENV_WATCHINSPECTOR_H

#include "inspector.h"

#include <QLabel>
#include <QLineEdit>

namespace omnetpp {
namespace qtenv {

class QTENV_API WatchInspector: public Inspector
{
    Q_OBJECT

protected:
    QLabel *label;
    QLineEdit *editor;

public:
    WatchInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f);

public slots:
    virtual void refresh() override;
    virtual void commit() override;
};

} // namespace qtenv qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_WATCHINSPECTOR_H



