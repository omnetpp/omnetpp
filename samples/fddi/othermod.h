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
    FDDI_Generator() : cSimpleModule(GENERATOR_STACKSIZE) {f = 0;}
    ~FDDI_Generator() {if (f) fclose(f);}

  protected:
    virtual void activity();
    virtual void InitStatistics() = 0;

    // Returns false at the end of trace file, (if load is taken directly from
    // there. Otherwise it returns true.
    virtual bool RetrieveDestLength(cMessage *, int &, int &) = 0;
    virtual char *GetFileNameSuffix() = 0;
};

class FDDI_GeneratorFromTraceFile : public FDDI_Generator
{
    char *line;

  public:
    FDDI_GeneratorFromTraceFile() {line = 0;}
    ~FDDI_GeneratorFromTraceFile() {delete [] line;}

  protected:
    virtual void InitStatistics();
    virtual bool RetrieveDestLength(cMessage *, int &, int &);
    virtual char *GetFileNameSuffix() {return ".trc";}
};

class FDDI_GeneratorHistogram2x1D : public FDDI_Generator
{
  protected:
    virtual void InitStatistics();
    virtual bool RetrieveDestLength(cMessage *, int &, int &);
    virtual char *GetFileNameSuffix() {return ".hst";}
};

class FDDI_GeneratorPiSquare2x1D : public FDDI_Generator
{
  protected:
    virtual void InitStatistics();
    virtual bool RetrieveDestLength(cMessage *, int &, int &);
    virtual char *GetFileNameSuffix() {return ".psq";}
};

class FDDI_GeneratorKSplit2x1D : public FDDI_Generator
{
  protected:
    virtual void InitStatistics();
    virtual bool RetrieveDestLength(cMessage *, int &, int &);
    virtual char *GetFileNameSuffix() {return ".ksp";}
};

class FDDI_GeneratorKSplit2D : public FDDI_Generator
{
  protected:
    virtual void InitStatistics();
    virtual bool RetrieveDestLength(cMessage *, int &, int &);
    virtual char *GetFileNameSuffix() {return ".k2d";}
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
    FDDI_Address_Generator() : cSimpleModule(GENERATOR_STACKSIZE) {f = 0;}
    ~FDDI_Address_Generator() {if (f) fclose(f);}

  protected:
    virtual void activity();
    virtual void InitStatistics();
    virtual int RetrieveNewAddress(int);
    virtual char *GetFileNameSuffix() {return ".hst";}
};

class FDDI_Generator4Ring : public cSimpleModule
{
  public:
    FDDI_Generator4Ring() : cSimpleModule(GENERATOR_STACKSIZE) {}
    virtual void activity();
};

class FDDI_Generator4Sniffer : public cSimpleModule
{
  public:
    FDDI_Generator4Sniffer() : cSimpleModule(GENERATOR_STACKSIZE) {}
    virtual void activity();
};

class Stat : public cSimpleModule
{
  public:
    Stat() : cSimpleModule(STAT_STACKSIZE) {}
    virtual void activity();
};

class FDDI_Sink : public cSimpleModule
{
  public:
    FDDI_Sink() : cSimpleModule(SINK_STACKSIZE) {}
    virtual void activity();
};

class FDDI_Monitor : public cSimpleModule
{
  public:
    FDDI_Monitor() : cSimpleModule(MONITOR_STACKSIZE) {}
    virtual void activity();
};

class LoadControl : public cSimpleModule
{
  public:
    LoadControl() : cSimpleModule(LOADCONTROL_STACKSIZE) {}
    virtual void activity();
};

#endif

