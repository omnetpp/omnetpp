//=========================================================================
//  CKSPLIT.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cKSplit : implements the k-split algorithm in 1 dimension
//
//   Original version by Babak Fakhamzadeh, TU Delft, Mar-Jun 1996
//   This version written by Andras Varga, 1997-98
//
//=========================================================================
/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "macros.h"
#include "cenvir.h"
#include "cksplit.h"
#include "random.h"
#include "distrib.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;
using std::endl;

//=========================================================================
//=== Registration

Register_Class(cKSplit);

//=========================================================================
//#define DISTRIBUTE_ON_CHILD_SPLIT --obsolete

#if K<2 || (K>2 && K==2*(K/2))
#  error "K must be 2 or a >=3 odd number"
#endif

//==========================================================================
// Cell division criteria
//   They are used to decide whether a cell should be split.

double critdata_default[] = {20, 4, 2};

int critfunc_const(const cKSplit&, cKSplit::Grid& g, int i, double *c)
{
     return g.cells[i] >= c[0];
}

int critfunc_depth(const cKSplit& ks, cKSplit::Grid& g, int i, double *c)
{
     int depth = g.reldepth - ks.rootGrid().reldepth;
     return g.cells[i] >= c[1]*pow(c[2],depth);
}

double divdata_default[] = {0.5};

double divfunc_const(const cKSplit&, cKSplit::Grid&, double, double *d)
{
     return d[0]; // lambda=constant
}

double divfunc_babak(const cKSplit&, cKSplit::Grid& g, double mother, double *d)
{
     int newobs = g.total-g.mother;
     double lambda = newobs / (d[1]*mother);
     return lambda<1.0 ? lambda : 1.0;
}

//==========================================================================

cKSplit::cKSplit(const cKSplit& r) : cDensityEstBase()
{
    setName( r.name() );
    gridv=NULL; iter=NULL;
    operator=(r);
}

cKSplit::cKSplit(const char *name) : cDensityEstBase(name)
{
    num_cells = 0;

    critfunc = critfunc_depth;
    critdata = critdata_default;
    divfunc = divfunc_const;
    divdata = divdata_default;
    rangeext_enabled = true;

    gridv = NULL;
    gridv_size = 0;

    iter = NULL;
}

cKSplit::~cKSplit()
{
    delete [] gridv;
    delete iter;
}

void cKSplit::netPack(cCommBuffer *buffer)
{
    // cDensityEstBase::netPack(buffer);
    throw new cRuntimeError(this,"netPack() not implemented");
}

void cKSplit::netUnpack(cCommBuffer *buffer)
{
    // cDensityEstBase::netUnpack(buffer);
    throw new cRuntimeError(this,"netUnpack() not implemented");
}

cKSplit& cKSplit::operator=(const cKSplit& res)
{
    if (this==&res) return *this;

    cDensityEstBase::operator= (res);

    num_cells = res.num_cells;

    rootgrid = res.rootgrid;
    lastgrid = res.lastgrid;
    gridv_size = res.gridv_size;

    delete [] gridv;
    if (!res.gridv)
       gridv = NULL;
    else
    {
       gridv = new Grid[gridv_size + 1];
       for (int i=1; i<=lastgrid; i++)
          gridv[i] = res.gridv[i];
    }

    critfunc = res.critfunc;
    critdata = res.critdata;
    divfunc = res.divfunc;
    divdata = res.divdata;
    rangeext_enabled = res.rangeext_enabled;

    delete iter;
    iter = NULL;

    return (*this);
}

void cKSplit::writeContents(ostream& os)
{
   cDensityEstBase::writeContents(os);

   int nn = num_samples<=num_cells+1 ? num_samples : num_cells+1; //???

   os << "\n  cells:\n";
   for (int i=0; i<nn; i++)
      os << "       >=" << basepoint(i) << ": " << cell(i) << endl;
}

void cKSplit::setCritFunc(CritFunc _critfunc, double *_critdata)
{
    critfunc = _critfunc;
    critdata = _critdata;
}

void cKSplit::setDivFunc(DivFunc _divfunc, double *_divdata)
{
    divfunc = _divfunc;
    divdata = _divdata;
}

void cKSplit::rangeExtension( bool enabled )
{
    rangeext_enabled = enabled;
}

void cKSplit::resetGrids( int grid )
{
    Grid *g = &(gridv[grid]);
    g->total = g->mother = 0;
    for (int i=0; i<K; i++)
    {
       if (g->cells[i] < 0)
          resetGrids( - g->cells[i] );
       else
          g->cells[i] = 0;
    }
}

