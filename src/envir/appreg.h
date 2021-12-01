//==========================================================================
//  APPREG.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Declaration of the following classes:
//    Registration of user interface classes
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

#define Register_OmnetApp(UINAME,CLASSNAME,SCORE,DESCR) \
    static cRunnableEnvir *__FILEUNIQUENAME__() {return new CLASSNAME();} \
    EXECUTE_ON_STARTUP(omnetapps.getInstance()->add(new cOmnetAppRegistration(UINAME,SCORE,DESCR,__FILEUNIQUENAME__)))

extern ENVIR_API cGlobalRegistrationList omnetapps;

/**
 * Class for registering user interfaces as subclasses of cRunnableEnvir.
 * User interfaces have an associated score, and on startup, OMNeT++ will
 * instantiate the one with the highest score.
 */
class ENVIR_API cOmnetAppRegistration : public cOwnedObject
{
    typedef cRunnableEnvir *(*AppCreatorFunc)();
    AppCreatorFunc factoryFunction;
    std::string description;
    int score;
  public:
    cOmnetAppRegistration(const char *name, int score, const char *description, AppCreatorFunc f) :
      cOwnedObject(name, false), factoryFunction(f), description(description), score(score) {}
    cRunnableEnvir *createOne()  {return factoryFunction();}
    std::string str() const override {return description;}
    int getScore()  {return score;}

    static cOmnetAppRegistration *chooseBest();
};

}  // namespace envir
}  // namespace omnetpp

#endif

