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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __APPREG_H
#define __APPREG_H

#include "chead.h"
#include "onstartup.h"

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
#define Register_OmnetApp(UINAME,CLASSNAME,ISSLAVE,SCORE,DESCR) \
  TOmnetApp *CLASSNAME##__create(ArgList *args, cIniFile *inifile) {return new CLASSNAME(args, inifile);} \
  EXECUTE_ON_STARTUP(__##CLASSNAME##_ui, (new cOmnetAppRegistration(UINAME,ISSLAVE,SCORE,DESCR,CLASSNAME##__create))->setOwner(&omnetapps);)

class TOmnetApp;
class ArgList;
class cIniFile;

extern ENVIR_API cHead omnetapps;

// registration class
class ENVIR_API cOmnetAppRegistration : public cObject
{
    typedef TOmnetApp *(*AppCreatorFunc)(ArgList *,cIniFile *);
    AppCreatorFunc creatorfunc;
    opp_string desc;
    int scor;
    bool isslave;
  public:
    cOmnetAppRegistration(const char *name, bool isSlave, int score,
                          const char *description, AppCreatorFunc f) :
      cObject(name),
      creatorfunc(f), desc(description), scor(score), isslave(isSlave) {}
    virtual ~cOmnetAppRegistration()  {}

    // redefined functions
    virtual const char *className() const {return "cOmnetAppRegistration";}

    // new functions
    TOmnetApp *createOne(ArgList *args, cIniFile *inifile)  {return creatorfunc(args,inifile);}
    const char *description()  {return desc;}
    int score()  {return scor;}
    bool isSlave()  {return isslave;}
};

#endif

