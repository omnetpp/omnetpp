//=========================================================================
//  CSTDDEV.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//        Based on the MISS simulator's result collection
//
//   Member functions of
//    cStdDev: basic statistics (mean, stddev, min, max, etc)
//    cWeightedStdDev: weighted version
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string>

#include "cstddev.h"
#include "random.h"
#include "distrib.h"
#include "globals.h"
#include "cdetect.h"  //NL
#include "csimplemodule.h"
#include "cexception.h"
#include "cenvir.h"
#include "stringutil.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;

Register_Class(cStdDev);
Register_Class(cWeightedStdDev);


cStdDev::cStdDev(const char *s) : cStatistic(s)
{
    num_vals = 0L;
    sum_vals = sqrsum_vals = 0;
    min_vals = max_vals= 0;
}

std::string cStdDev::info() const
{
    std::stringstream out;
    out << "n=" << count()
        << " mean=" << mean()
        << " stddev=" << stddev()
        << " min=" << min()
        << " max=" << max();
    return out.str();
}

void cStdDev::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cStatistic::netPack(buffer);
    buffer->pack(num_vals);
    buffer->pack(min_vals);
    buffer->pack(max_vals);
    buffer->pack(sum_vals);
    buffer->pack(sqrsum_vals);
#endif
}

void cStdDev::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cStatistic::netUnpack(buffer);
    buffer->unpack(num_vals);
    buffer->unpack(min_vals);
    buffer->unpack(max_vals);
    buffer->unpack(sum_vals);
    buffer->unpack(sqrsum_vals);
#endif
}

cStdDev& cStdDev::operator=(const cStdDev& res)
{
    if (this==&res) return *this;

    cStatistic::operator=(res);
    num_vals = res.num_vals;
    min_vals = res.min_vals;
    max_vals = res.max_vals;
    sum_vals = res.sum_vals;
    sqrsum_vals = res.sqrsum_vals;

    return *this;
}

void cStdDev::collect(double value)
{
    if (++num_vals <= 0)
    {
        // num_vals overflow: issue warning and stop collecting
        ev.printf("\a\nWARNING: (%s)%s: collect(): observation count overflow!\n\n",className(),fullPath().c_str());
        num_vals--;  // restore
        return;
    }

    sum_vals += value;
    sqrsum_vals += value*value;

    if (num_vals > 1)
    {
        if (value < min_vals)
            min_vals = value;
        else if (value > max_vals)
            max_vals = value;
    }
    else
    {
        min_vals = max_vals = value;
    }

    if (transientDetectionObject()) td->collect(value);  //NL
    if (accuracyDetectionObject()) ra->collect(value);   //NL
}

void cStdDev::doMerge(const cStatistic *other)
{
    long orig_num_vals = num_vals;
    num_vals += other->count();
    if (num_vals < 0)
        throw cRuntimeError(this, "merge(): observation count overflow");

    if (other->count()>0 && (orig_num_vals==0 || min_vals>other->min()))
        min_vals = other->min();
    if (other->count()>0 && (orig_num_vals==0 || max_vals<other->max()))
        max_vals = other->max();

    sum_vals += other->sum();
    sqrsum_vals += other->sqrSum();
}

void cStdDev::merge(const cStatistic *other)
{
    if (other->isWeighted())
        throw cRuntimeError(this, "Cannot merge weighted statistics (%s)%s into unweighted statistics",
                                  other->className(), other->fullPath().c_str());
    doMerge(other);
}

double cStdDev::variance() const
{
    if (num_vals<=1)
        return 0.0;
    else
    {
        // note: no check for division by zero, we prefer to return Inf or NaN
        double devsqr = (sqrsum_vals - sum_vals*sum_vals/num_vals)/(num_vals-1);
        if (devsqr<=0)
            return 0.0;
        else
            return devsqr;
    }
}

double cStdDev::stddev() const
{
    return sqrt(variance());
}

std::string cStdDev::detailedInfo() const
{
    std::stringstream os;
    os <<   "  Number of values = " << num_vals << "\n";
    if (num_vals==1)
        os << "  Value          = " << min_vals << "\n";
    else if (num_vals>0)
    {
        os << "  Mean value     = " << mean() << "\n";
        os << "  Standard dev.  = " << stddev() << "\n";
        os << "  Minimal value  = " << min_vals << "\n";
        os << "  Maximal value  = " << max_vals << "\n";
    }
    return os.str();
}

void cStdDev::clearResult()
{
    num_vals=0;
    sum_vals=sqrsum_vals=min_vals=max_vals=0;
}

double cStdDev::random() const
{
    switch (num_vals)
    {
        case 0:  return 0.0;
        case 1:  return min_vals;
        default: return normal(mean(), stddev(), genk);
    }
}

