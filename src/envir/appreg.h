//==========================================================================
//   APPREG.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    Registration of user interface classes
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __APPREG_H
#define __APPREG_H

#include "chead.h"

//
// Register_OmnetApp() macro, omnetapps list, cOmnetAppRegistration class.
//
// For registering user interfaces as subclasses of TOmnetApp.
// Creating the user interface object allows us to link an envir.dll on
// Windows without having to know in which dll (cmdenv, tkenv) TOmnetApp's
// appropriate subclass will come from. (On Unix, this was no problem anyway...)
//
//  isslave: true for slave applications, used by distributed execution
//           false for normal user interfaces
//  score:   by default, OMNeT++ will choose the user inteface with the
//           highest score when it starts
//  desc:    textual description of the user interface
//


// the macro
#define Register_OmnetApp(CLASSNAME,ISSLAVE,SCORE,DESCR) \
  TOmnetApp *CLASSNAME##__create(int argc, char *argv[]) {return new CLASSNAME(argc,argv);} \
  cOmnetAppRegistration CLASSNAME##__reg(#CLASSNAME,ISSLAVE,SCORE,DESCR,CLASSNAME##__create);

class TOmnetApp;

extern ENVIR_API cHead omnetapps;

// registration class
class ENVIR_API cOmnetAppRegistration : public cObject
{
    TOmnetApp *(*creatorfunc)(int, char *[]);
    opp_string desc;
    int scor;
    bool isslave;
  public:
    cOmnetAppRegistration(const char *name, bool isSlave, int score,
                          const char *description, TOmnetApp *(*f)(int, char *[])) :
      cObject(name,(cObject *)&omnetapps),
      isslave(isSlave), desc(description), scor(score), creatorfunc(f) {}
    virtual ~cOmnetAppRegistration() {}

    // redefined functions
    virtual const char *className()  {return "cOmnetAppRegistration";}

    // new functions
    TOmnetApp *createOne(int argc, char *argv[])  {return creatorfunc(argc,argv);}
    const char *description()  {return desc;}
    int score()  {return scor;}
    bool isSlave()  {return isslave;}
};

#endif

