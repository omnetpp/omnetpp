//========================================================================
//
//  COUTVECT.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cOutFileMgr        : manages output files
//    cOutVector         : represents a vector in the output file
//
//   Original version:  Gabor Lencse
//   Rewrite, bugfixes: Andras Varga
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>   // strlen
#include "coutvect.h"
#include "cmodule.h"
#include "csimul.h"
#include "cenvir.h"

//==========================================================================
// cOutFileMgr - see file OUTVECT.H for comments

cOutFileMgr::cOutFileMgr(char *s) : cObject(s)
{
   nextID = 0;
   handle=NULL;
}

cOutFileMgr::~cOutFileMgr()
{
   if (handle)
      fclose(handle);
}

void cOutFileMgr::setFileName(char *s)
{
   // substitute run number into ##'s in file namestr
   fname = s;
   char *p=strstr(fname,"##");
   if (p)
   {
       char runno[10];
       sprintf(runno, "%02.2d", simulation.runNumber());
       for (int i=0; runno[i]!='\0'; i++)
       {
          if (p[i]=='#')
              p[i]=runno[i];
          else
              {opp_error("Run number `%s' does not fit into filename `%s'",runno,s);break;}
       }
   }
}

char *cOutFileMgr::fileName()
{
   return fname;
}

void cOutFileMgr::openFile()
{
   handle = fopen(fname,"a");
   if (handle==NULL)
       opp_error("Cannot open output file `%s'",(char *)fname);
}

void cOutFileMgr::closeFile()
{
   if (handle)
   {
       fclose(handle);
       handle = NULL;
   }
}

void cOutFileMgr::deleteFile()
{
   if (handle)
       closeFile();
   remove(fname);
}

long cOutFileMgr::getNewID()
{
   return nextID++;
}

FILE *cOutFileMgr::filePointer()
{
   if (handle==NULL)
       openFile();
   return handle;
}

//==========================================================================
// cOutVector - see file OUTVECT.H for comments

cOutVector::cOutVector (char *s, int tupl) : cObject(s)
{
   tuple = tupl;
   enabled = TRUE;
   initialised = FALSE;
   ID = simulation.outvectfilemgr.getNewID();
   starttime = 0.0;
   stoptime = 0.0;       // means 'no stop time'

   record_in_inspector = NULL;

   ev.getOutVectorConfig(simulation.contextSimpleModule()->fullPath(),name(),
                          enabled,starttime,stoptime);
}

cOutVector::~cOutVector()
{
}

void cOutVector::setName(char *nam)
{
   cObject::setName(nam);

   ev.getOutVectorConfig(simulation.contextSimpleModule()->fullPath(),name(),
                          enabled,starttime,stoptime);
}

void cOutVector::info(char *buf)
{
   cObject::info(buf);
   strcpy( buf+strlen(buf), tuple==1 ? " (single)" :
                            tuple==2 ? " (pair)" :
                            " (?)" );
}


#define CHECK(fprintf)    if (fprintf<0) opp_error(eOUTVECT)

void cOutVector::record(double value)
{
   if (tuple!=1)
      {opp_error(eNUMARGS,isA(),name(),1);return;}

   if (record_in_inspector)
      record_in_inspector(data_for_inspector,value,0.0);

   if (!enabled) return;

   simtime_t t = simulation.simTime();
   if (t>=starttime && (stoptime==0.0 || t<=stoptime))
   {
      FILE *f = simulation.outvectfilemgr.filePointer();
      if (f==NULL) return;

      if (!initialised)
      {
          CHECK(fprintf(f,"vector %ld  \"%s\"  \"%s\"  1\n", ID,
                  simulation.contextModule()->fullPath(), name()));
          initialised=TRUE;
      }
      CHECK(fprintf(f,"%ld\t%lg\t%lg\n",ID, t, value));
   }

}

void cOutVector::record(double value1, double value2)
{
   if (tuple!=2)
      {opp_error(eNUMARGS,isA(),name(),2);return;}

   if (record_in_inspector)
      record_in_inspector(data_for_inspector,value1,value2);

   if (!enabled) return;

   simtime_t t = simulation.simTime();
   if (t>=starttime && (stoptime==0.0 || t<=stoptime))
   {
      FILE *f = simulation.outvectfilemgr.filePointer();
      if (f==NULL) return;

      if (!initialised)
      {
         CHECK(fprintf(f,"vector %ld  \"%s\"  \"%s\"  2\n", ID,
                 simulation.contextModule()->fullPath(), name()));
         initialised=TRUE;
      }

      CHECK(fprintf(f,"%ld\t%lg\t%lg\t%lg\n",ID, t, value1, value2));
   }

}
#undef CHECK

void cOutVector::enable()
{
   enabled=TRUE;
}

void cOutVector::disable()
{
   enabled=FALSE;
}

void cOutVector::setStartTime(simtime_t t)
{
   starttime=t;
}

void cOutVector::setStopTime(simtime_t t)
{
   stoptime=t;
}

