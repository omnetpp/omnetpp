//=========================================================================
//  CPSQUARE.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//     cPSquare: calculates quantile values without storing the observations
//
//   Written by Babak Fakhamzadeh, TU Delft, Dec 1996
//   Modifications by Andras Varga
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
#include "cpsquare.h"
#include "cexception.h"
#include "random.h"
#include "distrib.h"
#include "errmsg.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;
using std::endl;

//=========================================================================
//=== Registration

Register_Class(cPSquare);

//=========================================================================
//
cPSquare::cPSquare(const cPSquare& r) : cDensityEstBase()
{
    setName( r.name() );
    operator=(r);
}


cPSquare::cPSquare (const char *name, int b) : cDensityEstBase(name)
{
    transfd = true;

    numcells=b;
    numobs=0;
    n = new int[numcells+2];
    q = new double[numcells+2];

    for (int i=0; i<=numcells+1; i++)
    {
      n[i]=i;
      q[i]=-1e50;       //this should be -(max(double))
    }
}

cPSquare::~cPSquare()
{
    delete [] q;
    delete [] n;
}


void cPSquare::netPack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cDensityEstBase::netPack(buffer);

    buffer->pack(numcells);
    buffer->pack(numobs);

    if (buffer->packFlag(n!=NULL))
        buffer->pack(n, numcells + 2);
    if (buffer->packFlag(q!=NULL))
        buffer->pack(q, numcells + 2);
#endif
}

void cPSquare::netUnpack(cCommBuffer *buffer)
{
#ifndef WITH_PARSIM
    throw new cRuntimeError(this,eNOPARSIM);
#else
    cDensityEstBase::netUnpack(buffer);

    buffer->unpack(numcells);
    buffer->unpack(numobs);

    if (buffer->checkFlag())
    {
        n = new int[numcells + 2];
        buffer->unpack(n, numcells + 2);
    }

    if (buffer->checkFlag())
    {
        q = new double[numcells + 2];
        buffer->unpack(q, numcells + 2);
    }
#endif
}

cPSquare& cPSquare::operator=(const cPSquare& res)
{
    if (this==&res) return *this;

    cDensityEstBase::operator=(res);

    numobs=res.numobs;
    numcells=res.numcells;
    delete [] n;
    delete [] q;
    n=new int[numcells+2];
    q=new double[numcells+2];
    for (int i=0; i<=numcells+1; i++)
    {
      n[i]=res.n[i];
      q[i]=res.q[i];
    }
    return (*this);
}

void cPSquare::giveError()
{
    throw new cRuntimeError(this, "setRange..() and setNumFirstVals() makes no sense with cPSquare");
}

#ifdef NEW_CODE_FROM_PUPPIS_WITH_FLOATING_POINT_EXCEPTION
void cPSquare::collectTransformed(double val)
{
    int i, j;

    numobs++;

    if (numobs <= numcells + 1)
    {
       q[numcells+2-numobs] = val;
    }
    else
    {
      // now numobs==b+1, number of observations is enough to produce 'b' cells,
      // estimation has to be done
      if (numobs == numcells+2) {
         for (i=1; i<numcells+1; i++) {
            for (j=i+1; j<numcells+2; j++) {
               if (q[j] < q[i])
               {
                  double temp = q[i];
                  q[i] = q[j];
                  q[j] = temp;
               }
            }
         }
      }

      int k = 0;        //the cell number in which 'val' falls

      for (i=1; i<=numcells+1; i++)
      {
         if (val <= q[i]) {
            if (i==1)
            {
               q[1] = val;
               k = 1;
            }
            else {
               k = i-1;
            }
            break;
         }
      }

      if (k==0) //the new value falls outside of the current cells
      {
         q[numcells+1] = val;
         k = numcells;
      }

      for (i=k+1; i<=numcells+1; i++)
         n[i] = n[i]+1;

      double d, qd;
      int e;
      for (i=2; i<=numcells; i++)
      {
         d = 1 + (i - 1) * (numobs - 1) / ((double)numcells) - n[i];

         if ((d>=1 && n[i+1]-n[i]>1) || (d<=-1 && n[i-1]-n[i]<-1))
         //if it is possible to adjust the marker position
         {
            d < 0 ? e = -1 : e = 1;

            //try the parabolic formula
            qd = q[i] + e / ((double)(n[i+1] - n[i-1])) * ((n[i] - n[i-1] + e)
                 * (q[i+1] - q[i]) / ((double)(n[i+1] - n[i])) + (n[i+1]
                 - n[i] - e) * (q[i] - q[i-1]) / ((double)(n[i] - n[i-1])));

            if ((qd>q[i-1]) && (qd<q[i+1]))       // if it is possible to fit the new height
               q[i] = qd;                         // then do so (in between the neigbouring heights)
            else                                  // else use the linear formula
               q[i] += e * (q[i+e] - q[i]) / (double)(n[i+e] - n[i]);
            n[i] += e;
         }
      }
    }
}
#else /* OLDER_WORKING_CODE */
void cPSquare::collectTransformed(double val)
{
    int i;

    numobs++;          //an extra observation is added

    if (numobs <= numcells + 1)
    {
      // old code:
      //q[numcells+2-numobs] = val;
      // places val in front, because qsort puts everything at the end,
      // because initialized with q[i]=-max
      //qsort(q, numcells+2, sizeof(*q), CompDouble);

      // insert val into q[]; q[0] is not used!
      for(i=numobs; i>=2 && val<=q[i-1]; i--)
         q[i]=q[i-1];
      q[i]=val;
    }
    else
    // now numobs==b+1, number of observations is enough to produce 'b' cells,
    // estimation has to be done
    { int k = 0;        //the cell number in which 'val' falls

      for (i=1; i<=numcells+1; i++)
      { if (val <= q[i])
        { if (i==1)
          {  q [1] = val;
             k = 1;
          }
          else
          { k = i - 1;}
          break;
        }
      }
      if (k==0) //the new value falls outside of the current cells
      { q[numcells+1]=val;
        k = numcells;
      }
      for (i=k+1; i<=numcells+1; i++)
      { n[i]=n[i]+1;}

      double d, qd;
      int e;
      for (i=2; i<=numcells; i++)
      { d = 1 + (i - 1) * (numobs - 1) / ((double)numcells) - n[i];

        if ((d>=1 && n[i+1]-n[i]>1) || (d<=-1 && n[i-1]-n[i]<-1))
        //if it is possible to adjust the marker position
        { if (d < 0)
          { e = - 1;}
          else
          { e = 1;}
          //try the parabolic formula
          qd = q[i] + e / ((double)(n [i + 1] - n [i - 1])) * ((n [i] - n [i - 1] + e)
               * (q [i + 1] - q [i]) / ((double)(n [i + 1] - n [i])) + (n [i + 1]
               - n [i] - e) * (q [i] - q [i - 1]) / ((double)(n [i] - n [i - 1])));

          if ((qd>q[i-1]) && (qd<q[i+1]))       //if it is possible to fit the new height

          { q [i] = qd;}                        //then do so (in between the neigbouring heights)
          else                                  //else
          { q [i] += e * (q [i + e] - q [i])    //use the linear formula
                     / ((double)(n [i + e]
                     - n [i]));
          }
          n [i] += e;
        }
      }
    }
}
#endif

