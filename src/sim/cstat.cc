//=========================================================================
//
//  CSTAT.CC - part of
//                          OMNeT++
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
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <fstream.h>
#include <iomanip.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "cstat.h"
#include "macros.h"
#include "cdetect.h"  //NL

//=========================================================================
//=== Registration
Register_Class( cStdDev )
Register_Class( cWeightedStdDev )

//=========================================================================
// cStatistic - almost all functions are inline

cStatistic& cStatistic::operator=(cStatistic& res)   //--VA
{
    cObject::operator=( res );
    genk = res.genk;
    if (td && td->owner()==this)  free( td );
    if (ra && ra->owner()==this)  free( ra );
    td = res.td; if (td && td->owner()==&res)  take( td = (cTransientDetection *)td->dup() );
    ra = res.ra; if (ra && ra->owner()==&res)  take( ra = (cAccuracyDetection *)ra->dup() );
    return *this;
}

void cStatistic::addTransientDetection(cTransientDetection *obj)  //NL
{
    td = obj;                       // create pointer to td object
    td->setHostObject( this );      // and create one back
    if (takeOwnership()) take(td);  //--VA
}

void cStatistic::addAccuracyDetection(cAccuracyDetection *obj)  //NL
{
    ra = obj;                       // create pointer to ra object
    ra->setHostObject( this );      // and create one back
    if (takeOwnership()) take(ra);  //--VA
}

void cStatistic::collect2(double, double)
{
    opp_error("(%s)%s: collect2() not implemented in %s",
                       isA(), fullName(),isA());
}

void cStatistic::freadvarsf (FILE *f,  char *fmt, ...)
{
    char line[101];

    // read line and chop CR/LF
    fgets(line,101,f);
    char *end = line+strlen(line)-1;
    while (end>=line && (*end=='\n' || *end=='\r')) *end-- = '\0';

    // match '#=' tags
    char *fmt_comment = strstr(fmt,"#=");
    char *line_comment = strstr(line,"#=");
    if (fmt_comment && line_comment && strcmp(fmt_comment,line_comment)!=0)
    {
        opp_error("(%s)%s: bad file format in loadFromFile(): "
                         "expected `%s' and got `%s'",
                         isA(), fullName(),fmt,line);
        return;
    }

    // actual read
    va_list args;
    va_start(args,fmt);
    opp_vsscanf(line,fmt,args);
}

//==========================================================================
// cStdDev - member functions

void cStdDev::info(char *buf)
{
    cStatistic::info( buf );
    sprintf( buf+strlen(buf), " (n=%ld)", num_samples);
}

cStdDev& cStdDev::operator=(cStdDev& res)
{
    cStatistic::operator=(res);
    num_samples = res.num_samples; min_samples = res.min_samples; max_samples = res.max_samples;
    sum = res.sum; sqrsum = res.sqrsum;
    return *this;
}

// collect one value
void cStdDev::collect(double val)
{
    num_samples++;  sum+=val;  sqrsum+=val*val;

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

double cStdDev::variance()
{
    if (num_samples<=1)
        return 0.0;
    else
    {
        double devsqr = (sqrsum - sum*sum/num_samples)/(num_samples-1);
        if (devsqr<=0)
            return 0.0;
        else
            return devsqr;
    }
}

double cStdDev::stddev()
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
    sum=sqrsum=min_samples=max_samples=0;
}

double cStdDev::random()
{
    switch( num_samples )
    {
        case 0:  return 0.0;
        case 1:  return min_samples;
        default: return genk_normal(genk, mean(), stddev());
    }
}

void cStdDev::saveToFile(FILE *f)
{
    fprintf(f,"\n#\n# (%s) %s\n#\n", isA(), fullPath());
    fprintf(f,"%ld\t #= num_samples\n",num_samples);
    fprintf(f,"%lg %lg\t #= min, max\n", min_samples, max_samples);
    fprintf(f,"%lg\t #= sum\n", sum);
    fprintf(f,"%lg\t #= square sum\n", sqrsum );
}

void cStdDev::loadFromFile(FILE *f)
{
    freadvarsf(f,"");  freadvarsf(f,""); freadvarsf(f,""); freadvarsf(f,"");
    freadvarsf(f,"%ld\t #= num_samples",&num_samples);
    freadvarsf(f,"%lg %lg\t #= min, max", &min_samples, &max_samples);
    freadvarsf(f,"%lg\t #= sum", &sum);
    freadvarsf(f,"%lg\t #= square sum", &sqrsum);
}

//==========================================================================
// cWeightedStdDev - member functions

cWeightedStdDev& cWeightedStdDev::operator=(cWeightedStdDev& res)
{
    cStdDev::operator=(res);
    sum_weights = res.sum_weights;
    return *this;
}


void cWeightedStdDev::collect2(double val, double weight)
{
    cStdDev::collect(val);
    sum_weights += weight;
}

void cWeightedStdDev::clearResult()
{
    cStdDev::clearResult();
    sum_weights=0.0;
}

double cWeightedStdDev::variance()
{
    opp_error("(%s)%s: variance()/stddev() not implemented",
                      isA(), fullName());
    return 0.0;


    // if (sum_weights==0)
    //   return 0.0;
    // else
    // {
    //   double devsqr = (sqrsum - sum*sum/sum_weights)/(sum_weights-1);
    //   if (devsqr<=0)
    //       return 0.0;
    //   else
    //       return devsqr;
    //}
}

void cWeightedStdDev::saveToFile(FILE *f)
{
    cStdDev::saveToFile(f);
    fprintf(f,"%lg\t #= sum_weights\n",sum_weights);
}

void cWeightedStdDev::loadFromFile(FILE *f)
{
    cStdDev::loadFromFile(f);
    freadvarsf(f,"%lg\t #= sum_weights",&sum_weights);
}
