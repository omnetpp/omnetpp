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
#include <QDebug>
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
}

void ExponentialSpinBox::adjust(double val, bool notify)
{
    // don't want to influence the speed here programatically through the valueChanged signal
    bool blocked = blockSignals(!notify);

    if (std::isnan(val)) {
        // making it kindof exponential
        int magnitude = std::ceil(std::log10(minimum()*1.0001));
        setSingleStep(minimum());
        // the ifs are here to keep it spinning (except when magnitude changes...)
        if (decimals() != (4 -magnitude))
            setDecimals(4 - magnitude);

        setValue(minimum());
    }
    else {
        // making it kindof exponential
        int magnitude = std::ceil(std::log10(val*1.0001));
        setSingleStep(val / 10);
        // the ifs are here to keep it spinning (except when magnitude changes...)
        if (decimals() != (4 -magnitude))
            setDecimals(4 - magnitude);

        setValue(val);
    }

    blockSignals(blocked);
}

void ExponentialSpinBox::stepBy(int steps)
{
    QDoubleSpinBox::stepBy(steps);
    if ((steps > 0) && value() == minimum()) {
        setValue(2*minimum());
        if (minimum() == 0) {
            setValue(singleStep());
            if (singleStep() == 0) {
                adjust(1e-15);
            }
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

    return QDoubleSpinBox::valueFromText(text);
}

QValidator::State ExponentialSpinBox::validate(QString &input, int &pos) const
{
    auto res = QDoubleSpinBox::validate(input, pos);

    // if the inherited validator did not accept, let's try without a trailing 'e'
    if (res == QValidator::Invalid && input.endsWith('e')) {
        // so strip it from the end
        QString stripped = input.left(input.length()-1);
        // and if there is no more of it, let's check again
        if (!stripped.contains('e'))
            res = QDoubleSpinBox::validate(stripped, pos);
    }

    // if still no bueno, the user might just want to erase the special value
    if (res == QValidator::Invalid && specialValueText().contains(input))
        res = QValidator::Intermediate;

    return res;
}

} // namespace qtenv
} // namespace omnetpp
