//=========================================================================
//  STATISTICS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cfloat>
#include "statistics.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {

Statistics Statistics::makeUnweighted(int64_t count, double minValue, double maxValue, double sumValues, double sumSquaredValues)
{
    Statistics tmp;
    tmp.weighted = false;
    tmp.count = count;
    tmp.sumWeights = count;
    tmp.minValue = minValue;
    tmp.maxValue = maxValue;
    tmp.sumWeightedValues = sumValues;
    tmp.sumSquaredWeights = count;
    tmp.sumWeightedSquaredValues = sumSquaredValues;
    return tmp;
}

Statistics Statistics::makeWeighted(int64_t count, double minValue, double maxValue, double sumWeights, double sumWeightedValues, double sumSquaredWeights, double sumWeightedSquaredValues)
{
    Statistics tmp;
    tmp.weighted = true;
    tmp.count = count;
    tmp.sumWeights = sumWeights;
    tmp.minValue = minValue;
    tmp.maxValue = maxValue;
    tmp.sumWeightedValues = sumWeightedValues;
    tmp.sumSquaredWeights = sumSquaredWeights;
    tmp.sumWeightedSquaredValues = sumWeightedSquaredValues;
    return tmp;
}

Statistics Statistics::makeInvalid(bool weighted)
{
    Statistics tmp;
    tmp.weighted = weighted;
    tmp.count = -1;
    tmp.sumWeights = tmp.minValue = tmp.maxValue = tmp.sumWeightedValues = tmp.sumSquaredWeights = tmp.sumWeightedSquaredValues = NaN;
    return tmp;
}

void Statistics::assertUnweighted() const
{
    if (weighted)
        throw opp_runtime_error("Statistics: sum and sqrSum are not available for weighted statistics");
}

void Statistics::clear()
{
    count = 0;
    sumWeights = 0.0;
    minValue = POSITIVE_INFINITY;
    maxValue = NEGATIVE_INFINITY;
    sumWeightedValues = 0.0;
    sumSquaredWeights = 0.0;
    sumWeightedSquaredValues = 0.0;
}

void Statistics::collect(double value)
{
    Assert(!weighted);
    count++;
    sumWeights += 1;
    minValue = (minValue < value ? minValue : value);
    maxValue = (maxValue > value ? maxValue : value);
    sumWeightedValues += value;
    sumSquaredWeights += 1;
    sumWeightedSquaredValues += value * value;
}

void Statistics::collect(double value, double weight)
{
    Assert(weighted);
    count++;
    sumWeights += weight;
    minValue = (minValue < value ? minValue : value);
    maxValue = (maxValue > value ? maxValue : value);
    sumWeightedValues += weight * value;
    sumSquaredWeights += weight * weight;
    sumWeightedSquaredValues += weight * value * value;
}

void Statistics::adjoin(const Statistics& other)
{
    Assert(weighted == other.weighted);
    count += other.count;
    sumWeights += other.sumWeights;
    minValue = (minValue < other.minValue ? minValue : other.minValue);
    maxValue = (maxValue > other.maxValue ? maxValue : other.maxValue);
    sumWeightedValues += other.sumWeightedValues;
    sumSquaredWeights += other.sumSquaredWeights;
    sumWeightedSquaredValues += other.sumWeightedSquaredValues;
}

double Statistics::getStddev() const
{
    return std::sqrt(getVariance());
}

double Statistics::getVariance() const
{
    // note: no checks for division by zero, we prefer to return Inf or NaN
    if (count <= 1)
        return NaN;
    else if (minValue == maxValue)
        return 0;
    else if (!isWeighted()) {
        double var = (sumWeightedSquaredValues - sumWeightedValues*sumWeightedValues/sumWeights) / (sumWeights-1);
        return var < 0 ? 0 : var;
    }
    else {
        double var = (sumWeights * sumWeightedSquaredValues - sumWeightedValues * sumWeightedValues) / (sumWeights * sumWeights - sumSquaredWeights);
        return var < 0 ? 0 : var;
    }
}

}  // namespace common
}  // namespace omnetpp
