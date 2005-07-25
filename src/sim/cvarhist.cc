//=========================================================================
//  CVARHIST.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cVarHistogram     : Variable bin size histogram
//
//   Authors: Gabor Lencse
//   Adapted by: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "macros.h"
#include "random.h"
#include "distrib.h"
#include "cvarhist.h"
#include "cexception.h"
#include "errmsg.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

//=========================================================================
//=== Registration
Register_Class(cVarHistogram);

//=========================================================================
// cVarHistogram - member functions
//
// num_cells = -1 means that 0 bin boundaries are defined ( num_cells+1 is 0 )
cVarHistogram::cVarHistogram(const char *name, int maxnumcells, int transformtype ) :
cHistogramBase(name,-1) //--LG
{
    range_mode = RANGE_NOTSET;
    transform_type = transformtype;
    max_num_cells = maxnumcells;
    bin_bounds = NULL; // no bin bounds are defined

    if ( (transform_type==HIST_TR_AUTO_EPC_DBL ||
         transform_type==HIST_TR_AUTO_EPC_INT) && max_num_cells<2 )
        throw new cRuntimeError(this,"constructor: the maximal number of cells/bin should be >=2");
}

cVarHistogram::~cVarHistogram()
{
    delete [] bin_bounds;
}

void cVarHistogram::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cHistogramBase::netPack(buffer);

    buffer->pack(max_num_cells);
    if (buffer->packFlag(bin_bounds!=NULL))
        buffer->pack(bin_bounds, max_num_cells + 1);
#endif
}

void cVarHistogram::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cHistogramBase::netUnpack(buffer);

    buffer->unpack(max_num_cells);
    if (buffer->checkFlag())
    {
        bin_bounds = new double[max_num_cells + 1];
        buffer->unpack(bin_bounds, max_num_cells + 1);
    }
#endif
}

void cVarHistogram::addBinBound(double x) //--LG
{
    if (transformed())
        throw new cRuntimeError(this,"cannot add bin bound after transform()");

    // create bin_bounds if not exists
    if ( bin_bounds == NULL )
        bin_bounds = new double [max_num_cells+1];

    // expand if full
    if ( num_cells == max_num_cells )
    {
        double * temp = new double [max_num_cells*2+1];
        memcpy(temp,bin_bounds,(max_num_cells+1)*sizeof(double));
        delete [] bin_bounds;
        bin_bounds=temp;
        max_num_cells*=2;
    }

    // insert bound
    int i;
    for (i=num_cells+1; bin_bounds[i]>x; i-- )
        bin_bounds[i] = bin_bounds[i-1]; // shift up bin boundaries
    bin_bounds[i]=x;

    num_cells++;
}

cVarHistogram& cVarHistogram::operator=(const cVarHistogram& res) //--LG
{
    if (this==&res) return *this;

    cHistogramBase::operator=(res);
    // hack: as this ^ uses num_cells instead of max_num_cells, we must correct it:
    if (res.cellv)
    {
        delete [] cellv;
        cellv = new unsigned [max_num_cells];
        memcpy(cellv, res.cellv, num_cells*sizeof(unsigned));
    }

    max_num_cells = res.max_num_cells;
    transform_type = res.transform_type;

    delete [] bin_bounds;
    bin_bounds = NULL;
    if (res.bin_bounds)
    {
        bin_bounds = new double [max_num_cells+1];
        memcpy(bin_bounds, res.bin_bounds, (num_cells+1)*sizeof(double));
    }

    return *this;
}

static int double_compare_function( const void *p1, const void *p2 ) //--LG
{
    double x1 = * (double *) p1;
    double x2 = * (double *) p2;

    if ( x1 == x2 )
        return 0;
    else if ( x1 < x2 )
        return -1;
    else
        return 1;
}

