//==========================================================================
//  SSM_IFS.H  -   Definitions & Declarations for SSM I/O interfaces for
//                 Discrete System Simulation in OMNeT++
//
//  Simulated system: TUB Northern&Southern FDDI Rings
//
//  Author: Gabor.Lencse@hit.bme.hu
//==========================================================================

//--------------------------------------------------------------*
//  Copyright (C) 1996,97 Gabor Lencse,
//  Technical University of Budapest, Dept. of Telecommunications,
//  Stoczek u.2, H-1111 Budapest, Hungary.
//
//  This file is distributed WITHOUT ANY WARRANTY. See the file
//  `license' for details on this and other legal matters.
//--------------------------------------------------------------*/

#ifndef __SSM_IFS_H
#define __SSM_IFS_H


class SSM_OIF_by_frequency : public cSimpleModule
  {
  public:
  Module_Class_Members(SSM_OIF_by_frequency,cSimpleModule,SSM_OIF_HEAPSIZE)
  virtual void activity();
  };

class SSM_OIF_by_time : public cSimpleModule
  {
  public:
  Module_Class_Members(SSM_OIF_by_time,cSimpleModule,SSM_OIF_HEAPSIZE)
  virtual void activity();
  };

class SSM_IIF : public cSimpleModule
  {
  public:
  Module_Class_Members(SSM_IIF,cSimpleModule,SSM_IIF_HEAPSIZE)
  virtual void activity();
  };

#endif
