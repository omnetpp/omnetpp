//=========================================================================
//  CTYPES.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//   Define_Function macros for common <math.h> functions
//
//   Member functions of
//    cModuleInterface: module interface descriptor
//    cModuleType     : module descriptor
//    cChannelType    : channel type (propagation delay, error rate, data rate)
//    cNetworkType    : network initializer object
//    cClassRegister  : class registration (supports createOne() factory)
//
//  Author: Andras Varga
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
#include "cpar.h"
#include "cmodule.h"
#include "cchannel.h"
#include "macros.h"
#include "ctypes.h"
#include "cenvir.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#include "parsim/cplaceholdermod.h"
#endif

//=== Functions  (register them for findFunction())
Define_Function( acos,  1 )
Define_Function( asin,  1 )
Define_Function( atan,  1 )
Define_Function( atan2, 2 )

Define_Function( sin,  1 )
Define_Function( cos,  1 )
Define_Function( tan,  1 )

Define_Function( ceil,  1 )
Define_Function( floor, 1 )

Define_Function( exp,  1 )
Define_Function( pow,  2 )
Define_Function( sqrt, 1 )

Define_Function( fabs, 1 )
Define_Function( fmod, 2 )

Define_Function( hypot, 2 )

Define_Function( log,   1 )
Define_Function( log10, 1 )


//=========================================================================
//=== cModuleInterface - member functions

cModuleInterface::cModuleInterface(const char *name) :
  cObject(name)
{
    numgates = maxnumgates = 0;
    numparams = maxnumparams = 0;

    gatev = NULL;
    paramv = NULL;
}

cModuleInterface::cModuleInterface(const char *name, DeclarationItem *decltable) :
  cObject(name)
{
    numgates = maxnumgates = 0;
    numparams = maxnumparams = 0;

    gatev = NULL;
    paramv = NULL;

    setup(decltable);

    // do consistency check anyway
    checkConsistency();
}

void cModuleInterface::setup(DeclarationItem *decltable)
{
    // Sets up params and params in this cModuleInterface.
    //
    // decltable points to the data table created by the macros:
    //    Interface(...)
    //      Parameter( parname, types )
    //      Gate( gatename, type )
    //    End
    // This function copies the info into two separate vectors:
    //    gatev, paramv
    // The create() member function will use it.

    DeclarationItem *tab;

    int ng = 0, np = 0;
    for (tab=decltable; tab->what!='E'; tab++)
    {
       switch (tab->what)
       {
         case 'G': ng++; break;
         case 'P': np++; break;
       }
    }

    allocateGateDecls(ng);
    allocateParamDecls(np);

    for (tab=decltable; tab->what!='E'; tab++)
    {
        bool isvec; int len; char *s;
        switch (tab->what)
        {
           case 'G':  // GATE(name, Input/Output)
              s = opp_strdup(tab->name);
              isvec = s && (len=strlen(s))>=3 && s[len-2]=='[' && s[len-1]==']';
              if (isvec)
                  s[len-2]='\x0';  // cut off '[]'
              addGateDecl(s, tab->type, isvec);
              delete [] s;
              break;

           case 'P':  // PARAM(name, types)
              addParamDecl(tab->name, tab->types);
              break;
        }
    }
}

cModuleInterface::cModuleInterface(const cModuleInterface& mi) :
  cObject(),
  gatev(NULL),
  paramv(NULL)
{
    setName(mi.name());
    operator=( mi );
}

cModuleInterface::~cModuleInterface()
{
    int i;
    for (i=0; i<numgates; i++)    {delete [] gatev[i].name;}
    for (i=0; i<numparams; i++)   {delete [] paramv[i].name;delete [] paramv[i].types;}
    delete [] gatev;
    delete [] paramv;
}

cModuleInterface& cModuleInterface::operator=(const cModuleInterface&)
{
    throw new cRuntimeError("cModuleInterface cannot copy itself!");
}

