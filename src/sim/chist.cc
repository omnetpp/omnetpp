//=========================================================================
//
//  CHIST.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Member functions of
//    cHistogramBase    : common base class for histogram classes
//    cEqdHistogramBase : Equi-distant histograms
//    cLongHistogram    : long integer histogram
//    cDoubleHistogram  : double histogram
//
//   Authors: Andras Varga, Gabor Lencse
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "macros.h"
#include "cdetect.h"
#include "chist.h"

#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

//=========================================================================
//=== Registration

Register_Class( cLongHistogram )
Register_Class( cDoubleHistogram )

//==========================================================================
// cHistogramBase - member functions

cHistogramBase::cHistogramBase( char *namestr, int numcells) :
cDensityEstBase(namestr)
{
   cellv = NULL;
   num_cells = numcells;
}

cHistogramBase::~cHistogramBase()
{
   delete [] cellv;
}

cHistogramBase& cHistogramBase::operator=(cHistogramBase& res)
{
   cDensityEstBase::operator=(res);

   num_cells = res.num_cells;
   delete [] cellv;  cellv = NULL;
   if (res.cellv)
   {
      cellv = new unsigned[num_cells];
      memcpy(cellv, res.cellv, num_cells*sizeof(unsigned));
   }
   return *this;
}

void cHistogramBase::clearResult ()
{
   cDensityEstBase::clearResult();

   num_cells = 0;
   delete [] cellv;
   cellv=NULL;
}

void cHistogramBase::transform()
{
   int i;
   cellv = new unsigned [num_cells];
   for (i=0; i<num_cells; i++) cellv[i]=0;

   setupRange();
   for (i=0; i<num_samples; i++) collectTransformed( firstvals[i] );
   transfd = TRUE;
}

int cHistogramBase::cells()
{
   if (!transformed()) return 0;

   return num_cells;
}

// write to stream
void cHistogramBase::writeContents(ostream& os)
{
   if (!transformed())
   {
       // if the histogram is not transformed, we create a temporary copy,
       // transform it and call its writeContents() function to do the job.
       cHistogramBase *temp = (cHistogramBase *)dup();
       temp->transform();
       temp->writeContents( os );
       delete temp;
       return;
   }

   // Now the histogram is surely transformed.

   cDensityEstBase::writeContents(os);         // write statistics

   if (num_samples>1)
   {
      const int picwidth=55;        // width of picture
      double most=0;                // biggest cell value
      int nc = cells();             // number of cells
      int k;
      double d;
      for (k=0; k<nc; k++)
         if ((d=cell(k))>most)
            most = d;
      double a=(double)picwidth/most;

      os << "\n  Distribution density function:\n";
      for (k=0; k<nc; k++)
        plotline(os,"< ", basepoint(k), (int) cell(k), a);
      plotline(os,">=",basepoint(nc),0,a);
      os << "\n";
   }
}

void cHistogramBase::saveToFile(FILE *f)
{
   cDensityEstBase::saveToFile(f);
   fprintf(f,"%d\t #= num_cells\n", num_cells);
   fprintf(f,"%d\t #= cellv[] exists\n", cellv!=NULL);
   if (cellv) for (int i=0; i<num_cells; i++) fprintf(f," %u\n",cellv[i]);
}

void cHistogramBase::loadFromFile(FILE *f)
{
   cDensityEstBase::loadFromFile(f);
   freadvarsf(f,"%d\t #= num_cells", &num_cells);

   int cellv_exists;
   freadvarsf(f,"%d\t #= cellv[] exists", &cellv_exists);
   delete [] cellv; cellv = NULL;
   if (cellv_exists)
   {
      cellv = new unsigned[num_cells];
      for (int i=0; i<num_cells; i++) freadvarsf(f," %u",cellv+i);
   }
}


//=========================================================================
// cEqdHistogramBase - member functions

cEqdHistogramBase::cEqdHistogramBase(char *namestr, int numcells) :
cHistogramBase(namestr,numcells)
{
   cellsize=0;
}

cEqdHistogramBase& cEqdHistogramBase::operator=(cEqdHistogramBase& res)
{
   cHistogramBase::operator=(res);
   cellsize = res.cellsize;
   return *this;
}

void cEqdHistogramBase::setupRange()
{
    cHistogramBase::setupRange();

    cellsize = (rangemax - rangemin) / num_cells;
}

void cEqdHistogramBase::collectTransformed (double val)
{
   if (val>=rangemax)  // sample falls out of [rangemin, rangemax]: overflow
      cell_over++;
   else if (val<rangemin) // sample falls out of [rangemin, rangemax]: underflow
      cell_under++;
   else
      cellv[unsigned((val-rangemin)/cellsize)]++;
}

