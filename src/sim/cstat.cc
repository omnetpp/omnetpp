//=========================================================================
//  CSTAT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//        Based on the MISS simulator's result collection
//
//   Member functions of
//     cStatistic: base class of different statistic collecting classes
//     cStdDev:    mean, std deviation, min_samples, max_samples
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
#include <assert.h>
#include <string>

#include "cstat.h"
#include "random.h"
#include "distrib.h"
#include "macros.h"
#include "cdetect.h"  //NL
#include "csimplemodule.h"
#include "cexception.h"
#include "cenvir.h"
#include "errmsg.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;


//=========================================================================
//=== Registration
Register_Class(cStdDev);
Register_Class(cWeightedStdDev);

//=========================================================================
// cStatistic - almost all functions are inline

cStatistic::cStatistic(const cStatistic& r) : cObject()
{
    setName(r.name());
    td=NULL;
    ra=NULL;
    operator=(r);
}

cStatistic::cStatistic(const char *name) : cObject(name)
{
    td=NULL;
    ra=NULL;
    genk=0;
}

cStatistic::~cStatistic()
{
    dropAndDelete(td);
    dropAndDelete(ra);
}

void cStatistic::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cObject::netPack(buffer);

    buffer->pack(genk);

    if (buffer->packFlag(td!=NULL))
        buffer->packObject(td);
    if (buffer->packFlag(ra!=NULL))
        buffer->packObject(ra);
#endif
}

void cStatistic::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cObject::netUnpack(buffer);

    buffer->unpack(genk);

    if (buffer->checkFlag())
        take(td = (cTransientDetection *) buffer->unpackObject());
    if (buffer->checkFlag())
        take(ra = (cAccuracyDetection *) buffer->unpackObject());
#endif
}

cStatistic& cStatistic::operator=(const cStatistic& res)   //--VA
{
    if (this==&res) return *this;

    cObject::operator=( res );
    genk = res.genk;
    dropAndDelete(td);
    dropAndDelete(ra);
    td = res.td;
    if (td)
        take( td = (cTransientDetection *)td->dup() );
    ra = res.ra;
    if (ra)
        take( ra = (cAccuracyDetection *)ra->dup() );
    return *this;
}

void cStatistic::addTransientDetection(cTransientDetection *obj)  //NL
{
    if (td)
        throw new cRuntimeError(this,"addTransientDetection(): object already has a transient detection algorithm");
    td = obj;                       // create pointer to td object
    td->setHostObject( this );      // and create one back
    take(td);
}

void cStatistic::addAccuracyDetection(cAccuracyDetection *obj)  //NL
{
    if (ra)
        throw new cRuntimeError(this,"addAccuracyDetection(): object already has an accuracy detection algorithm");
    ra = obj;                       // create pointer to ra object
    ra->setHostObject( this );      // and create one back
    take(ra);
}

void cStatistic::collect2(double, double)
{
    throw new cRuntimeError(this, "collect2() not implemented");
}

void cStatistic::recordScalar(const char *scalarname)
{
    cSimpleModule *mod = dynamic_cast<cSimpleModule *>(simulation.contextModule());
    if (!mod)
        throw new cRuntimeError(this,"recordScalar() may only be invoked from within a simple module");
    std::string n = scalarname ? scalarname : fullName();
    mod->recordScalar((n+".samples").c_str(), samples());
    mod->recordScalar((n+".mean").c_str(), mean());
    mod->recordScalar((n+".stddev").c_str(), stddev());
    mod->recordScalar((n+".min").c_str(), min());
    mod->recordScalar((n+".max").c_str(), max());
}

void cStatistic::freadvarsf (FILE *f,  const char *fmt, ...)
{
    char line[101];

    // read line and chop CR/LF
    fgets(line,101,f);
    char *end = line+strlen(line)-1;
    while (end>=line && (*end=='\n' || *end=='\r')) *end-- = '\0';

    // match '#=' tags
    const char *fmt_comment = strstr(fmt,"#=");
    const char *line_comment = strstr(line,"#=");
    if (fmt_comment && line_comment && strcmp(fmt_comment,line_comment)!=0)
        throw new cRuntimeError(this, "bad file format in loadFromFile(): expected `%s' and got `%s'",fmt,line);

    // actual read
    va_list args;
    va_start(args,fmt);
    opp_vsscanf(line,fmt,args);
}

