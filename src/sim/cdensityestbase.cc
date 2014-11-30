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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sstream>
#include "globals.h"
#include "cdensityestbase.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

NAMESPACE_BEGIN

using std::ostream;


cDensityEstBase::cDensityEstBase(const char *name) : cStdDev(name)
{
    range_mode = RANGE_AUTO;
    num_firstvals = 100;
    range_ext_factor = 2.0;
    rangemin = rangemax = 0;
    cell_under = cell_over = 0;
    transfd = false;
    firstvals = new double[num_firstvals];  // to match RANGE_AUTO
}

cDensityEstBase::~cDensityEstBase()
{
    delete [] firstvals;
}

void cDensityEstBase::parsimPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, eNOPARSIM);
#else
    cStdDev::parsimPack(buffer);

    buffer->pack(rangemin);
    buffer->pack(rangemax);
    buffer->pack(num_firstvals);
    buffer->pack(cell_under);
    buffer->pack(cell_over);
    buffer->pack(range_ext_factor);
    buffer->pack(range_mode);
    buffer->pack(transfd);

    if (buffer->packFlag(firstvals!=NULL))
        buffer->pack(firstvals, num_vals); // pack the used positions only
#endif
}

void cDensityEstBase::parsimUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw cRuntimeError(this, eNOPARSIM);
#else
    cStdDev::parsimUnpack(buffer);

    buffer->unpack(rangemin);
    buffer->unpack(rangemax);
    buffer->unpack(num_firstvals);
    buffer->unpack(cell_under);
    buffer->unpack(cell_over);
    buffer->unpack(range_ext_factor);
    int tmp;
    buffer->unpack(tmp);
    range_mode = (RangeMode)tmp;
    buffer->unpack(transfd);

    delete[] firstvals;
    firstvals = NULL;
    if (buffer->checkFlag())
    {
        firstvals = new double[num_firstvals];
        buffer->unpack(firstvals, num_vals);
    }
#endif
}

void cDensityEstBase::copy(const cDensityEstBase& res)
{
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
        firstvals = new double[num_firstvals];
        memcpy(firstvals, res.firstvals, num_firstvals*sizeof(double));
    }
}

cDensityEstBase& cDensityEstBase::operator=(const cDensityEstBase& res)
{
    cStdDev::operator=(res);
    copy(res);
    return *this;
}

void cDensityEstBase::merge(const cStatistic *other)
{
    if (dynamic_cast<const cDensityEstBase *>(other) == NULL)
        throw cRuntimeError(this, "Cannot merge non-histogram (non-cDensityEstBase) statistics (%s)%s into a histogram type",
                                  other->getClassName(), other->getFullPath().c_str());

    const cDensityEstBase *otherd = (const cDensityEstBase *)other;

    if (!otherd->isTransformed())
    {
        // easiest and exact solution: simply recollect the observations
        // the other object has collected
        for (int i=0; i<otherd->num_vals; i++)
            collect(firstvals[i]);
    }
    else
    {
        // merge the base class
        cStdDev::merge(otherd);

        // force this object to be transformed as well
        if (!isTransformed())
            transform();

        // make sure that cells are aligned
        if (getNumCells() != otherd->getNumCells())
            throw cRuntimeError(this, "Cannot merge (%s)%s: different number of histogram cells (%d vs %d)",
                                      otherd->getClassName(), otherd->getFullPath().c_str(), getNumCells(), otherd->getNumCells());
        int n = getNumCells();
        for (int i=0; i<=n; i++)
            if (getBasepoint(i) != otherd->getBasepoint(i))
                throw cRuntimeError(this, "Cannot merge (%s)%s: histogram cells are not aligned",
                                          otherd->getClassName(), otherd->getFullPath().c_str());

        // merge underflow/overflow cells
        cell_under += otherd->getUnderflowCell(); //FIXME check overflow!! but this is unsigned long....
        cell_over += otherd->getOverflowCell();

        // then merge cell counters
        doMergeCellValues(otherd);
    }
}

