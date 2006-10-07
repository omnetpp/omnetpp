//=========================================================================
//  CCOMPONENTTYPE.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>          // strlen
#include <stdarg.h>          // va_list
#include "ccomponenttype.h"
#include "cneddeclaration.h"
#include "cmodule.h"
#include "cchannel.h"
#include "globals.h"
#include "cenvir.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#include "parsim/cplaceholdermod.h"
#endif



cModuleType::cModuleType(const char *classname,
                         const char *interf_name,
                         ModuleCreateFunc cf) :
  cNoncopyableObject(classname)
{
    // create_func:
    //   Ptr to a small function that creates a module of type classname.
    //   E.g, if classname is "Processor", create_func points to a function
    //   like this:
    //      static cModule *Processor__create()
    //      {
    //           return (cModule *) new Processor();
    //      }
    //  For each module type, such a function is automatically created by
    //  the Define_Module( classname ) macro.

    create_func = cf;

    // We cannot find() the interface object (and store its pointer) yet,
    // because it might not have been created yet.
    interface_name = opp_strdup(interf_name);
    iface = NULL;
}

cModuleType::~cModuleType()
{
    delete [] interface_name;
}

cModule *cModuleType::create(const char *modname, cModule *parentmod)
{
    return create(modname, parentmod, -1, 0);
}

cModule *cModuleType::create(const char *modname, cModule *parentmod, int vectorsize, int index)
{
    // Creates a module.
    //  In addition to creating an object of the correct type,
    //  this function inserts it into cSimulation's module vector
    //  and adds parameter and gate objects specified in the interface
    //  description.
    cContextTypeSwitcher tmp(CTX_BUILD);

    // Object members of the new module class are collected to tmplist.
    cDefaultList tmplist;
    cDefaultList *oldlist = cObject::defaultOwner();
    cObject::setDefaultOwner(&tmplist);

    // create the new module object
    cModule *mod;
#ifdef WITH_PARSIM
    if (ev.isModuleLocal(parentmod,modname,index))
        mod = createModuleObject();
    else
        mod = new cPlaceHolderModule();
#else
    mod = createModuleObject();
#endif
    // set up module: set name, module type, vector size, parent
    mod->setName(modname);
    mod->setModuleType(this);
    if (vectorsize>=0)
        mod->setIndex(index, vectorsize);
    if (parentmod)
        parentmod->insertSubmodule(mod);

    // set system module (must be done before takeAllObjectsFrom(tmplist) because
    // if parentmod==NULL, mod itself is on tmplist)
    if (!parentmod)
         simulation.setSystemModule( mod );

    // put the object members of the new module to their place
    mod->takeAllObjectsFrom(tmplist);

    // restore defaultowner
    cObject::setDefaultOwner(oldlist);

    // register with cSimulation
    int id = simulation.registerModule(mod);
    mod->setId(id);

    // set up RNG mapping
    ev.getRNGMappingFor(mod);

/*FIXME replace with new code!!!
    // add parameters and gates to the new module
    cNEDDeclaration *iface = moduleNEDDeclaration();
    iface->addParametersGatesTo(mod);
*/
    // notify envir
    ev.moduleCreated(mod);

    // done -- if it's a compound module, buildInside() will do the rest
    return mod;
}

cModule *cModuleType::createModuleObject()
{
    return create_func();
}

void cModuleType::buildInside(cModule *mod)
{
    mod->buildInside();
}

cModule *cModuleType::createScheduleInit(char *modname, cModule *parentmod)
{
    // This is a convenience function to get a module up and running in one step.
    //
    // Should work for simple and compound modules alike.
    // Not applicable if the module:
    //  - has parameters to be set
    //  - gate vector sizes to be set
    //  - gates to be connected before initialize()
    //
    // First creates starter message for the new module(s), then calls
    // initialize() for it (them). This order is important because initialize()
    // functions might contain scheduleAt() calls which could otherwise insert
    // a message BEFORE the starter messages for module...
    //
    if (!parentmod)
        throw new cRuntimeError("createScheduleInit(): parent module pointer cannot be NULL "
                                "when creating module named '%s' of type %s", modname, name());
    cModule *mod = create(modname, parentmod);
    mod->buildInside();
    mod->scheduleStart( simulation.simTime() );
    mod->callInitialize();
    return mod;
}


cNEDDeclaration *cModuleType::moduleNEDDeclaration()
{
    if (!iface)
        iface = cNEDDeclaration::find(interface_name);  //FIXMe check it's really a module declaration
    if (!iface)
        throw new cRuntimeError(eNOMODIF, interface_name, name());
    return iface;
}