void cVarHistogram::createEquiProbableCells()
{
    if (num_cells>0)
        throw new cRuntimeError(this,"some bin bounds already present when making equi-probable cells");

    if (range_mode != RANGE_NOTSET)
    {
        throw new cRuntimeError(this,"setRange..() only supported with HIST_TR_NO_TRANSFORM mode");

        // // put away samples that are out of range
        // int num_inrange = num_samples;
        // for (i=0; i<num_samples; i++)
        // {
        //    if (firstvals[i]<rangemin || firstvals[i]>=rangemax)
        //    {
        //       // swap [i] and [num_inrange-1]
        //       double tmp = firstvals[i];
        //       firstvals[i]=firstvals[--num_inrange];
        //       firstvals[i]=tmp;
        //    }
        // }
        //
        // // following code should only deal with first num_inrange samples,
        // // and make 2 additional bin bounds (rangemin and rangemax).
    }

    // this version automatically sets the cell boundaries...
    assert(max_num_cells>=2); // maybe 1 is enough...

    // allocate cellv and bin_bounds
    cellv = new unsigned [max_num_cells];
    bin_bounds = new double [max_num_cells+1];

    qsort(firstvals, num_samples, sizeof(double), double_compare_function);

    // expected sample number per cell/bin
    double esnpc = num_samples/(double)max_num_cells;

    int cell;       // index of cell being constructed
    int prev_index; // index of first observation in firstvals[] that will go into cellv[cell]
    int index;      // index of first observation in firstvals[] that will be left for the next cell (cellv[cell+1])

    double prev_boundary; // previous value of boundary
    double boundary;      // firstvals[index]

    // construct cells; last cell will be handled separately
    for ( cell=0, prev_index=0, prev_boundary=firstvals[prev_index],
         rangemin=bin_bounds[0]=firstvals[0], index=prev_index+(int)esnpc;

         cell<max_num_cells-1 && index<num_samples;

         cell++, prev_index=index, prev_boundary=boundary,
         index=(int)MAX(prev_index+esnpc,(cell+1)*esnpc) )
    {
        boundary = firstvals[index];
        if ( boundary == prev_boundary )
        {
            // try to find a greater one
            int j;
            for ( j=index; j<num_samples && firstvals[j] == prev_boundary; j++ )
                 ;
            // remark: either j == num_samples or
            //  prev_boundary == firstvals[j-1] < firstvals[j] holds
            if ( j == num_samples )
                 break; // the cell-th cell/bin will be the last cell/bin
            else
            {
                 index = j; // a greater one was found
                 boundary = firstvals[index];
            }
        }
        else
        {
            // go backwards in firstvals[] to find first observation that
            // equals `boundary' (that is, check if there's a j:
            // j<index, firstvals[j] == firstvals[index])
            int j;
            // sure: prev_boundary==firstvals[prev_index] < boundary
            //       AND index > prev_index (otherwise   ^^^ here '=' would be)
            //        ==> j >= prev_index when firstvals[j] is evaluated
            // for this reason we do not need to check j>=0
            for ( j=index-1; firstvals[j]==boundary; j-- )
                ; // may run 0 or more times
            index = j+1; // unnecessary if cycle ran 0 times
        }
        bin_bounds[cell+1]=boundary;
        cellv[cell]=index-prev_index;
    }

    // the last cell/bin:
    cellv[cell] = num_samples-prev_index;
    // the last boundary:
    rangemax = bin_bounds[cell+1]=firstvals[num_samples-1];

    // correction of the last boundary (depends on DBL/INT)
    if (transform_type == HIST_TR_AUTO_EPC_DBL)
    {
        double range = firstvals[num_samples-1]-firstvals[0];
        double epsilon = range*1e-6;   // hack: value < boundary; not '<='
        rangemax=bin_bounds[cell+1] += epsilon;
    }
    else if (transform_type == HIST_TR_AUTO_EPC_INT)
    {
        rangemax=bin_bounds[cell+1] += 1;  // hack: take the next integer
    }

    // remark: cellv[0]...cellv[cell] are the valid cells
    num_cells = cell+1; // maybe num_cells < max_num_cells
}

void cVarHistogram::transform() //--LG
{
    int i;

    setupRange();

    if (transform_type==HIST_TR_AUTO_EPC_DBL || transform_type==HIST_TR_AUTO_EPC_INT)
    {
        // create bin bounds based on firstvals[]
        createEquiProbableCells();
    }
    else
    {
        assert(transform_type==HIST_TR_NO_TRANSFORM);

        // all manually added bin bounds must be in the range
        if (range_mode != RANGE_NOTSET)
        {
            if (rangemin>bin_bounds[0] || rangemax<bin_bounds[num_cells])
                throw new cRuntimeError(this,"some bin bounds out of preset range");

            if (rangemin<bin_bounds[0]) addBinBound(rangemin);
            if (rangemax>bin_bounds[num_cells]) addBinBound(rangemax);
        }

        // create cell vector and insert observations
        cellv = new unsigned [num_cells];
        for (i=0; i<num_cells; i++) cellv[i]=0;

        for (i=0; i<num_samples; i++)
            collectTransformed( firstvals[i] );
    }
    transfd = true;
}

