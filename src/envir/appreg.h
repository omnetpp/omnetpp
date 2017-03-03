//==========================================================================
//  APPREG.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    Registration of user interface classes
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_APPREG_H
#define __OMNETPP_ENVIR_APPREG_H

#include "omnetpp/cenvir.h"
#include "omnetpp/globals.h"
#include "omnetpp/onstartup.h"
#include "envirdefs.h"

namespace omnetpp {
namespace envir {

//
// Register_OmnetApp() macro, omnetapps list, cOmnetAppRegistration class.
//
// For registering user interfaces as subclasses of cEnvir.
// Creating the user interface object allows us to link an envir.dll on
// Windows without having to know in which dll (cmdenv, tkenv) cEnvir's
// appropriate subclass will come from. (On Unix, this was no problem anyway...)
//
//  score:   by default, OMNeT++ will choose the user interface with the
//           highest score when it starts
//  desc:    textual description of the user interface
//


// the macro
#define Register_OmnetApp(UINAME,CLASSNAME,SCORE,DESCR) \
    static cRunnableEnvir *__FILEUNIQUENAME__() {return new CLASSNAME();} \
    EXECUTE_ON_STARTUP(omnetapps.getInstance()->add(new cOmnetAppRegistration(UINAME,SCORE,DESCR,__FILEUNIQUENAME__)))

extern ENVIR_API cGlobalRegistrationList omnetapps;

// registration class
class ENVIR_API cOmnetAppRegistration : public cOwnedObject
{
    typedef cRunnableEnvir *(*AppCreatorFunc)();
    AppCreatorFunc creatorfunc;
    std::string desc;
    int scor;
  public:
    cOmnetAppRegistration(const char *name, int score, const char *description, AppCreatorFunc f) :
      cOwnedObject(name, false), creatorfunc(f), desc(description), scor(score) {}
    virtual ~cOmnetAppRegistration()  {}

    // redefined functions
    virtual const char *getClassName() const override {return "cOmnetAppRegistration";}

    // new functions
    cRunnableEnvir *createOne()  {return creatorfunc();}
    std::string str() const override {return desc;}
    int getScore()  {return scor;}

    static cOmnetAppRegistration *chooseBest();
};

} // namespace envir
}  // namespace omnetpp

#endif