double cPSquare::random() const
{
    double s;
    int k, l;

    //if (num_obs==0)   // newer, from PUPPIS
    if (numobs<numcells+1)
        throw new cRuntimeError(this,"must collect at least num_cells values before random() can be used");

    s = numobs * genk_dblrand(genk);

    for (int i=1; i<=numcells+1; i++)
    {
       if (s< n[i])
       {
          k=i;
          l=k-1;
          break;
       }
    }

    if (k==1)
       l=k;

    if (numobs<numcells+1)
    {
       k += numcells-numobs+1;
       l += numcells-numobs+1;
    }

    return dblrand()*(q[k]-q[l])+q[l];
}

int cPSquare::cells() const
{
    if (numobs<2)
       return 0;
    else if (numobs<numcells)
       return numobs-1;
    else
       return numcells;
}

double cPSquare::basepoint(int k) const
{
    return q[k+1];
}

double cPSquare::cell(int k) const
{
    return n[k+2] - n[k+1] + (k==0);
}

void cPSquare::writeContents(ostream& os)
{
   cDensityEstBase::writeContents(os);

   int nn = numobs<=numcells+1 ? numobs : numcells+1;

   os << "\n  The quantiles (#(sample: sample<=marker)):\n";
   os << "      #samples\t<=marker\n";
   for (int i=1; i<=nn; i++)
      os << "       " << n[i] << "\t " << q[i] << endl;

}

double cPSquare::cdf(double x) const
{
   // returns 0..1; uses linear approximation between two markers
   for (int i=1; i<numcells+2 ; i++)
      if (q[i]>x)
         return ((x-q[i-1]) / (q[i]-q[i-1]) * (n[i]-n[i-1] + (i==1)) + n[i-1] + (i==1)) / numobs;
   return 1.0;
}

double cPSquare::pdf(double x) const
{
   // returns 0..1; assumes constant PDF within a cell
   for (int i=1 ; i<numcells+2 ; i++)
      if (q[i]>x)
        return (n[i]-n[i-1] + (i==1))/(q[i]-q[i-1])/numobs;
   return 0;
}

void cPSquare::saveToFile(FILE *f) const
{
   cDensityEstBase::saveToFile(f);

   fprintf(f,"%u\t #= numcells\n",numcells);
   fprintf(f,"%ld\t #= numobs\n",numobs);

   int i;
   fprintf(f,"#= n[]\n");
   for (i=0; i<numcells+2; i++)  fprintf(f," %d\n",n[i]);

   fprintf(f,"#= q[]\n");
   for (i=0; i<numcells+2; i++)  fprintf(f," %g\n",q[i]);
}

void cPSquare::loadFromFile(FILE *f)
{
   cDensityEstBase::loadFromFile(f);

   freadvarsf(f,"%u\t #= numcells",&numcells);
   freadvarsf(f,"%ld\t #= numobs",&numobs);

   int i;
   freadvarsf(f,"#= n[]");
   for (i=0; i<numcells+2; i++)  freadvarsf(f," %d",n+i);

   freadvarsf(f,"#= q[]");
   for (i=0; i<numcells+2; i++)  freadvarsf(f," %g",q+i);
}