void cVarHistogram::collectTransformed(double val)
{
    if (val < rangemin) // rangemin == bin_bounds[0]
    {
       cell_under++;
    }
    else if (val >= rangemax) // rangemax == bin_bounds[num_cells]
    {
       cell_over++;
    }
    else // sample falls in the range of ordinary cells/bins
    {
       // rangemin <= val < rangemax
       // binary search
       int lower_index, upper_index, index;
       for ( lower_index = 0, upper_index = num_cells,
             index=(lower_index+upper_index)/2;

             lower_index<index;

             index=(lower_index+upper_index)/2 )
       {
          // cycle invariant: bin_bound[lower_index]<=val<bin_bounds[upper_index]
           if (val < bin_bounds[index])
               upper_index = index;
           else
               lower_index = index;
       }
       // here, bin_bound[lower_index]<=val<bin_bounds[lower_index+1]

       // increment the appropriate counter
       cellv[lower_index]++;
    }
}

// clear results
void cVarHistogram::clearResult() //--LG
{
    cHistogramBase::clearResult();

    delete [] bin_bounds;
    bin_bounds = NULL;
}

// return kth basepoint
double cVarHistogram::basepoint(int k) const
{
    if (k<num_cells+1)
        return bin_bounds[k];
    else
        throw new cRuntimeError(this,"invalid basepoint index %u",k);
}

double cVarHistogram::cell(int k) const
{
    if (k<num_cells)
        return cellv[k];
    else
        throw new cRuntimeError(this,"invalid cell index %u",k);
}

double cVarHistogram::random() const //--LG
{
    if (num_samples==0) return 0L;

    if (num_samples<num_firstvals)
    {
        // randomly select a sample from the stored ones
        return firstvals[genk_intrand(genk,num_samples)];
    }
    else
    {
        double lower, upper;
        // generate in [lower, upper)

        double m = genk_intrand(genk, num_samples-cell_under-cell_over);

        // select a random interval (k-1) and return a random number from
        // that interval generated according to uniform distribution.
        m -= cell_under;
        int k;
        for (k=0; m>=0; k++)
            m -= cellv[k];
        lower = bin_bounds[k-1];
        upper = bin_bounds[k];

        return lower + genk_dblrand(genk)*(upper-lower);
    }
}

double cVarHistogram::pdf(double x) const // --LG
{
    if (!num_samples)
        return 0.0;

    if (!transformed())
        throw new cRuntimeError(this,"pdf(x) cannot be called before histogram is transformed");

    if (x<rangemin || x>=rangemax)
        return 0.0;

    // rangemin <= x < rangemax
    // binary search
    int lower_index, upper_index, index;
    for ( lower_index = 0, upper_index = num_cells,
         index=(lower_index+upper_index)/2;

         lower_index<index;

         index=(lower_index+upper_index)/2 )
    {
        // cycle invariant: bin_bound[lower_index]<=x<bin_bounds[upper_index]
        if (x < bin_bounds[index])
            upper_index = index;
        else
            lower_index = index;
    }

    // here, bin_bound[lower_index]<=x<bin_bounds[lower_index+1]
    return cellv[lower_index]/(bin_bounds[lower_index+1]-bin_bounds[lower_index])/num_samples;
}

double cVarHistogram::cdf(double) const
{
    throw new cRuntimeError(this,"cdf(x) not implemented");
}

void cVarHistogram::saveToFile(FILE *f) const //--LG
{
    cHistogramBase::saveToFile(f);

    fprintf(f,"%d\t #= transform_type\n", transform_type);
    fprintf(f,"%u\t #= max_num_cells\n", max_num_cells);

    fprintf(f,"%d\t #= bin_bounds[] exists\n", bin_bounds!=NULL);
    if (bin_bounds) for (int i=0; i<max_num_cells+1; i++) fprintf(f," %g\n",bin_bounds[i]);
}

void cVarHistogram::loadFromFile(FILE *f)
{
    cHistogramBase::loadFromFile(f);

    freadvarsf(f,"%d\t #= transform_type", &transform_type);
    freadvarsf(f,"%u\t #= max_num_cells", &max_num_cells);

    // increase allocated size of cellv[] to max_num_cells
    if (cellv && max_num_cells>num_cells)
    {
        unsigned int *new_cellv = new unsigned [max_num_cells];
        memcpy(new_cellv, cellv, num_cells*sizeof(unsigned));
        delete [] cellv; cellv = new_cellv;
    }

    int binbounds_exists;
    freadvarsf(f,"%d\t #= bin_bounds[] exists", &binbounds_exists);
    delete [] bin_bounds; bin_bounds = NULL;
    if (binbounds_exists)
    {
        bin_bounds = new double[max_num_cells+1];
        for (int i=0; i<max_num_cells+1; i++) freadvarsf(f," %g",bin_bounds+i);
    }
}