void cModuleInterface::allocateGateDecls(int maxngates)
{
    if (numgates>0)
        throw new cRuntimeError(this, "allocateGateDecls(): too late, some gates already added");
    maxnumgates = maxngates;
    gatev = new GateDecl[maxnumgates];
}

void cModuleInterface::allocateParamDecls(int maxnparams)
{
    if (numparams>0)
        throw new cRuntimeError(this, "allocateParamDecls(): too late, some parameters already added");
    maxnumparams = maxnparams;
    paramv = new ParamDecl[maxnumparams];
}

void cModuleInterface::addGateDecl(const char *name, const char type, bool isvector)
{
    if (numgates==maxnumgates)
        throw new cRuntimeError(this, "addGateDecl(): vector full or not yet allocated");
    int k = numgates++;
    gatev[k].name = opp_strdup(name);
    gatev[k].type = type;
    gatev[k].vect = isvector;
}

void cModuleInterface::addParamDecl(const char *name, const char *types)
{
    if (numparams==maxnumparams)
        throw new cRuntimeError(this, "addParamDecl(): vector full or not yet allocated");
    int k = numparams++;
    paramv[k].name = opp_strdup(name);
    paramv[k].types = opp_strdup(types);
}


void cModuleInterface::addParametersGatesTo(cModule *module)
{
    int i;
    for (i=0;i<numgates;i++)
        module->addGate(gatev[i].name, gatev[i].type, gatev[i].vect);
    for (i=0;i<numparams;i++)
    {
        module->addPar(paramv[i].name);
        if (opp_strcmp(paramv[i].types,"S")==0)
        {
            module->par(paramv[i].name) = "";
            module->par(paramv[i].name).setInput(true);
        }
    }
}

int cModuleInterface::numParams()
{
    return numparams;
}

int cModuleInterface::findParam(const char *name)
{
    for (int i=0; i<numparams; i++)
        if (!strcmp(paramv[i].name, name))
            return i;
    return -1;
}

const char *cModuleInterface::paramName(int k)
{
    if (k<0 || k>=numparams) return NULL;
    return paramv[k].name;
}

const char *cModuleInterface::paramType(int k)
{
    if (k<0 || k>=numparams) return NULL;
    return paramv[k].types;
}

bool cModuleInterface::isParamConst(int k)
{
    if (k<0 || k>=numparams) return false;
    return strchr(paramv[k].types, ParType_Const[0])!=NULL;
}

int cModuleInterface::numGates()
{
    return numgates;
}

int cModuleInterface::findGate(const char *name)
{
    for (int i=0; i<numgates; i++)
        if (!strcmp(gatev[i].name, name))
            return i;
    return -1;
}

const char *cModuleInterface::gateName(int k)
{
    if (k<0 || k>=numgates) return NULL;
    return gatev[k].name;
}

char cModuleInterface::gateType(int k)
{
    if (k<0 || k>=numgates) return 'I';
    return gatev[k].type;
}

bool cModuleInterface::isGateVector(int k)
{
    if (k<0 || k>=numgates) return false;
    return gatev[k].vect;
}

void cModuleInterface::checkParametersOf( cModule *mod )
{
    // check parameter types and convert const parameters to constant
    for (int i=0;i<numparams;i++)
    {
        cPar *par = &(mod->par(paramv[i].name));

        if (!strchr(paramv[i].types, '*') && !strchr(paramv[i].types, par->type()))
        {
           throw new cRuntimeError("Module declaration doesn't allow type `%c' for parameter `%s'",
                                   par->type(), par->fullPath().c_str());
        }

        if (strchr(paramv[i].types,'#'))
        {
           par->convertToConst();
        }
    }
}

