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
    num_samples = 0L;
    sum_samples = sqrsum_samples = 0;
    min_samples = max_samples= 0;
}

std::string cStdDev::info() const
{
    std::stringstream out;
    out << "n=" << samples()
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
    buffer->pack(num_samples);
    buffer->pack(min_samples);
    buffer->pack(max_samples);
    buffer->pack(sum_samples);
    buffer->pack(sqrsum_samples);
#endif
}

void cStdDev::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cStatistic::netUnpack(buffer);
    buffer->unpack(num_samples);
    buffer->unpack(min_samples);
    buffer->unpack(max_samples);
    buffer->unpack(sum_samples);
    buffer->unpack(sqrsum_samples);
#endif
}

cStdDev& cStdDev::operator=(const cStdDev& res)
{
    if (this==&res) return *this;

    cStatistic::operator=(res);
    num_samples = res.num_samples;
    min_samples = res.min_samples;
    max_samples = res.max_samples;
    sum_samples = res.sum_samples;
    sqrsum_samples = res.sqrsum_samples;

    return *this;
}

void cStdDev::collect(double value)
{
    if (++num_samples <= 0)
    {
        // num_samples overflow: issue warning and stop collecting
        ev.printf("\a\nWARNING: (%s)%s: collect(): observation count overflow!\n\n",className(),fullPath().c_str());
        num_samples--;  // restore
        return;
    }

    sum_samples += value;
    sqrsum_samples += value*value;

    if (num_samples > 1)
    {
        if (value < min_samples)
            min_samples = value;
        else if (value > max_samples)
            max_samples = value;
    }
    else
    {
        min_samples = max_samples = value;
    }

    if (transientDetectionObject()) td->collect(value);  //NL
    if (accuracyDetectionObject()) ra->collect(value);   //NL
}

double cStdDev::variance() const
{
    if (num_samples<=1)
        return 0.0;
    else
    {
        // note: no check for division by zero, we prefer to return Inf or NaN
        double devsqr = (sqrsum_samples - sum_samples*sum_samples/num_samples)/(num_samples-1);
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
    os <<   "  Number of values = " << num_samples << "\n";
    if (num_samples==1)
        os << "  Value          = " << min_samples << "\n";
    else if (num_samples>0)
    {
        os << "  Mean value     = " << mean() << "\n";
        os << "  Standard dev.  = " << stddev() << "\n";
        os << "  Minimal value  = " << min_samples << "\n";
        os << "  Maximal value  = " << max_samples << "\n";
    }
    return os.str();
}

void cStdDev::clearResult()
{
    num_samples=0;
    sum_samples=sqrsum_samples=min_samples=max_samples=0;
}

double cStdDev::random() const
{
    switch (num_samples)
    {
        case 0:  return 0.0;
        case 1:  return min_samples;
        default: return normal(mean(), stddev(), genk);
    }
}

void cStdDev::saveToFile(FILE *f) const
{
    fprintf(f,"\n#\n# (%s) %s\n#\n", className(), fullPath().c_str());
    fprintf(f,"%ld\t #= num_samples\n",num_samples);
    fprintf(f,"%g %g\t #= min, max\n", min_samples, max_samples);
    fprintf(f,"%g\t #= sum\n", sum_samples);
    fprintf(f,"%g\t #= square sum\n", sqrsum_samples );
}

void cStdDev::loadFromFile(FILE *f)
{
    freadvarsf(f,"");  freadvarsf(f,""); freadvarsf(f,""); freadvarsf(f,"");
    freadvarsf(f,"%ld\t #= num_samples",&num_samples);
    freadvarsf(f,"%g %g\t #= min, max", &min_samples, &max_samples);
    freadvarsf(f,"%g\t #= sum", &sum_samples);
    freadvarsf(f,"%g\t #= square sum", &sqrsum_samples);
}

//----

std::string cWeightedStdDev::info() const
{
    std::stringstream out;
    out << "n=" << samples()
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
    //FIXME add the rest!!!!
#endif
}

void cWeightedStdDev::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this,eNOPARSIM);
#else
    cStdDev::netUnpack(buffer);
    buffer->unpack(sum_weights);
    //FIXME add the rest!!!
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
        if (++num_samples <= 0)
        {
            // num_samples overflow: issue warning and stop collecting
            ev.printf("\a\nWARNING: (%s)%s: collect2(): observation count overflow!\n\n",className(),fullPath().c_str());
            num_samples--;  // restore
            return;
        }

        sum_samples += value;
        sqrsum_samples += value*value;

        if (num_samples > 1)
        {
            if (value < min_samples)
                min_samples = value;
            else if (value > max_samples)
                max_samples = value;
        }
        else
        {
            min_samples = max_samples = value;
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
    //FIXME add the rest!
}

void cWeightedStdDev::loadFromFile(FILE *f)
{
    cStdDev::loadFromFile(f);
    freadvarsf(f,"%g\t #= sum_weights", &sum_weights);
    //FIXME add the rest!
}