void cDensityEstBase::clearResult()
{
    cStdDev::clearResult();

    transfd = false;
    range_mode = RANGE_AUTO;
    num_firstvals = 100;
    range_ext_factor = 2.0;
    rangemin = rangemax = 0;
    cell_under = cell_over = 0;

    delete [] firstvals;
    firstvals = new double[num_firstvals];  // to match RANGE_AUTO
}

void cDensityEstBase::setRange(double lower, double upper)
{
    if (num_vals>0 || isTransformed())
        throw cRuntimeError(this, "setRange() can only be called before collecting any values");

    range_mode = RANGE_FIXED;
    rangemin = lower;
    rangemax = upper;

    delete [] firstvals;
    firstvals = NULL;    // not needed with RANGE_FIXED
}

void cDensityEstBase::setRangeAuto(int num_fstvals, double range_ext_fct)
{
    if (num_vals>0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    range_mode = RANGE_AUTO;
    num_firstvals = num_fstvals;
    range_ext_factor = range_ext_fct;

    delete [] firstvals;
    firstvals = new double[num_firstvals];
}

void cDensityEstBase::setRangeAutoLower(double upper, int num_fstvals, double range_ext_fct)
{
    if (num_vals>0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    range_mode = RANGE_AUTOLOWER;
    num_firstvals = num_fstvals;
    rangemax = upper;
    range_ext_factor = range_ext_fct;

    delete [] firstvals;
    firstvals = new double[num_firstvals];
}

void cDensityEstBase::setRangeAutoUpper(double lower, int num_fstvals, double range_ext_fct)
{
    if (num_vals>0 || isTransformed())
        throw cRuntimeError(this, "setRange...() can only be called before collecting any values");

    range_mode = RANGE_AUTOUPPER;
    num_firstvals = num_fstvals;
    rangemin = lower;
    range_ext_factor = range_ext_fct;

    delete [] firstvals;
    firstvals = new double[num_firstvals];
}

void cDensityEstBase::setNumFirstVals(int num_fstvals)
{
    if (num_vals>0 || isTransformed())
        throw cRuntimeError(this, "setNumFirstVals() can only be called before collecting any values");

    num_firstvals = num_fstvals;

    // if already allocated, reallocate with the correct size
    if (firstvals)
    {
        delete [] firstvals;
        firstvals = new double[num_firstvals];
    }
}

void cDensityEstBase::setupRange()
{
    //
    // set rangemin and rangemax.
    //   Attempts not to make zero width range (makes it 1.0 wide).
    //
    double c, r;
    switch (range_mode)
    {
      case RANGE_AUTO:
         c = (min_vals+max_vals)/2;
         r = (max_vals-min_vals)*range_ext_factor;
         if (r == 0) r = 1.0; // warning?
         rangemin = c-r/2;
         rangemax = c+r/2;
         break;
      case RANGE_AUTOLOWER:
         if (rangemax <= min_vals)
             rangemin = rangemax-1.0; // warning?
         else
             rangemin = rangemax-(rangemax-min_vals)*range_ext_factor;
         break;
      case RANGE_AUTOUPPER:
         if (rangemin >= max_vals)
             rangemax = rangemin+1.0; // warning?
         else
             rangemax = rangemin+(max_vals-rangemin)*range_ext_factor;
         break;
    }
}

void cDensityEstBase::collect(double val)
{
    if (!isTransformed() && range_mode == RANGE_FIXED)
        transform();

    cStdDev::collect(val); // this also increments num_vals

    if (!isTransformed())
    {
        ASSERT(firstvals);
        firstvals[num_vals-1] = val;

        if (num_vals == num_firstvals)
            transform();
    }
    else
    {
        collectTransformed(val);  // must maintain underflow/overflow cells
    }
}

double cDensityEstBase::getCellPDF(int k) const
{
    if (num_vals == 0) return 0.0;
    double cellsize = getBasepoint(k+1) - getBasepoint(k);
    return cellsize == 0 ? 0.0 : getCellValue(k)/cellsize/getCount();
}

void cDensityEstBase::plotline(ostream& os, const char *pref, double xval, double count, double a)
{
    const int picwidth = 54;  // width of picture
    char buf[101];
    sprintf(buf, "   %s%12f %5g :", pref, xval, count);
    char *s = buf+strlen(buf);
    int x = (int) floor(a*count+.5);
    int k = x<=picwidth ? x : picwidth;
    for (int m = 1;  m<=k;  m++)
        *s++ = '-';
    strcpy(s, x<=picwidth ? "*\n" : ">\n");
    os << buf;
}

std::string cDensityEstBase::detailedInfo() const
{
    std::stringstream os;
    os << cStdDev::detailedInfo();

    if (!isTransformed())
    {
        ASSERT(firstvals || num_vals==0);
        os << "Values collected so far: ";
        for (int i=0; i<num_vals; i++)
            os << (i==0?"":", ") << firstvals[i];
        os << "\n";
    }
    else
    {
        // transformed
        const int picwidth = 55;   // width of picture
        double max = 0;            // biggest cell value
        int nc = getNumCells();    // number of cells
        int k;
        double d;
        for (k = 0; k<nc; k++)
            if ((d = getCellValue(k)) > max)
                max = d;
        double a = (double)picwidth/max;

        os << "Distribution density function:\n";
        for (k = 0; k<nc; k++)
            plotline(os, "< ", getBasepoint(k), (k==0 ? cell_under : getCellValue(k-1)), a);
        plotline(os, ">=", getBasepoint(nc), cell_over, a);
        os << "\n";
    }
    return os.str();
}

void cDensityEstBase::saveToFile(FILE *f) const
{
    cStdDev::saveToFile(f);

    fprintf(f, "%d\t #= transformed\n", transfd);
    fprintf(f, "%d\t #= range_mode\n", range_mode);
    fprintf(f, "%g\t #= range_ext_factor\n", range_ext_factor);
    fprintf(f, "%g %g\t #= range\n", rangemin, rangemax);
    fprintf(f, "%lu %lu\t #= cell_under, cell_over\n", cell_under, cell_over);
    fprintf(f, "%d\t #= num_firstvals\n", num_firstvals);

    fprintf(f, "%d\t #= firstvals[] exists\n", firstvals!=NULL);
    if (firstvals)
        for (int i=0; i<num_vals; i++)
            fprintf(f, " %g\n", firstvals[i]);
}

void cDensityEstBase::loadFromFile(FILE *f)
{
    cStdDev::loadFromFile(f);

    freadvarsf(f, "%d\t #= transformed", &transfd);
    freadvarsf(f, "%d\t #= range_mode", &range_mode);
    freadvarsf(f, "%g\t #= range_ext_factor", &range_ext_factor);
    freadvarsf(f, "%g %g\t #= range", &rangemin, &rangemax);
    freadvarsf(f, "%lu %lu\t #= cell_under, cell_over", &cell_under, &cell_over);
    freadvarsf(f, "%ld\t #= num_firstvals", &num_firstvals);

    int firstvals_exists;
    freadvarsf(f, "%d\t #= firstvals[] exists", &firstvals_exists);

    delete [] firstvals;
    firstvals = NULL;
    if (firstvals_exists)
    {
        firstvals = new double[num_firstvals];
        for (int i=0; i<num_vals; i++)
            freadvarsf(f, " %g", firstvals+i);
    }
}

cDensityEstBase::Cell cDensityEstBase::getCellInfo(int k) const
{
    if (k<0 || k>=getNumCells())
        return Cell();
    Cell c;
    c.lower = getBasepoint(k);
    c.upper = getBasepoint(k+1);
    c.value = getCellValue(k);
    c.relativeFreq = getCellPDF(k);
    return c;
}

const cDensityEstBase::Cell& cDensityEstBase::internalGetCellInfo(int k) const
{
    // only for use in sim_std.msg (each call overwrites the static buffer!)
    static Cell buf;
    buf = getCellInfo(k);
    return buf;
}

NAMESPACE_END