void cModuleInterface::checkConsistency()
{
    // check if there's somebody else registered with the same name
    cModuleInterface *other = findModuleInterface(name());
    if (other==this)
    {
        // if found ourselves, temporarily remove ourselves from the list and try again
        modinterfaces.instance()->remove(this);
        other = findModuleInterface(name());
        modinterfaces.instance()->add(this);
    }

    // if we're alone (or the 1st one), nothing to do
    if (!other)
        return;

    char *what, *which;
    int id;

    // now check if everything's consistent
    if (numgates!=other->numgates)
          {what="number of gates";goto error1;}
    if (numparams!=other->numparams)
          {what="number of parameters";goto error1;}

    int i;
    for (i=0;i<numparams;i++)
    {
       if (opp_strcmp(paramv[i].name,other->paramv[i].name)!=0)
          {what="names for parameter";id=i;which=paramv[i].name;goto error2;}
       if (paramv[i].types!=NULL || other->paramv[i].types!=NULL)
          if ((paramv[i].types==NULL && other->paramv[i].types!=NULL) ||
              (paramv[i].types!=NULL && other->paramv[i].types==NULL) ||
              strspn(paramv[i].types,other->paramv[i].types)!=strlen(paramv[i].types)||
              strspn(other->paramv[i].types,paramv[i].types)!=strlen(other->paramv[i].types)
             )
              {what="allowed types for parameter";id=i;which=paramv[i].name;goto error2;}
    }
    for (i=0;i<numgates;i++)
    {
       if (opp_strcmp(gatev[i].name,other->gatev[i].name)!=0)
          {what="names for gate #";id=i;which=gatev[i].name;goto error2;}
       if (gatev[i].type!=other->gatev[i].type)
          {what="direction (in/out) for gate";id=i;which=gatev[i].name;goto error2;}
       if (gatev[i].vect!=other->gatev[i].vect)
          {what="vector/scalar type for gate";id=i;which=gatev[i].name;goto error2;}
    }

    // and then remove and throw out the other one
    modinterfaces.instance()->remove(other);
    delete other;
    return;

    error1:
    throw new cRuntimeError("Inconsistent module interfaces (Interface..End) for `%s':"
                            " %s does not match",
                            name(), what);

    error2:
    throw new cRuntimeError("Inconsistent module interfaces (Interface..End) for `%s':"
                            " %s `%s' (id=%d) do not match",
                            name(), what, which, id);
}

//=========================================================================
//=== cModuleType - member functions

cModuleType::cModuleType(const char *classname,
                         const char *interf_name,
                         ModuleCreateFunc cf) :
  cObject(classname)
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


cModuleType::cModuleType(const cModuleType& mi) :
  cObject()
{
    setName(mi.name());
    operator=( mi );
}

cModuleType::~cModuleType()
{
    delete [] interface_name;
}

