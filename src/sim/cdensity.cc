//=========================================================================
//
//  CDENSITY.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Member functions of
//    cDensityEstBase : common base class for density estimation classes
//
//   Authors: Andras Varga
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
#include "macros.h"
#include "cdensity.h"

//=========================================================================
//=== Registration

// cDensityEstBase is an abstract class -- needs no registration

//==========================================================================
// cDensityEstBase - member functions

cDensityEstBase::cDensityEstBase( char *name ) : cStdDev(name)
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

cDensityEstBase& cDensityEstBase::operator=(cDensityEstBase& res)
{
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
   transfd = FALSE;
}

void cDensityEstBase::setRange(double lower, double upper)
{
    if (num_samples>0 || transformed())
       {opp_error("(%s)%s: setRange() can only be called before collecting any values", isA(), fullName());return;}

    range_mode = RANGE_FIXED;
    rangemin = lower;
    rangemax = upper;
}

void cDensityEstBase::setRangeAuto(int num_fstvals, double range_ext_fct)
{
    if (num_samples>0 || transformed())
       {opp_error("(%s)%s: setRange...() can only be called before collecting any values", isA(), fullName());return;}

    range_mode = RANGE_AUTO;
    num_firstvals = num_fstvals;
    firstvals = new double [num_firstvals];
    range_ext_factor = range_ext_fct;
}

void cDensityEstBase::setRangeAutoLower(double upper, int num_fstvals, double range_ext_fct)
{
    if (num_samples>0 || transformed())
       {opp_error("(%s)%s: setRange...() can only be called before collecting any values", isA(), fullName());return;}

    range_mode = RANGE_AUTOLOWER;
    num_firstvals = num_fstvals;
    firstvals = new double [num_firstvals];
    rangemax = upper;
    range_ext_factor = range_ext_fct;
}

void cDensityEstBase::setRangeAutoUpper(double lower, int num_fstvals, double range_ext_fct)
{
    if (num_samples>0 || transformed())
       {opp_error("(%s)%s: setRange...() can only be called before collecting any values", isA(), fullName());return;}

    range_mode = RANGE_AUTOUPPER;
    num_firstvals = num_fstvals;
    firstvals = new double [num_firstvals];
    rangemin = lower;
    range_ext_factor = range_ext_fct;
}

void cDensityEstBase::setNumFirstVals(int num_fstvals)
{
    if (num_samples>0 || transformed())
       {opp_error("(%s)%s: setNumFirstVals() can only be called before collecting any values", isA(), fullName());return;}

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
      {opp_error("(%s)%s: must call setRange..() before collecting",isA(),fullName());return;}

   if (firstvals==0 && !transformed()) transform();

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

double cDensityEstBase::cellPDF(int k)
{
     if (num_samples==0) return 0.0;
     double cellsize = basepoint(k+1) - basepoint(k);
     return cellsize==0 ? 0.0 : cell(k)/cellsize/samples();
}

// plot one line
void cDensityEstBase::plotline( ostream& os, char *pref, double xval,
                               double count, double a )
{
   const int picwidth=54;           // width of picture
   char buf[101], *s;
   int x,m,k;
   sprintf(buf, "   %s%12lf %5lg :", pref, xval, count);
   s = buf+strlen(buf);
   x = (int) floor(a*count+.5);
   if (x<=picwidth)  k=x;  else k=picwidth;
   for (m=1;  m<=k;  m++)  *s++ = '-';
   if (x<=picwidth) strcpy(s,"*\n");  else strcpy(s,">\n");
   os << buf;
}

// write to stream
void cDensityEstBase::writeContents(ostream& os)
{
   if (!transformed())
   {
       // if the histogram is not transformed, we create a temporary copy,
       // transform it and call its writeContents() function to do the job.
       cDensityEstBase *temp = (cDensityEstBase *)dup();
       temp->transform();
       temp->writeContents( os );
       delete temp;
       return;
   }

   // Now the histogram is surely transformed.

   cStdDev::writeContents( os );         // write statistics

   if (num_samples>1)
   {
      const int picwidth=55;        // width of picture
      double most=0;                // biggest cell value
      int nc = cells();        // number of cells
      int k;
      double d;
      for (k=0; k<nc; k++)
         if ((d=cell(k))>most)
            most = d;
      double a=(double)picwidth/most;

      os << "\n  Distribution density function:\n";
      for (k=0; k<nc; k++)
        plotline(os,"< ", basepoint(k), cell(k), a);
      plotline(os,">=",basepoint(nc),0,a);
      os << "\n";

   }
}

void cDensityEstBase::saveToFile(FILE *f)
{
   cStdDev::saveToFile(f);

   fprintf(f,"%d\t #= transformed\n",transfd);
   fprintf(f,"%d\t #= range_mode\n",range_mode);
   fprintf(f,"%lg\t #= range_ext_factor\n",range_ext_factor);
   fprintf(f,"%lg %lg\t #= range\n",rangemin,rangemax);
   fprintf(f,"%lu %lu\t #= cell_under, cell_over\n",cell_under,cell_over);
   fprintf(f,"%ld\t #= num_firstvals\n",num_firstvals);

   fprintf(f,"%d\t #= firstvals[] exists\n",firstvals!=NULL);
   if (firstvals) for (int i=0; i<num_firstvals; i++) fprintf(f," %lg\n",firstvals[i]);
}

void cDensityEstBase::loadFromFile(FILE *f)
{
   cStdDev::loadFromFile(f);

   freadvarsf(f,"%d\t #= transformed",&transfd);
   freadvarsf(f,"%d\t #= range_mode",&range_mode);
   freadvarsf(f,"%lg\t #= range_ext_factor",&range_ext_factor);
   freadvarsf(f,"%lg %lg\t #= range",&rangemin,&rangemax);
   freadvarsf(f,"%lu %lu\t #= cell_under, cell_over",&cell_under,&cell_over);
   freadvarsf(f,"%ld\t #= num_firstvals",&num_firstvals);

   int firstvals_exists;
   freadvarsf(f,"%d\t #= firstvals[] exists", &firstvals_exists);
   delete [] firstvals; firstvals = NULL;
   if (firstvals_exists)
   {
      firstvals = new double[ num_firstvals ];
      for (int i=0; i<num_firstvals; i++) freadvarsf(f," %lg",firstvals+i);
   }
}
