//=========================================================================
//  QUANTITYFORMATTER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2022 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "quantityformatter.h"

using namespace std;

namespace omnetpp {
namespace common {

QuantityFormatter::Output QuantityFormatter::formatQuantity(double value, const char *unit)
{
    const Input input(value, unit);
    checkInput(input);
    checkOptions();
    State state;
    determineSign(input, state);
    determineOutputUnit(input, state);
    computeValueInOutputUnit(input, state);
    if (std::isfinite(value) && value != 0) {
        do {
            clearDigitGroups(state);
            clearStreams(state);
            printAbsoluteValueInOutputUnit(state);
            computeScientificNotationExponent(state);
            determineNotation(state);
            applyScientificNotation(state);
            computeRoundedAbsoluteValueInOutputUnit(state);
            computeIntegralPart(state);
            computeFractionalPart(state);
            formatQuantity(input, state);
            fitIntoAvailableSpace(state);
        }
        while (retryFormat(state));
    }
    else {
        formatQuantity(input, state);
        fitIntoAvailableSpace(state);
    }
    return createOutput(state);
}

}  // namespace common
}  // namespace omnetpp

