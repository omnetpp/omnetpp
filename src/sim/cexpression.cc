//==========================================================================
//   CEXPRESSION.CC  - part of
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

#include "omnetpp/cexpression.h"

using namespace std;

namespace omnetpp {

cValue cExpression::evaluate(cComponent *contextComponent) const
{
    Context context(contextComponent, nullptr);
    return evaluate(&context);
}

bool cExpression::boolValue(cComponent *contextComponent) const
{
    Context context(contextComponent, nullptr);
    return boolValue(&context);
}

intval_t cExpression::intValue(cComponent *contextComponent, const char *expectedUnit) const
{
    Context context(contextComponent, nullptr);
    return intValue(&context, expectedUnit);
}

double cExpression::doubleValue(cComponent *contextComponent, const char *expectedUnit) const
{
    Context context(contextComponent, nullptr);
    return doubleValue(&context, expectedUnit);
}

std::string cExpression::stringValue(cComponent *contextComponent) const
{
    Context context(contextComponent, nullptr);
    return stringValue(&context);
}

cXMLElement *cExpression::xmlValue(cComponent *contextComponent) const
{
    Context context(contextComponent, nullptr);
    return xmlValue(&context);
}

}  // namespace omnetpp