void cKSplit::transform()
{
    int i;

    setupRange();
    createRootGrid();

    // Trick to avoid error from cell splits: first, insert observations
    // just to create grid structure; second, reset all cells to zero and
    // insert observations again, with cell splits disabled now.

    for (i=0; i<num_samples; i++)
         collectTransformed( firstvals[i] );

    resetGrids( rootgrid );

    for (i=0; i<num_samples; i++)
         insertIntoGrids( firstvals[i], false );

    transfd = true;
}

void cKSplit::createRootGrid()
{
   gridv_size = 8;
   gridv = new Grid[gridv_size+1];

   rootgrid = 1;
   lastgrid = 1;
   num_cells = K;

   gridv[1].parent = 0;
   gridv[1].reldepth = 0;
   gridv[1].total = 0;
   gridv[1].mother = 0;
   for (int i=0; i<K; i++)
      gridv[1].cells[i] = 0;
}

void cKSplit::collectTransformed(double x)
{
   // see if x fits into current range and act accordingly
   if (x>=rangemin && x<rangemax)
      insertIntoGrids( x, true );
   else if (rangeext_enabled)
      newRootGrids( x );
   else if (x<rangemin)
      cell_under++;
   else if (x>=rangemax)
      cell_over++;
}

void cKSplit::insertIntoGrids(double x, int enable_splits)
{
   int i;

   // new observation fits in the current grid
   double gridmin = rangemin;
   double gridmax = rangemax;
   double cellsize = (gridmax - gridmin) / (double)K;

   int location = rootgrid;

   // keep searching until the right grid has been found
   for(;;)
   {
      gridv[location].total++;

      // calc. the cell in which the new observation falls
      i = (int) ((x-gridmin) / cellsize);

      // guard against rounding errors
      if (i<0)  i=0;
      if (i>=K) i=K-1;

      // exit loop if no subgrid
      if (gridv[location].cells[i] >= 0)
         break;

      // go down to subgrid
      location = - gridv[location].cells[i];

      gridmin += i * cellsize;
      //gridmax = gridmin + cellsize;
      cellsize /= K;
   }

   // arrived at gridv[location].cells[i] -- possibly split this cell
   if (enable_splits && critfunc(*this, gridv[location], i, critdata))
   {
      splitCell( location, i );

      // go down to new subgrid and insert the observation there
      // (NOTE: code is copied here from the above loop)

      location = - gridv[location].cells[i];

      gridmin += i * cellsize;
      //gridmax = gridmin + cellsize;
      cellsize /= K;

      gridv[location].total++;

      i = (int) ((x-gridmin) / cellsize);

      if (i<0)  i=0;
      if (i>=K) i=K-1;
   }

   // increment cell value
   gridv[location].cells[i]++;
}

void cKSplit::splitCell(int grid, int cell)
{
   // make room for new grid
   if (lastgrid == gridv_size)
      expandGridVector();

   // create shorthands for the two grids in question
   Grid& g = gridv[grid];
   Grid& subg = gridv[lastgrid+1];

   // if none of the cells in the current grid has divided yet, than
   // the observations from the mother cell have to be divided as well
#ifdef DISTRIBUTE_ON_CHILD_SPLIT
   if (g.mother>0) distributeMotherObservations( grid );
#endif

   // insert subg into g.cell[cell]
   subg.parent = grid;
   subg.reldepth = g.reldepth+1;
   subg.mother = g.cells[cell];
   subg.total = subg.mother;
   for (int i=0; i<K; i++)
      subg.cells[i] = 0;

   g.cells[cell] = -(lastgrid+1);

   lastgrid++;
   num_cells += K-1;
}

#ifdef DISTRIBUTE_ON_CHILD_SPLIT
void cKSplit::distributeMotherObservations(int grid)
{
   Grid& g = gridv[grid];
   Grid orig = g;

   for(int i=0; i<K; i++)
      g.cells[i] = (unsigned)realCellValue(orig,i); // WRONG!!!

   g.mother = 0;
}
#endif

