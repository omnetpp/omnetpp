//=========================================================================
//  CDENSITY.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cDensityEstBase : common base class for density estimation classes
//
//   Authors: Andras Varga
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
#include "macros.h"
#include "cdensity.h"
#include "cexception.h"
#include "errmsg.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;

//=========================================================================
//=== Registration

// cDensityEstBase is an abstract class -- needs no registration

//==========================================================================
// cDensityEstBase - member functions

cDensityEstBase::cDensityEstBase(const char *name ) : cStdDev(name)
{
    range_mode = RANGE_INVALID;
    num_firstvals = 0;
    cell_under = cell_over = 0;

    transfd = 0;
    firstvals = NULL;
}

cDensityEstBase::~cDensityEstBase()
{
    delete [] firstvals;
}

void cDensityEstBase::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cStdDev::netPack(buffer);

    buffer->pack(rangemin);
    buffer->pack(rangemax);
    buffer->pack(num_firstvals);
    buffer->pack(cell_under);
    buffer->pack(cell_over);
    buffer->pack(range_ext_factor);
    buffer->pack(range_mode);
    buffer->pack(transfd);

    if (buffer->packFlag(firstvals!=NULL))
        buffer->pack(firstvals, num_firstvals);
#endif
}

void cDensityEstBase::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cStdDev::netUnpack(buffer);

    buffer->unpack(rangemin);
    buffer->unpack(rangemax);
    buffer->unpack(num_firstvals);
    buffer->unpack(cell_under);
    buffer->unpack(cell_over);
    buffer->unpack(range_ext_factor);
    buffer->unpack(range_mode);
    buffer->unpack(transfd);

    if (buffer->checkFlag())
    {
        firstvals = new double[num_firstvals];
        buffer->unpack(firstvals, num_firstvals);
    }
#endif
}

cDensityEstBase& cDensityEstBase::operator=(const cDensityEstBase& res)
{
    if (this==&res) return *this;

    cStdDev::operator=(res);

    rangemin = res.rangemin;
    rangemax = res.rangemax;
    num_firstvals = res.num_firstvals;
    cell_under = res.cell_under;
    cell_over = res.cell_over;

    range_ext_factor = res.range_ext_factor;
    range_mode = res.range_mode;

    transfd = res.transfd;

    delete [] firstvals;
    firstvals = NULL;
    if (res.firstvals)
    {
        firstvals = new double[ num_firstvals ];
        memcpy(firstvals, res.firstvals, num_firstvals*sizeof(double));
    }
    return *this;
}

// clear results
void cDensityEstBase::clearResult ()
{
    cStdDev::clearResult();

    range_mode = RANGE_INVALID;
    num_firstvals = 0;
    cell_under = cell_over = 0;
    delete [] firstvals;
    firstvals=NULL;
    transfd = false;
}

void cDensityEstBase::setRange(double lower, double upper)
{
    if (num_samples>0 || transformed())
        throw new cRuntimeError(this,"setRange() can only be called before collecting any values");

    range_mode = RANGE_FIXED;
    rangemin = lower;
    rangemax = upper;
}

void cDensityEstBase::setRangeAuto(int num_fstvals, double range_ext_fct)
{
    if (num_samples>0 || transformed())
        throw new cRuntimeError(this,"setRange...() can only be called before collecting any values");

    range_mode = RANGE_AUTO;
    num_firstvals = num_fstvals;
    firstvals = new double [num_firstvals];
    range_ext_factor = range_ext_fct;
}

void cDensityEstBase::setRangeAutoLower(double upper, int num_fstvals, double range_ext_fct)
{
    if (num_samples>0 || transformed())
        throw new cRuntimeError(this,"setRange...() can only be called before collecting any values");

    range_mode = RANGE_AUTOLOWER;
    num_firstvals = num_fstvals;
    firstvals = new double [num_firstvals];
    rangemax = upper;
    range_ext_factor = range_ext_fct;
}

void cDensityEstBase::setRangeAutoUpper(double lower, int num_fstvals, double range_ext_fct)
{
    if (num_samples>0 || transformed())
        throw new cRuntimeError(this,"setRange...() can only be called before collecting any values");

    range_mode = RANGE_AUTOUPPER;
    num_firstvals = num_fstvals;
    firstvals = new double [num_firstvals];
    rangemin = lower;
    range_ext_factor = range_ext_fct;
}

void cDensityEstBase::setNumFirstVals(int num_fstvals)
{
    if (num_samples>0 || transformed())
        throw new cRuntimeError(this,"setNumFirstVals() can only be called before collecting any values");

    num_firstvals = num_fstvals;
    delete [] firstvals;
    firstvals = new double [num_firstvals];
}

