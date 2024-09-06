//==========================================================================
//  exponentialspinbox.cc - part of
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

#include "exponentialspinbox.h"

#include <cmath>
#include <QtCore/QDebug>
#include "common/stringutil.h"

namespace omnetpp {
using namespace common;
namespace qtenv {

ExponentialSpinBox::ExponentialSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
{
    // well, this is disgusting.
    connect(this, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            [this](double val) { adjust(val); });

    setDecimals(15);
}

void ExponentialSpinBox::adjust(double val, bool notify)
{
    // don't want to influence the speed here programatically through the valueChanged signal
    bool blocked = blockSignals(!notify);

    if (std::isnan(val)) {
        setSingleStep(minimum());
        setValue(minimum());
    }
    else {
        setSingleStep(val / 10);
        setValue(val);
    }

    blockSignals(blocked);
}

void ExponentialSpinBox::stepBy(int steps)
{
    QDoubleSpinBox::stepBy(steps);
    if ((steps > 0) && value() == minimum()) {
        setValue(2 * minimum());
        if (minimum() == 0) {
            setValue(singleStep());
            if (singleStep() == 0)
                adjust(1e-9);
        }
    }
}

QString ExponentialSpinBox::textFromValue(double val) const
{
    return opp_stringf("%.4g", val).c_str();
}

double ExponentialSpinBox::valueFromText(const QString &text) const
{
    // the special value, "none"
    if (text.isEmpty())
        return specialValueText().isEmpty() ? value() : minimum();

    return QLocale("en_US").toDouble(text);
}

void ExponentialSpinBox::fixup(QString &str) const
{
    // the user might just want to erase the special value one character at a time
    if (specialValueText().contains(str))
        str = specialValueText();

    // they might be in the middle of entering a number in scientific notation
    if (str.endsWith("e"))
        str = str.left(str.length() - 1);
    if (str.endsWith("e-") || str.endsWith("e+"))
        str = str.left(str.length() - 2);
}

QValidator::State ExponentialSpinBox::validate(QString &input, int &pos) const
{
    if (input.contains(','))
        return QValidator::Invalid;

    // the user might just want to erase the special value one character at a time
    if (specialValueText().contains(input))
        return QValidator::Intermediate;

    // they might be in the middle of entering a number in scientific notation
    if (input.endsWith("e") || input.endsWith("e-") || input.endsWith("e+"))
        return QValidator::Intermediate;

    bool ok;
    QLocale("en_US").toDouble(input, &ok);
    return ok ? QValidator::Acceptable : QValidator::Invalid;
}

}  // namespace qtenv
}  // namespace omnetpp