void cKSplit::newRootGrids(double x)
{
   // new "supergrid" has to be inserted until it includes x
   for(;;)
   {
      // a new grid still has to be inserted around the current root grid
      if (lastgrid == gridv_size)
         expandGridVector();

      int old_rootgrid = rootgrid;
      rootgrid = lastgrid+1;

      lastgrid++;
      num_cells += K-1;

      gridv[old_rootgrid].parent = rootgrid;
      gridv[rootgrid].parent = 0;
      gridv[rootgrid].reldepth = gridv[old_rootgrid].reldepth - 1;
      gridv[rootgrid].total = gridv[old_rootgrid].total;
      gridv[rootgrid].mother = 0;
      for (int i=0; i<K; i++)
         gridv[rootgrid].cells[i] = 0;

      double gridsize = rangemax - rangemin;
#if K==2
      if (x<rangemin)
      {
         gridv[rootgrid].cells[ 1 ] = -old_rootgrid;
         rangemin -= gridsize;
      }
      else // (x>=rangemax)
      {
         gridv[rootgrid].cells[ 0 ] = -old_rootgrid;
         rangemax += gridsize;
      }
#else
      gridv[rootgrid].cells[ (K-1)/2 ] = -old_rootgrid;

      rangemin -= (K-1)/2*gridsize;
      rangemax += (K-1)/2*gridsize;
#endif

      if (x>=rangemin && x<rangemax)
         break;
   }

   // now, insert x into new root grid

   // calc. in which cell x falls
   int i = (int)(K * (x-rangemin) / (rangemax-rangemin));

   // if it falls in the old root grid, we have to correct it
   if (i == (K-1)/2)
   {
      if (x > (rangemax-rangemin)/2)
         i++;
      else
         i--;
   }

   // increment cell value
   gridv[rootgrid].cells[i]++;
   gridv[rootgrid].total++;
}

void cKSplit::expandGridVector()
{
   gridv_size += 8;
   Grid *newgridv = new Grid[gridv_size+1];

   for (int i=1; i<=lastgrid; i++)
      newgridv[i] = gridv[i];

   delete [] gridv;
   gridv = newgridv;
}

double cKSplit::realCellValue (Grid& grid, int i) const
{
   // returns the actual amount of observations in cell 'i' of 'grid'

#ifdef DISTRIBUTE_ON_CHILD_SPLIT
   double mother = grid.mother;
#else
   // might go up until the root grid to collect mother observations
   double mother;
   if (grid.parent==0)
   {
      mother = 0;
   }
   else
   {
      // find (into k) which cell of the parent our 'grid' was
      int k;
      Grid& parentgrid = gridv[grid.parent];
      int gridnum = &grid - gridv;
      for (k=0; k<K; k++)
         if (parentgrid.cells[k]==-gridnum)
            break;

      // parent grid's undistributed observations
      mother = realCellValue(parentgrid,k);
   }
#endif
   // interpolate between even and proportional division of the mother obs.
   double lambda = divfunc(*this,grid,mother,divdata);

   int cell_tot=grid.cells[i]; if (cell_tot<0) cell_tot=gridv[-cell_tot].total;
   int cell_mot=grid.cells[i]; if (cell_mot<0) cell_mot=gridv[-cell_mot].mother;

   double even = mother / K;
   double prop = mother * cell_tot/(grid.total-grid.mother);

   return cell_mot + (1-lambda)*even + lambda*prop;
}

int cKSplit::treeDepth() const
{
   return treeDepth( gridv[rootgrid] );
}

int cKSplit::treeDepth(Grid& grid) const
{
   int d, maxd=0;
   double c;
   for (int i=0; i<K; i++)
   {
      c = grid.cells[i];
      if (c<0)
      {
         d = treeDepth( gridv[-(int)c] );
         if (d>maxd) maxd=d;
      }
   }
   return maxd+1;
}

void cKSplit::printGrids() const
{
   if (!transformed())
   {
      ev << "collecting initial observations; no grids yet.\n";
      return;
   }
   ev << "Range: " << rangemin << "..." << rangemax << "\n";
   ev << "Root grid: #" << rootgrid << "\n";
   for (int i=1; i<=lastgrid; i++)
   {
      ev << "grid #" << i << ": parent=#" << gridv[i].parent;
      ev << "  total=" << gridv[i].total;
      ev << "  mother=" << gridv[i].mother << "  (";

      for (int j=0; j<K; j++)
         if (gridv[i].cells[j] < 0)
            ev << " " << gridv[i].cells[j];
         else
            ev << " " << gridv[i].cells[j];
      ev << " )\n";
   }
}