void cDensityEstBase::setupRange()
{
    //
    // set rangemin and rangemax.
    //   Attempts not to make zero width range (makes it 1.0 wide).
    //
    double c,r;
    switch (range_mode)
    {
      case RANGE_AUTO:
         c = (min_samples+max_samples)/2;
         r = (max_samples-min_samples)*range_ext_factor;
         if (r==0) r=1.0; // warning?
         rangemin = c-r/2;
         rangemax = c+r/2;
         break;
      case RANGE_AUTOLOWER:
         if (rangemax<=min_samples) rangemin=rangemax-1.0; // warning?
         else rangemin = rangemax-(rangemax-min_samples)*range_ext_factor;
         break;
      case RANGE_AUTOUPPER:
         if (rangemin>=max_samples) rangemax=rangemin+1.0; // warning?
         else rangemax = rangemin+(max_samples-rangemin)*range_ext_factor;
         break;
    }
}

void cDensityEstBase::collect(double val)
{
    if (range_mode == RANGE_INVALID && !transformed())
        setRangeAuto();

    if (firstvals==0 && !transformed())
        transform();

    cStdDev::collect(val); // this also increments num_samples

    if (!transformed())
    {
        firstvals[num_samples-1] = val;

        if (num_samples==num_firstvals)
        {
            transform();  // must set transfd and call setupRange()
            delete [] firstvals;
            firstvals = NULL;
        }
    }
    else
    {
        collectTransformed( val );  // must maintain underflow/overflow cells
    }
}

double cDensityEstBase::cellPDF(int k) const
{
    if (num_samples==0) return 0.0;
    double cellsize = basepoint(k+1) - basepoint(k);
    return cellsize==0 ? 0.0 : cell(k)/cellsize/samples();
}

// plot one line
void cDensityEstBase::plotline(ostream& os, char *pref, double xval,
                               double count, double a )
{
    const int picwidth=54;           // width of picture
    char buf[101], *s;
    int x,m,k;
    sprintf(buf, "   %s%12f %5g :", pref, xval, count);
    s = buf+strlen(buf);
    x = (int) floor(a*count+.5);
    if (x<=picwidth)  k=x;  else k=picwidth;
    for (m=1;  m<=k;  m++)  *s++ = '-';
    if (x<=picwidth) strcpy(s,"*\n");  else strcpy(s,">\n");
    os << buf;
}

std::string cDensityEstBase::detailedInfo() const
{
    if (!transformed())
    {
        // if the histogram is not transformed, we create a temporary copy,
        // transform it and call its detailedInfo() to do the job.
        cDensityEstBase *temp = (cDensityEstBase *)dup();
        temp->transform();
        std::string res = temp->detailedInfo();
        delete temp;
        return res;
    }

    // Now the histogram is surely transformed.
    std::stringstream os;
    os << cStdDev::detailedInfo();

    if (num_samples>1)
    {
        const int picwidth=55;   // width of picture
        double max=0;           // biggest cell value
        int nc = cells();        // number of cells
        int k;
        double d;
        for (k=0; k<nc; k++)
            if ((d=cell(k)) > max)
               max = d;
        double a=(double)picwidth/max;

        os << "Distribution density function:\n";
        for (k=0; k<nc; k++)
            plotline(os,"< ", basepoint(k), (k==0 ? cell_under : cell(k-1)), a);
        plotline(os,">=",basepoint(nc), cell_over, a);
        os << "\n";
    }
    return os.str();
}

void cDensityEstBase::saveToFile(FILE *f) const
{
    cStdDev::saveToFile(f);

    fprintf(f,"%d\t #= transformed\n",transfd);
    fprintf(f,"%d\t #= range_mode\n",range_mode);
    fprintf(f,"%g\t #= range_ext_factor\n",range_ext_factor);
    fprintf(f,"%g %g\t #= range\n",rangemin,rangemax);
    fprintf(f,"%lu %lu\t #= cell_under, cell_over\n",cell_under,cell_over);
    fprintf(f,"%ld\t #= num_firstvals\n",num_firstvals);

    fprintf(f,"%d\t #= firstvals[] exists\n",firstvals!=NULL);
    if (firstvals)
        for (int i=0; i<num_firstvals; i++)
            fprintf(f," %g\n",firstvals[i]);
}

void cDensityEstBase::loadFromFile(FILE *f)
{
    cStdDev::loadFromFile(f);

    freadvarsf(f,"%d\t #= transformed",&transfd);
    freadvarsf(f,"%d\t #= range_mode",&range_mode);
    freadvarsf(f,"%g\t #= range_ext_factor",&range_ext_factor);
    freadvarsf(f,"%g %g\t #= range",&rangemin,&rangemax);
    freadvarsf(f,"%lu %lu\t #= cell_under, cell_over",&cell_under,&cell_over);
    freadvarsf(f,"%ld\t #= num_firstvals",&num_firstvals);

    int firstvals_exists;
    freadvarsf(f,"%d\t #= firstvals[] exists", &firstvals_exists);
    delete [] firstvals; firstvals = NULL;
    if (firstvals_exists)
    {
        firstvals = new double[ num_firstvals ];
        for (int i=0; i<num_firstvals; i++)
            freadvarsf(f," %g",firstvals+i);
    }
}

cDensityEstBase::Cell cDensityEstBase::cellInfo(int k) const
{
    if (k<0 || k>=cells())
        return Cell();
    Cell c;
    c.lower = basepoint(k);
    c.upper = basepoint(k+1);
    c.value = cell(k);
    c.relativeFreq = cellPDF(k);
    return c;
}