//==========================================================================
// cStdDev - member functions

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
    throw new cRuntimeError(this,eNOPARSIM);
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
    throw new cRuntimeError(this,eNOPARSIM);
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

// collect one value
void cStdDev::collect(double val)
{
    if (++num_samples <= 0)
    {
        // FIXME: num_samples overflow: issue warning and must stop collecting!
        ev.printf("\a\nWARNING: (%s)%s: observation count overflow!\n\n",className(),fullPath().c_str());
    }
    sum_samples+=val;
    sqrsum_samples+=val*val;

    if (num_samples>1)
    {
        if (val<min_samples)
            min_samples=val;
        else if (val>max_samples)
            max_samples=val;
    }
    else
    {
        min_samples=max_samples=val;
    }

    if (transientDetectionObject()) td->collect(val);  //NL
    if (accuracyDetectionObject()) ra->collect(val);   //NL
}

double cStdDev::variance() const
{
    if (num_samples<=1)
        return 0.0;
    else
    {
        double devsqr = (sqrsum_samples - sum_samples*sum_samples/num_samples)/(num_samples-1);
        if (devsqr<=0)
            return 0.0;
        else
            return devsqr;
    }
}

double cStdDev::stddev() const
{
    return sqrt( variance() );
}

void cStdDev::writeContents(ostream& os)
{
    os <<   "  Number of values = " << num_samples << "\n";
    if (num_samples==1)
        os << "  Value          = " << min_samples << "\n";
    else  if (num_samples>0)
    {
        os << "  Mean value     = " << mean() << "\n";
        os << "  Standard dev.  = " << stddev() << "\n";
        os << "  Minimal value  = " << min_samples << "\n";
        os << "  Maximal value  = " << max_samples << "\n";
    }
}

void cStdDev::clearResult()
{
    num_samples=0;
    sum_samples=sqrsum_samples=min_samples=max_samples=0;
}

double cStdDev::random() const
{
    switch(num_samples)
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
    fprintf(f,"%g\t #= square sum\n", sqrsum_samples);
}

void cStdDev::loadFromFile(FILE *f)
{
    freadvarsf(f,"");  freadvarsf(f,""); freadvarsf(f,""); freadvarsf(f,"");
    freadvarsf(f,"%ld\t #= num_samples",&num_samples);
    freadvarsf(f,"%g %g\t #= min, max", &min_samples, &max_samples);
    freadvarsf(f,"%g\t #= sum", &sum_samples);
    freadvarsf(f,"%g\t #= square sum", &sqrsum_samples);
}

//==========================================================================
// cWeightedStdDev - member functions

void cWeightedStdDev::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cStdDev::netPack(buffer);
    buffer->pack(sum_weights);
#endif
}

void cWeightedStdDev::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cStdDev::netUnpack(buffer);
    buffer->unpack(sum_weights);
#endif
}

cWeightedStdDev& cWeightedStdDev::operator=(const cWeightedStdDev& res)
{
    if (this==&res) return *this;

    cStdDev::operator=(res);
    sum_weights = res.sum_weights;
    return *this;
}

void cWeightedStdDev::collect2(double val, double weight)
{
    cStdDev::collect(weight*val);
    sum_weights += weight;
}

void cWeightedStdDev::clearResult()
{
    cStdDev::clearResult();
    sum_weights=0.0;
}

double cWeightedStdDev::variance() const
{
    throw new cRuntimeError(this, "variance()/stddev() not implemented");

    // if (sum_weights==0)
    //   return 0.0;
    // else
    // {
    //   double devsqr = (sqrsum_samples - sum_samples*sum_samples/sum_weights)/(sum_weights-1);
    //   if (devsqr<=0)
    //       return 0.0;
    //   else
    //       return devsqr;
    //}
}

void cWeightedStdDev::saveToFile(FILE *f) const
{
    cStdDev::saveToFile(f);
    fprintf(f,"%g\t #= sum_weights\n",sum_weights);
}

void cWeightedStdDev::loadFromFile(FILE *f)
{
    cStdDev::loadFromFile(f);
    freadvarsf(f,"%g\t #= sum_weights",&sum_weights);
}
