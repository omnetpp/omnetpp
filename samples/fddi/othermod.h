//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
//  Author: Gabor.Lencse@hit.bme.hu
//

//--------------------------------------------------------------*
//  Copyright (C) 1996,97 Gabor Lencse,
//  Technical University of Budapest, Dept. of Telecommunications,
//  Stoczek u.2, H-1111 Budapest, Hungary.
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------*/

#ifndef __OTHERMOD_H
#define __OTHERMOD_H

#include <omnetpp.h>

struct histogram_plus_address
  {
  cLongHistogram length;
  int dest;
  };

class FDDI_Generator : public cSimpleModule
  {
  protected:
    int RingID;
    FILE *f;
    int my_station_id;
    double LoadMultiplier;

  public:
  FDDI_Generator(const char *namestr, cModule *parentmod):
    cSimpleModule(namestr, parentmod, GENERATOR_HEAPSIZE) { f = 0; }
  ~FDDI_Generator() { if ( f ) fclose(f); }
  virtual void activity();
  protected:
  virtual void InitStatistics() = 0;
  virtual bool RetrieveDestLength(cMessage *, int &, int &)=0;
  // ^ returns false at the end of trace file, (if load is taken directly from
  // there. Otherwise it must return true !!!
  virtual char * FileNameEnding() = 0;
  };

class FDDI_GeneratorFromTraceFile : public FDDI_Generator
  {
  char * line;

  public:
  FDDI_GeneratorFromTraceFile(const char *namestr, cModule *parentmod) :
    FDDI_Generator(namestr, parentmod) { line=0; }
  ~FDDI_GeneratorFromTraceFile() { if ( line ) delete [] line; }
  protected:
  virtual void InitStatistics();
  virtual bool RetrieveDestLength(cMessage *, int &, int &);
  virtual char * FileNameEnding() { return ".trc"; }
  };

class FDDI_GeneratorHistogram2x1D : public FDDI_Generator
  {
  public:
  FDDI_GeneratorHistogram2x1D(const char *namestr, cModule *parentmod):
    FDDI_Generator(namestr, parentmod) { }
  protected:
  virtual void InitStatistics();
  virtual bool RetrieveDestLength(cMessage *, int &, int &);
  virtual char * FileNameEnding() { return ".hst"; }
  };

class FDDI_GeneratorPiSquare2x1D : public FDDI_Generator
  {
  public:
  FDDI_GeneratorPiSquare2x1D(const char *namestr, cModule *parentmod):
    FDDI_Generator(namestr, parentmod) { }
  protected:
  virtual void InitStatistics();
  virtual bool RetrieveDestLength(cMessage *, int &, int &);
  virtual char * FileNameEnding() { return ".psq"; }
  };

class FDDI_GeneratorKSplit2x1D : public FDDI_Generator
  {
  public:
  FDDI_GeneratorKSplit2x1D(const char *namestr, cModule *parentmod):
    FDDI_Generator(namestr, parentmod) { }
  protected:
  virtual void InitStatistics();
  virtual bool RetrieveDestLength(cMessage *, int &, int &);
  virtual char * FileNameEnding() { return ".ksp"; }
  };

class FDDI_GeneratorKSplit2D : public FDDI_Generator
  {
  public:
  FDDI_GeneratorKSplit2D(const char *namestr, cModule *parentmod):
    FDDI_Generator(namestr, parentmod) { }
  protected:
  virtual void InitStatistics();
  virtual bool RetrieveDestLength(cMessage *, int &, int &);
  virtual char * FileNameEnding() { return ".k2d"; }
  };

class FDDI_Address_Generator : public cSimpleModule
  {
  protected:
    int RingID;
    FILE *f;
    int my_station_id;
    //double LoadMultiplier;
    cBag length_histograms;

  public:
  FDDI_Address_Generator(const char *namestr, cModule *parentmod):
    cSimpleModule(namestr, parentmod, GENERATOR_HEAPSIZE) { f = 0; }
  ~FDDI_Address_Generator() { if ( f ) fclose(f); }
  virtual void activity();
  virtual void InitStatistics();
  virtual int RetrieveNewAddress(int);
  virtual char * FileNameEnding() { return ".hst"; }
  };

class FDDI_Generator4Ring : public cSimpleModule
  {
  Module_Class_Members(FDDI_Generator4Ring,cSimpleModule,GENERATOR_HEAPSIZE)
  virtual void activity();
  };

class FDDI_Generator4Sniffer : public cSimpleModule
  {
  Module_Class_Members(FDDI_Generator4Sniffer,cSimpleModule,GENERATOR_HEAPSIZE)
  virtual void activity();
  };

class Stat : public cSimpleModule
  {
  Module_Class_Members(Stat,cSimpleModule,STAT_HEAPSIZE)
  virtual void activity();
  };

class FDDI_Sink : public cSimpleModule
  {
  Module_Class_Members(FDDI_Sink,cSimpleModule,SINK_HEAPSIZE)
  virtual void activity();
  };

class FDDI_Monitor : public cSimpleModule
  {
  Module_Class_Members(FDDI_Monitor,cSimpleModule,MONITOR_HEAPSIZE)
  virtual void activity();
  };

class LoadControl : public cSimpleModule
  {
  public:
  Module_Class_Members(LoadControl,cSimpleModule,LOADCONTROL_HEAPSIZE)
  virtual void activity();
  };

#endif
