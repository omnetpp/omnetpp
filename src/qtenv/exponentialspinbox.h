//==========================================================================
//  exponentialspinbox.h - part of
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

#ifndef __OMNETPP_QTENV_EXPONENTIALSPINBOX_H
#define __OMNETPP_QTENV_EXPONENTIALSPINBOX_H

#include <QtWidgets/QDoubleSpinBox>

#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

class QTENV_API ExponentialSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

    QString textFromValue(double val) const override;
    double valueFromText(const QString &text) const override;

    void fixup(QString &str) const override;
    QValidator::State validate(QString &input, int &pos) const override;

public Q_SLOTS:
    void adjust(double val, bool notify = false);

    void stepBy(int steps) override;

public:
    explicit ExponentialSpinBox(QWidget *parent);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_EXPONENTIALSPINBOX_H