cModuleType& cModuleType::operator=(const cModuleType& mt)
{
    if (this==&mt) return *this;

    create_func = mt.create_func;
    interface_name = opp_strdup(mt.interface_name);
    iface = mt.iface;
    return *this;
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

    // add parameters and gates to the new module
    cModuleInterface *iface = moduleInterface();
    iface->addParametersGatesTo(mod);

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


cModuleInterface *cModuleType::moduleInterface()
{
    if (!iface)
        iface = findModuleInterface( interface_name );
    if (!iface)
        throw new cRuntimeError(eNOMODIF, interface_name, name());
    return iface;
}


//=========================================================================
//=== cChannelType - member functions

cChannelType::cChannelType(const char *name) : cObject(name)
{
}

//=========================================================================
//=== cLinkType - member functions

cLinkType::cLinkType(const char *name, cPar *(*d)(), cPar *(*e)(), cPar *(*dr)()) :
  cChannelType(name)
{
    delayfunc = d;
    errorfunc = e;
    dataratefunc = dr;
}

cLinkType::cLinkType(const cLinkType& li) : cChannelType()
{
    setName(li.name());
    operator=(li);
}

cChannel *cLinkType::create(const char *name)
{
    cBasicChannel *channel = new cBasicChannel(name);

    cPar *p;
    p = delayfunc ? delayfunc() : NULL;
    if (p)
    {
        p->setName("delay");
        channel->addPar(p);
    }

    p = errorfunc ? errorfunc() : NULL;
    if (p)
    {
        p->setName("error");
        channel->addPar(p);
    }

    p = dataratefunc ? dataratefunc() : NULL;
    if (p)
    {
        p->setName("datarate");
        channel->addPar(p);
    }

    return channel;
}


//=========================================================================
//=== cFunctionType - all functions inline
cFunctionType::cFunctionType(const char *name, MathFuncNoArg f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 0;
    if (ac!=-1 && ac!=0)
        throw new cRuntimeError("Register_Function() or cFunctionType: "
                                "attempt to register function \"%s\" with wrong "
                                "number of arguments %d, should be 0", name, ac);
}

cFunctionType::cFunctionType(const char *name, MathFunc1Arg f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 1;
    if (ac!=-1 && ac!=1)
        throw new cRuntimeError("Register_Function() or cFunctionType: "
                                "attempt to register function \"%s\" with wrong "
                                "number of arguments %d, should be 1", name, ac);
}

cFunctionType::cFunctionType(const char *name, MathFunc2Args f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 2;
    if (ac!=-1 && ac!=2)
        throw new cRuntimeError("Register_Function() or cFunctionType: "
                                "attempt to register function \"%s\" with wrong "
                                "number of arguments %d, should be 2", name, ac);
}

cFunctionType::cFunctionType(const char *name, MathFunc3Args f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 3;
    if (ac!=-1 && ac!=3)
        throw new cRuntimeError("Register_Function() or cFunctionType: "
                                "attempt to register function \"%s\" with wrong "
                                "number of arguments %d, should be 3", name, ac);
}

cFunctionType::cFunctionType(const char *name, MathFunc4Args f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 4;
    if (ac!=-1 && ac!=4)
        throw new cRuntimeError("Register_Function() or cFunctionType: "
                                "attempt to register function \"%s\" with wrong "
                                "number of arguments %d, should be 4", name, ac);
}

MathFuncNoArg cFunctionType::mathFuncNoArg()
{
    if (argc!=0)
        throw new cRuntimeError(this,"mathFuncNoArg(): arg count mismatch (argc=%d)",argc);
    return (MathFuncNoArg)f;
}

MathFunc1Arg cFunctionType::mathFunc1Arg()
{
    if (argc!=1)
        throw new cRuntimeError(this,"mathFunc1Arg(): arg count mismatch (argc=%d)",argc);
    return (MathFunc1Arg)f;
}

MathFunc2Args cFunctionType::mathFunc2Args()
{
    if (argc!=2)
        throw new cRuntimeError(this,"mathFunc2Args(): arg count mismatch (argc=%d)",argc);
    return (MathFunc2Args)f;
}

MathFunc3Args cFunctionType::mathFunc3Args()
{
    if (argc!=3)
        throw new cRuntimeError(this,"mathFunc3Args(): arg count mismatch (argc=%d)",argc);
    return (MathFunc3Args)f;
}

MathFunc4Args cFunctionType::mathFunc4Args()
{
    if (argc!=4)
        throw new cRuntimeError(this,"mathFunc4Args(): arg count mismatch (argc=%d)",argc);
    return (MathFunc4Args)f;
}

cFunctionType *findfunctionbyptr(MathFunc f)
{
    cArray *a = functions.instance();
    for (int i=0; i<a->items(); i++)
    {
        cFunctionType *ff = (cFunctionType *) a->get(i);
        if (ff->mathFunc() == f)
            return ff;
    }
    return NULL;
}

//=========================================================================

cClassRegister::cClassRegister(const char *name, cPolymorphic *(*f)()) : cObject(name)
{
    creatorfunc = f;
}

cPolymorphic *createOne(const char *classname)
{
    cClassRegister *p = (cClassRegister *)classes.instance()->get(classname);
    if (!p)
        throw new cRuntimeError("Class \"%s\" not found -- perhaps its code was not linked in, or the class wasn't registered via Register_Class()", classname);
    return p->createOne();
}

cPolymorphic *createOneIfClassIsKnown(const char *classname)
{
    cClassRegister *p = (cClassRegister *)classes.instance()->get(classname);
    if (!p)
        return NULL;
    return p->createOne();
}

