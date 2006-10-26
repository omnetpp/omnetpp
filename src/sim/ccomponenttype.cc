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

#include <string.h>
#include "ccomponenttype.h"
#include "cmodule.h"
#include "cchannel.h"
#include "cenvir.h"
#include "cexception.h"
#include "globals.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#include "parsim/cplaceholdermod.h"
#endif

//FIXME add separate nedSource() property, so that it can be inspected!!!!

cComponentType::cComponentType(const char *name, const char *description) : cNoncopyableObject(name,false)
{
    setDescription(description);
}

cComponentType *cComponentType::find(const char *name)
{
    return dynamic_cast<cComponentType *>(componentTypes.instance()->get(name));
}

std::string cComponentType::info() const
{
    return desc;
}

//----

cModuleType::cModuleType(const char *name, const char *description) : cComponentType(name, description)
{
}

cModule *cModuleType::create(const char *modname, cModule *parentmod)
{
    return create(modname, parentmod, -1, 0);
}

cModule *cModuleType::create(const char *modname, cModule *parentmod, int vectorsize, int index)
{
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
    mod->setComponentType(this);
    if (vectorsize>=0)
        mod->setIndex(index, vectorsize);
    if (parentmod)
        parentmod->insertSubmodule(mod);

    // set system module (must be done before takeAllObjectsFrom(tmplist) because
    // if parentmod==NULL, mod itself is on tmplist)
    if (!parentmod)
         simulation.setSystemModule(mod);

    // put the object members of the new module to their place
    mod->takeAllObjectsFrom(tmplist);

    // restore defaultowner
    cObject::setDefaultOwner(oldlist);

    // register with cSimulation
    int id = simulation.registerModule(mod);
    mod->setId(id);

    // set up RNG mapping
    ev.getRNGMappingFor(mod);

    // add parameters and gates to the new module
    addParametersGatesTo(mod);

    // notify envir
    ev.moduleCreated(mod);

    // done -- if it's a compound module, buildInside() will do the rest
    return mod;
}

cModule *cModuleType::instantiateModuleClass(const char *classname)
{
    cPolymorphic *obj = cClassFactory::createOne(classname); // this won't return NULL
    cModule *mod = dynamic_cast<cModule *>(obj);
    if (!mod)
        throw new cRuntimeError("class %s is not a module type", classname); //FIXME better msg
    return mod;
}

cModule *cModuleType::createScheduleInit(char *modname, cModule *parentmod)
{
    if (!parentmod)
        throw new cRuntimeError("createScheduleInit(): parent module pointer cannot be NULL "
                                "when creating module named '%s' of type %s", modname, name());
    cModule *mod = create(modname, parentmod);
    mod->buildInside();
    mod->scheduleStart(simulation.simTime());
    mod->callInitialize();
    return mod;
}

cModuleType *cModuleType::find(const char *name)
{
    return dynamic_cast<cModuleType *>(componentTypes.instance()->get(name));
}

//----

cChannelType::cChannelType(const char *name, const char *description) : cComponentType(name, description)
{
}

cChannel *cChannelType::instantiateChannelClass(const char *classname)
{
    cPolymorphic *obj = cClassFactory::createOne(classname); // this won't return NULL
    cChannel *channel = dynamic_cast<cChannel *>(obj);
    if (!channel)
        throw new cRuntimeError("class %s is not a channel type", classname); //FIXME better msg
    return channel;
}

cChannel *cChannelType::create(const char *name, cModule *parentmod)
{
    cContextTypeSwitcher tmp(CTX_BUILD);

    // Object members of the new channel class are collected to tmplist.
    cDefaultList tmplist;
    cDefaultList *oldlist = cObject::defaultOwner();
    cObject::setDefaultOwner(&tmplist);

    // create channel object
    cChannel *channel = createChannelObject();

    // set up channel: set name, channel type, etc
    channel->setName(name);
    channel->setComponentType(this);

    // put the object members of the new module to their place
    parentmod->take(channel); // temporarily -- then connection's src gate will take() it
    channel->takeAllObjectsFrom(tmplist);

    // restore defaultowner
    cObject::setDefaultOwner(oldlist);

    // set up RNG mapping
    ev.getRNGMappingFor(channel);

    // add parameters to the new module
    addParametersTo(channel);

    //FIXME what else?
    // register with simulation?
    // notify ev?

    return channel;
}

cChannelType *cChannelType::find(const char *name)
{
    return dynamic_cast<cChannelType *>(componentTypes.instance()->get(name));
}