double cEqdHistogramBase::pdf(double x)
{
   if (!transformed())
   {
      opp_error("(%s)%s: pdf(x) cannot be called before histogram is transformed", className(),name());
      return 0;
   }

   if (x<rangemin || x>rangemax)
      return 0.0;

   return cellv[(unsigned)((x-rangemin)/cellsize)]/cellsize/num_samples;
}

double cEqdHistogramBase::cdf(double)
{
   opp_error("(%s)%s: cdf() not implemented",className(), name());
   return 0.0;
}

// return kth basepoint
double cEqdHistogramBase::basepoint(int k)
{
    //   k=0           : rangemin
    //   k=1,2,...     : rangemin + k*cellsize
    //   k=num_cells   : rangemax

    if (k==0)
      return rangemin;
    else if (k<num_cells)
      return rangemin + k*cellsize;
    else if (k==num_cells)
      return rangemax;
    else
      {opp_error("(%s)%s: invalid basepoint index %u",className(),name(),k);return 0;}
}

double cEqdHistogramBase::cell(int k)
{
    if (k<num_cells)
      return cellv[k];
    else
      {opp_error("(%s)%s: invalid cell index %u",className(),name(),k);return 0;}
}

void cEqdHistogramBase::saveToFile(FILE *f)
{
    cHistogramBase::saveToFile(f);
    fprintf(f,"%lg\t #= cellsize\n", cellsize);
}

void cEqdHistogramBase::loadFromFile(FILE *f)
{
    cHistogramBase::loadFromFile(f);
    freadvarsf(f,"%lg\t #= cellsize", &cellsize);
}

//=========================================================================
// cLongHistogram - member functions

cLongHistogram::cLongHistogram(char *namestr, int numcells) :
cEqdHistogramBase(namestr,numcells)
{
}

cLongHistogram::~cLongHistogram()
{
}

void cLongHistogram::collect(double val)
{
     cEqdHistogramBase::collect( floor(val) );
}

void cLongHistogram::setupRange()
{
    cHistogramBase::setupRange();

    // adjust range so that cellsize be an integer
    rangemin = floor(rangemin)-0.5;
    rangemax = ceil(rangemax)+0.5;

    cellsize = (rangemax-rangemin)/num_cells;
    cellsize = ceil(cellsize);
    double newrange = cellsize*num_cells;
    double rangediff = newrange - (rangemax-rangemin);

    switch (range_mode)
    {
      case RANGE_AUTO:
         rangemin -= floor(rangediff/2);
         rangemax += rangediff - floor(rangediff/2);
         break;
      case RANGE_AUTOLOWER:
         rangemin = rangemax - newrange;
         break;
      case RANGE_AUTOUPPER:
         rangemax = rangemin + newrange;
         break;
    }
}

double cLongHistogram::random()
{
   if( num_samples==0 )
        return 0L;
   else if( num_samples<num_firstvals )
   {
        // randomly select a sample from the stored ones
        return firstvals[genk_intrand(genk,num_samples)];
   }
   else
   {
        // min_samples, max_samples: integer-valued doubles (e.g.: -3.0, 5.0)
        // rangemin, rangemax: doubles like -1.5, 4.5 (integer+0.5)
        // cellsize: integer-valued double, >0
        double m = genk_intrand(genk,num_samples-cell_under-cell_over);

        // select a random interval (k-1) and return a random number from
        // that interval generated according to uniform distribution.
        int k;
        for (k=0; m>=0; k++)
            m -= cellv[k];

        return ceil(rangemin) + (k-1)*(long)cellsize + genk_intrand(genk, (long)cellsize);
   }
}

//==========================================================================
// cDoubleHistogram - member functions

cDoubleHistogram::cDoubleHistogram(char *namestr, int numcells) :
cEqdHistogramBase(namestr,numcells)
{
}

cDoubleHistogram::~cDoubleHistogram()
{
}

double cDoubleHistogram::random()
{
   if( num_samples==0 )
        return 0L;
   else if( num_samples<num_firstvals )
   {
        // randomly select a sample from the stored ones
        return firstvals[genk_intrand(genk,num_samples)];
   }
   else
   {
        double m = genk_intrand(genk,num_samples-cell_under-cell_over);

        // select a random interval (k-1) and return a random number from
        // that interval generated according to uniform distribution.
        int k;
        for (k=0; m>=0; k++)
           m -= cellv[k];
        return rangemin + (k-1)*cellsize + genk_dblrand(genk)*cellsize;
   }
}