void cStdDev::saveToFile(FILE *f) const
{
    fprintf(f,"\n#\n# (%s) %s\n#\n", className(), fullPath().c_str());
    fprintf(f,"%ld\t #= num_vals\n",num_vals);
    fprintf(f,"%g %g\t #= min, max\n", min_vals, max_vals);
    fprintf(f,"%g\t #= sum\n", sum_vals);
    fprintf(f,"%g\t #= square sum\n", sqrsum_vals );
}

void cStdDev::loadFromFile(FILE *f)
{
    freadvarsf(f,"");  freadvarsf(f,""); freadvarsf(f,""); freadvarsf(f,"");
    freadvarsf(f,"%ld\t #= num_vals",&num_vals);
    freadvarsf(f,"%g %g\t #= min, max", &min_vals, &max_vals);
    freadvarsf(f,"%g\t #= sum", &sum_vals);
    freadvarsf(f,"%g\t #= square sum", &sqrsum_vals);
}

//----

std::string cWeightedStdDev::info() const
{
    std::stringstream out;
    out << "n=" << count()
        << " mean=" << mean()
        << " stddev=" << stddev()
        << " min=" << min()
        << " max=" << max();
    return out.str();
}

void cWeightedStdDev::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cStdDev::netPack(buffer);
    buffer->pack(sum_weights);
    buffer->pack(sum_weighted_vals);
    buffer->pack(sum_squared_weights);
    buffer->pack(sum_weights_squared_vals);
#endif
}

void cWeightedStdDev::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cStdDev::netUnpack(buffer);
    buffer->unpack(sum_weights);
    buffer->unpack(sum_weighted_vals);
    buffer->unpack(sum_squared_weights);
    buffer->unpack(sum_weights_squared_vals);
#endif
}

cWeightedStdDev& cWeightedStdDev::operator=(const cWeightedStdDev& res)
{
    if (this==&res) return *this;

    cStdDev::operator=(res);
    sum_weights = res.sum_weights;
    sum_weighted_vals = res.sum_weighted_vals;
    sum_squared_weights = res.sum_squared_weights;
    sum_weights_squared_vals = res.sum_weights_squared_vals;
    return *this;
}

void cWeightedStdDev::collect2(double value, double weight)
{
    if (weight > 0)
    {
        if (++num_vals <= 0)
        {
            // num_vals overflow: issue warning and stop collecting
            ev.printf("\a\nWARNING: (%s)%s: collect2(): observation count overflow!\n\n",className(),fullPath().c_str());
            num_vals--;  // restore
            return;
        }

        sum_vals += value;
        sqrsum_vals += value*value;

        if (num_vals > 1)
        {
            if (value < min_vals)
                min_vals = value;
            else if (value > max_vals)
                max_vals = value;
        }
        else
        {
            min_vals = max_vals = value;
        }

        sum_weights += weight;
        sum_weighted_vals += weight * value;
        sum_squared_weights += weight * weight;
        sum_weights_squared_vals += weight * value * value;

        if (transientDetectionObject()) td->collect(value);
        if (accuracyDetectionObject()) ra->collect(value);
    }
    else if (weight < 0)
    {
        throw new cRuntimeError(this, "collect2(): negative weight %g", weight);
    }
}

void cWeightedStdDev::merge(const cStatistic *other)
{
    cStdDev::doMerge(other);
    sum_weights += other->weights();
    sum_weighted_vals += other->weightedSum();
    sum_squared_weights += other->sqrSumWeights();
    sum_weights_squared_vals += other->weightedSqrSum();
}

void cWeightedStdDev::clearResult()
{
    cStdDev::clearResult();
    sum_weights = 0.0;
    sum_weighted_vals = 0.0;
    sum_squared_weights = 0.0;
    sum_weights_squared_vals = 0.0;
}

double cWeightedStdDev::variance() const
{
    // note: no check for division by zero, we prefer to return Inf or NaN
    double denominator = sum_weights * sum_weights - sum_squared_weights;
    return (sum_weights * sum_weights_squared_vals - sum_weighted_vals * sum_weighted_vals) / denominator;
}

void cWeightedStdDev::saveToFile(FILE *f) const
{
    cStdDev::saveToFile(f);
    fprintf(f,"%g\t #= sum_weights\n", sum_weights);
    fprintf(f,"%g\t #= sum_weighted_vals\n", sum_weighted_vals);
    fprintf(f,"%g\t #= sum_squared_weights\n", sum_squared_weights);
    fprintf(f,"%g\t #= sum_weights_squared_vals\n", sum_weights_squared_vals);
}

void cWeightedStdDev::loadFromFile(FILE *f)
{
    cStdDev::loadFromFile(f);
    freadvarsf(f,"%g\t #= sum_weights", &sum_weights);
    freadvarsf(f,"%g\t #= sum_weighted_vals", &sum_weighted_vals);
    freadvarsf(f,"%g\t #= sum_squared_weights", &sum_squared_weights);
    freadvarsf(f,"%g\t #= sum_weights_squared_vals", &sum_weights_squared_vals);
}