void cKSplit::iteratorToCell(int cell_nr) const
{
   // create iterator or reinit if it is stale
   iter=0;
   if (!iter)
      {iter=new Iterator(*this); iter_num_samples=num_samples;}
   else if (num_samples!=iter_num_samples)
      {iter->init(*this,cell_nr<num_cells/2); iter_num_samples=num_samples;}

   // drive iterator up or down to reach cell_nr
   if (cell_nr>iter->cellNumber())
      while (cell_nr!=iter->cellNumber())
         (*iter)++;
   else if (cell_nr<iter->cellNumber())
      while (cell_nr!=iter->cellNumber())
         (*iter)--;
}

int cKSplit::cells() const
{
   if (!transformed()) return 0;
   return num_cells;
}

double cKSplit::cell(int nr) const
{
  if (nr>=num_cells) return 0.0;

  iteratorToCell( nr );
  return iter->cellValue();
}

double cKSplit::basepoint (int nr) const
{
  if (nr>=num_cells) return rangemax;

  iteratorToCell( nr );
  return iter->cellMin();
}


/////////////////////////////////////////////////////////////////
// random()
//  This random number generator works by generating a random number
//  between 0 and the total number of observations. After that, it walks
//  through the cells, until the total of observations encountered
//  is larger than the random number drawn.
//
//
//  CODE NOT CLEANED UP BY VA YET!
/////////////////////////////////////////////////////////////////

