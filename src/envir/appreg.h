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
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __APPREG_H
#define __APPREG_H

#include "globals.h"
#include "onstartup.h"

//
// Register_OmnetApp() macro, omnetapps list, cOmnetAppRegistration class.
//
// For registering user interfaces as subclasses of TOmnetApp.
// Creating the user interface object allows us to link an envir.dll on
// Windows without having to know in which dll (cmdenv, tkenv) TOmnetApp's
// appropriate subclass will come from. (On Unix, this was no problem anyway...)
//
//  score:   by default, OMNeT++ will choose the user interface with the
//           highest score when it starts
//  desc:    textual description of the user interface
//


// the macro
#define Register_OmnetApp(UINAME,CLASSNAME,SCORE,DESCR) \
  TOmnetApp *CLASSNAME##__create(ArgList *args, cIniFile *inifile) {return new CLASSNAME(args, inifile);} \
  EXECUTE_ON_STARTUP(__##CLASSNAME##_ui, (omnetapps.instance()->add(new cOmnetAppRegistration(UINAME,SCORE,DESCR,CLASSNAME##__create))))

class TOmnetApp;
class ArgList;
class cIniFile;

extern ENVIR_API cSingleton<cArray> omnetapps;

// registration class
class ENVIR_API cOmnetAppRegistration : public cObject
{
    typedef TOmnetApp *(*AppCreatorFunc)(ArgList *,cIniFile *);
    AppCreatorFunc creatorfunc;
    opp_string desc;
    int scor;
  public:
    cOmnetAppRegistration(const char *name, int score,
                          const char *description, AppCreatorFunc f) :
      cObject(name),
      creatorfunc(f), desc(description), scor(score) {}
    virtual ~cOmnetAppRegistration()  {}

    // redefined functions
    virtual const char *className() const {return "cOmnetAppRegistration";}

    // new functions
    TOmnetApp *createOne(ArgList *args, cIniFile *inifile)  {return creatorfunc(args,inifile);}
    const char *description()  {return desc.c_str();}
    int score()  {return scor;}
};

#endif