double cKSplit::random() const
{
   int i;
   //int dp = treeDepth();
   int cd = 1;

   double x = genk_intrand( genk, num_samples);

   int location = rootgrid;

   double xi = rangemin, xa;

   short finish1 = 0;

   while (finish1 == 0)
   {
     i = 0;
     short finish2 = 0;

     double sum = 0;
     double hlp, hlp4;

     while (finish2 == 0)
     { Grid lp = gridv[location];

       if (lp.cells[i] >= 0)
       { hlp = realCellValue (lp,i);}
       else
       { hlp = lp.cells[i];}

       hlp4 = 0;

       if (hlp < 0)
       { int hlp3 = - (int)hlp;
         hlp = gridv[hlp3].total;
         hlp4 = gridv[hlp3].mother;
       }

       if ((sum + hlp + hlp4) > x)
       { finish2 = 1;}
       else
       { sum += hlp + hlp4;
         i ++;
       }
     } //while (finish2..
     x -= sum;

     xi += i * (rangemax - rangemin) / pow ((double)K, cd);

     if (gridv[location].cells[i] < 0)
     { location = -(int)gridv[location].cells[i];
       cd++;
     }
     else
     { finish1 = 1;}

   }     //while (finish1..

   xa = xi + (rangemax - rangemin) / pow ((double)K, cd);

   x = rand () / (double)RAND_MAX * (xa - xi) + xi;
   return x;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// pdf()
//  The location of the observation in the tree of cells is found. After that, the pdf
//  value of that location is calculated and returned. When the observation falls outside of
//  root grid, zero is returned.
//  The vars dpth and cdpth are used to calculate the actual pdf value of a possibly divided
//  cell.
//
//  CODE NOT CLEANED UP BY VA YET!
/////////////////////////////////////////////////////////////////////////////////////////////

double cKSplit::pdf (double x) const
{
   if (!transformed())
     return 0;

   int i;

   int dpth = treeDepth();
   int cdpth = 1;

   double xi = rangemin;             //used for computing the boundary values of the current
   double xa = rangemax;             //location

   int location = rootgrid;     //start at the beginning of the gridv
   short finish = 0;

   while (finish == 0)           //keep searching until the right grid has been found
   {                             //returns the cellnr, in which the point falls
      i = (int)(K * (x - xi) / (xa - xi));

      if (gridv[location].cells[i] < 0)
      {
         cdpth ++;

         location = - gridv[location].cells[i];
         double hlp = xi;

         xi += i * (xa - hlp) / K;
         xa = xi + (xa - hlp) / K;
     }
     else                        //the right grid has been found
         finish = 1;
   } //while...

   Grid lp = gridv[location];

   return realCellValue(lp,i) / pow ((double)K, dpth - cdpth);
}

double cKSplit::cdf (double) const
{
   throw new cRuntimeError(this,"cdf() not implemented");
}

void cKSplit::saveToFile(FILE *f) const
{
   cDensityEstBase::saveToFile(f);

   fprintf(f,"%d\t #= num_cells\n", num_cells);
   fprintf(f,"%d\t #= gridv_size\n", gridv_size);
   fprintf(f,"%d\t #= rootgrid\n", rootgrid);
   fprintf(f,"%d\t #= lastgrid\n", lastgrid);

   fprintf(f,"%d\t #= gridv_exists\n", gridv!=NULL);
   if (gridv)
   {
     for (int i=1; i<=lastgrid; i++)
     {
        fprintf(f,"# grid %d\n", i);
        fprintf(f,"%d\t #= parent\n", gridv[i].parent);
        fprintf(f,"%d\t #= reldepth\n", gridv[i].reldepth);
        fprintf(f,"%ld\t #= total\n", gridv[i].total);
        fprintf(f,"%d\t #= mother\n", gridv[i].mother);
#if K==2
        fprintf(f,"%d %d\t #= cells[0], cells[1]\n", gridv[i].cells[0], gridv[i].cells[1]);
#else
        fprintf(f,"#= cells[]\n");
        for (int j=0; j<K; j++)  fprintf(f," %d\n", gridv[i].cells[j]);
#endif
     }
   }
}

void cKSplit::loadFromFile(FILE *f)
{
   cDensityEstBase::loadFromFile(f);

   freadvarsf(f,"%d\t #= num_cells", &num_cells);
   freadvarsf(f,"%d\t #= gridv_size", &gridv_size);
   freadvarsf(f,"%d\t #= rootgrid", &rootgrid);
   freadvarsf(f,"%d\t #= lastgrid", &lastgrid);

   int gridv_exists;
   freadvarsf(f,"%d\t #= gridv_exists", &gridv_exists);
   delete [] gridv; gridv=NULL;
   if (gridv_exists)
   {
     gridv = new Grid[gridv_size+1];
     for (int i=1; i<=lastgrid; i++)
     {
        freadvarsf(f,"");
        freadvarsf(f,"%d\t #= parent", &gridv[i].parent);
        freadvarsf(f,"%d\t #= reldepth", &gridv[i].reldepth);
        freadvarsf(f,"%ld\t #= total", &gridv[i].total);
        freadvarsf(f,"%d\t #= mother", &gridv[i].mother);
#if K==2
        freadvarsf(f,"%d %d\t #= cells[0], cells[1]", gridv[i].cells+0, gridv[i].cells+1);
#else
        freadvarsf(f,"#= cells[]");
        for (int j=0; j<K; j++)  freadvarsf(f," %d", gridv[i].cells+j);
#endif
     }
   }
}

//========================================================================

cKSplit::Iterator::Iterator(const cKSplit& _ks, bool _beg)
{
   init(_ks,_beg);
}

void cKSplit::Iterator::init(const cKSplit& _ks, bool _beg)
{
   ks = const_cast<cKSplit*>(&_ks);
   grid = ks->rootgrid;
   cellnum = _beg ? 0 : ks->num_cells-1;
   cell = _beg ? 0 : K-1;
   cellsize = (ks->rangemax - ks->rangemin)/K;
   gridmin = ks->rangemin;

   dive( _beg ? 0 : K-1 );
}

void cKSplit::Iterator::dive(int where)
{
   // go into subgrids as deep as possible, along cells[where]
   while (ks->gridv[grid].cells[cell]<0)
   {
      gridmin += cell*cellsize;
      grid = - ks->gridv[grid].cells[cell];

      cell = where;
      cellsize /= K;
   }
}

void cKSplit::Iterator::operator++(int)
{
   if (grid==0)  {init(*ks,1); return;}

   cellnum++;

   cell++;
   while (cell==K)
   {
      // step back to parent
      int i,oldgrid = grid;
      grid = ks->gridv[grid].parent;
      if (grid==0) return;

      // find (into i) which cell of the parent 'grid' was
      for (i=0; i<K; i++)
         if (ks->gridv[grid].cells[i]==-oldgrid)
            break;

      // calc. cellsize and left edge of parent grid
      cellsize *= K;
      gridmin -= i*cellsize;

      cell = i+1;   // actual '++' operation
   }
   dive(0);
}

void cKSplit::Iterator::operator--(int)
{
   if (grid==0)  {init(*ks,0); return;}

   cellnum--;

   cell--;
   while (cell==-1)
   {
      // step to parent
      int i,oldgrid = grid;
      grid = ks->gridv[grid].parent;
      if (grid==0) return;

      // find (into i) which cell of the parent 'grid' was
      for (i=0; i<K; i++)
         if (ks->gridv[grid].cells[i]==-oldgrid)
            break;

      // calc. cellsize and left edge of parent grid
      cellsize *= K;
      gridmin -= i*cellsize;

      cell = i-1;   // actual '--' operation
   }
   dive(K-1);
}

double cKSplit::Iterator::cellValue() const
{
   cKSplit::Grid& g = ks->gridv[grid];
   return ks->realCellValue(g,cell);
}
