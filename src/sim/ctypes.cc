//=========================================================================
//
//  CTYPES.CC - part of
//                            OMNeT++
//             Discrete System Simulation in C++
//
//   Define_Function macros for common <math.h> functions
//
//   Member functions of
//    cModuleInterface: module interface descriptor
//    cModuleType     : module descriptor
//    cLinkType       : channel type (propagation delay, error rate, data rate)
//    cNetworkType    : network initializer object
//    cClassRegister  : class registration (supports createOne() factory)
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>          // strlen
#include <stdarg.h>          // va_list
#include "cpar.h"
#include "cmodule.h"
#include "macros.h"
#include "ctypes.h"
#include "cexception.h"

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

cModuleInterface::cModuleInterface(const char *name, sDescrItem *descr_tab ) :
  cObject(name)
{
    gatev = NULL;
    paramv = NULL;
    machinev = NULL;

    setup( descr_tab );

    // if no interface has been registered with this name, register ourselves
    if (modinterfaces.find(name)!=NULL)  // FIXME!!!!!!!!
    {
        setOwner( &modinterfaces );
    }

    // do consistency check anyway
    check_consistency();
}

void cModuleInterface::setup( sDescrItem *descr_tab )
{
    // Sets up gates and params in this cModuleInterface.
    //
    // descr_tab points to the data table created by the macros:
    //    Interface(...)
    //      Parameter( parname, types )
    //      Gate( gatename, type )
    //      Machine( machinename )
    //    End
    // This function copies the info into three separate vectors:
    //    gatev, paramv, machinev
    // The create() member function will use it.

    sDescrItem *tab;

    ngate = nparam = nmachine = 0;
    for (tab=descr_tab; tab->what!='E'; tab++)
    {
       switch( tab->what ) {
         case 'G': ngate++;    break;
         case 'P': nparam++;   break;
         case 'M': nmachine++; break;
       }
    }
    if (nmachine==0) nmachine=1;  // machine parameter "default"

    allocate( ngate, nparam, nmachine ); // allocate vectors of gates and params

    int g=0, p=0, m=0;      // fill vector with data
    for (tab=descr_tab; tab->what!='E'; tab++)
    {
        bool v; int l; char *s;
        switch (tab->what) {
           case 'G':  // GATE( name, Input/Output )
              s = opp_strdup( tab->name );
              v = s && (l=strlen(s))>=3 && s[l-2]=='[' && s[l-1]==']';
              if (v)  s[ strlen(s)-2 ]='\x0';  // cut off '[]'
              gatev[g].name = s;
              gatev[g].type = tab->type;
              gatev[g].vect = v;
              g++;
              break;

           case 'P':  // PARAM( name, types )
              paramv[p].name = opp_strdup( tab->name );
              paramv[p].types = opp_strdup( tab->types );
              p++;
              break;

           case 'M':  // MACHINE( name )
              machinev[m].name = opp_strdup( tab->name );
              m++;
              break;
        }
    }
    if (m==0) machinev[m].name = opp_strdup( "default" ); // add "default"
}

cModuleInterface::cModuleInterface(const cModuleInterface& mi) :
  cObject(),
  gatev(NULL),
  paramv(NULL),
  machinev(NULL)
{
    setName(mi.name());
    operator=( mi );
}

cModuleInterface::~cModuleInterface()
{
    int i;
    for (i=0; i<ngate; i++)    {delete gatev[i].name;}
    for (i=0; i<nparam; i++)   {delete paramv[i].name;delete paramv[i].types;}
    for (i=0; i<nmachine; i++) {delete machinev[i].name;}
    delete [] gatev;
    delete [] paramv;
    delete [] machinev;
}

cModuleInterface& cModuleInterface::operator=(const cModuleInterface&)
{
    throw new cException("cModuleInterface cannot copy itself!");
}

void cModuleInterface::allocate( int ngte, int npram, int nmach )
{
    ngate = ngte;
    gatev = new sGateInfo[ngate];

    nparam = npram;
    paramv = new sParamInfo[nparam];

    nmachine = nmach;
    machinev = new sMachineInfo[nmachine];

    if ((ngate&&!gatev) || (nparam&&!paramv) || (nmachine&&!machinev))
        throw new cException(eNOMEM);
}

void cModuleInterface::addParametersGatesTo( cModule *module)
{
    int i;
    for (i=0;i<nmachine;i++)
       module->addMachinePar( machinev[i].name );
    for (i=0;i<ngate;i++)
       module->addGate( gatev[i].name, gatev[i].type );
    for (i=0;i<nparam;i++)
    {
       module->addPar( paramv[i].name );
       if (opp_strcmp(paramv[i].types,"S")==0)
       {
          module->par( paramv[i].name ) = "";
          module->par( paramv[i].name ).setInput(true);
       }
    }
}

void cModuleInterface::checkParametersOf( cModule *mod )
{
    // check parameter types and convert const parameters to constant
    for (int i=0;i<nparam;i++)
    {
        cPar *par = &(mod->par( paramv[i].name ));

        if (!strchr(paramv[i].types, '*') &&
            !strchr(paramv[i].types, par->type()))
        {
           throw new cException("Module declaration doesn't allow type `%c' for parameter `%s'",
                                par->type(), par->fullPath());
        }

        if (strchr(paramv[i].types,'#'))
        {
           par->convertToConst();
        }
    }
}

void cModuleInterface::check_consistency()
{
    cObject *list = owner(); setOwner(NULL);
    cModuleInterface *m = findModuleInterface( name() );
    if (!m)
    {
         setOwner(list);
         return;
    }

    char *what,*which;
    int id;

    if (ngate!=m->ngate)
          {what="number of gates";goto error1;}
    if (nparam!=m->nparam)
          {what="number of parameters";goto error1;}
    if (nmachine!=m->nmachine)
          {what="number of machine pars";goto error1;}

    int i;
    for (i=0;i<nmachine;i++)
    {
       if (opp_strcmp(machinev[i].name,m->machinev[i].name)!=0)
          {what="names for machine par";id=i;which=machinev[i].name;goto error2;}
    }
    for (i=0;i<nparam;i++)
    {
       if (opp_strcmp(paramv[i].name,m->paramv[i].name)!=0)
          {what="names for parameter";id=i;which=paramv[i].name;goto error2;}
       if (paramv[i].types!=NULL || m->paramv[i].types!=NULL)
          if ((paramv[i].types==NULL && m->paramv[i].types!=NULL) ||
              (paramv[i].types!=NULL && m->paramv[i].types==NULL) ||
              strspn(paramv[i].types,m->paramv[i].types)!=strlen(paramv[i].types)||
              strspn(m->paramv[i].types,paramv[i].types)!=strlen(m->paramv[i].types)
             )
              {what="allowed types for parameter";id=i;which=paramv[i].name;goto error2;}
    }
    for (i=0;i<ngate;i++)
    {
       if (opp_strcmp(gatev[i].name,m->gatev[i].name)!=0)
          {what="names for gate #";id=i;which=gatev[i].name;goto error2;}
       if (gatev[i].type!=m->gatev[i].type)
          {what="direction (in/out) for gate";id=i;which=gatev[i].name;goto error2;}
       if (gatev[i].vect!=m->gatev[i].vect)
          {what="vector/scalar type for gate";id=i;which=gatev[i].name;goto error2;}
    }
    return;

    error1:
    throw new cException("Inconsistent module interfaces (Interface..End) for `%s':"
                     " %s does not match",
                     name(), what);

    error2:
    throw new cException("Inconsistent module interfaces (Interface..End) for `%s':"
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
    //      static cModule *Processor__create(char *name, cModule *parentmod )
    //      {
    //           return (cModule *) new Processor(name, parentmod);
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
    delete interface_name;
}

cModuleType& cModuleType::operator=(const cModuleType& mt)
{
    if (this==&mt) return *this;

    create_func = mt.create_func;
    interface_name = opp_strdup(mt.interface_name);
    iface = mt.iface;
    return *this;
}

cModule *cModuleType::create(const char *modname, cModule *parentmod, bool local)
{
    // Creates a module.
    //  In addition to creating an object of the correct type,
    //  this function inserts it into cSimulation's module vector
    //  and adds parameter and gate objects specified in the interface
    //  description.

    cModule *mod;

    // Object members of the new module class are collected to
    // the temporary list classmembers.
    cHead classmembers;
    cHead *oldl = simulation.localList();
    simulation.setLocalList( &classmembers );

    // create the new module object
    if (local)
        mod = create_func(modname, parentmod);
    else
        mod = new cCompoundModule(modname, parentmod);

    // put the object members of the new module to their place, mod->members
    cObject *p;
    cIterator i(classmembers);
    while ((p=i())!=NULL)
    {
        i++;
        p->setOwner( &(mod->members) );
    }
    simulation.setLocalList( oldl );

    mod->setModuleType( this );

    simulation.addModule(mod);

    if (parentmod!=NULL)
    {
        mod->setOwner( parentmod );
    }
    else
    {
         mod->setOwner( &simulation );
         simulation.setSystemModule( mod );
    }

    // add parameters and gates to the new module, using module interface object
    cModuleInterface *iface = moduleInterface();
    iface->addParametersGatesTo( mod );
    return mod;
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
        throw new cException(eNOMODIF, interface_name, name());
    return iface;
}

//=========================================================================
//=== cLinkType - member functions

cLinkType::cLinkType(const char *name, cPar *(*d)(), cPar *(*e)(), cPar *(*dr)()) :
  cObject(name)
{
    delayfunc = d;
    errorfunc = e;
    dataratefunc = dr;
}

cLinkType::cLinkType(const cLinkType& li) : cObject()
{
    setName(li.name());
    operator=( li );
}

cLinkType& cLinkType::operator=(const cLinkType& li)
{
    if (this==&li) return *this;

    cObject::operator=( li );
    delayfunc = li.delayfunc;
    errorfunc = li.errorfunc;
    dataratefunc = li.dataratefunc;
    return *this;
}

cPar *cLinkType::createDelay() const
{
    return delayfunc==NULL ? NULL : delayfunc();
}

cPar *cLinkType::createError() const
{
    return errorfunc==NULL ? NULL : errorfunc();
}

cPar *cLinkType::createDataRate() const
{
    return dataratefunc==NULL ? NULL : dataratefunc();
}

//=========================================================================
//=== cFunctionType - all functions inline
cFunctionType::cFunctionType(const char *name, MathFuncNoArg f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 0;
    if (ac!=-1 && ac!=0)
        throw new cException("Register_Function() or cFunctionType: "
                             "attempt to register function \"%s\" with wrong "
                             "number of arguments %d, should be 0", name, ac);
}

cFunctionType::cFunctionType(const char *name, MathFunc1Arg f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 1;
    if (ac!=-1 && ac!=1)
        throw new cException("Register_Function() or cFunctionType: "
                             "attempt to register function \"%s\" with wrong "
                             "number of arguments %d, should be 1", name, ac);
}

cFunctionType::cFunctionType(const char *name, MathFunc2Args f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 2;
    if (ac!=-1 && ac!=2)
        throw new cException("Register_Function() or cFunctionType: "
                             "attempt to register function \"%s\" with wrong "
                             "number of arguments %d, should be 2", name, ac);
}

cFunctionType::cFunctionType(const char *name, MathFunc3Args f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 3;
    if (ac!=-1 && ac!=3)
        throw new cException("Register_Function() or cFunctionType: "
                             "attempt to register function \"%s\" with wrong "
                             "number of arguments %d, should be 3", name, ac);
}

cFunctionType::cFunctionType(const char *name, MathFunc4Args f, int ac) : cObject(name)
{
    this->f = (MathFunc)f;
    argc = 4;
    if (ac!=-1 && ac!=4)
        throw new cException("Register_Function() or cFunctionType: "
                             "attempt to register function \"%s\" with wrong "
                             "number of arguments %d, should be 4", name, ac);
}

MathFuncNoArg cFunctionType::mathFuncNoArg()  
{
    if (argc!=0)
        throw new cException(this,"mathFuncNoArg(): arg count mismatch (argc=%d)",argc);
    return (MathFuncNoArg)f;
}

MathFunc1Arg cFunctionType::mathFunc1Arg()
{
    if (argc!=1)
        throw new cException(this,"mathFunc1Arg(): arg count mismatch (argc=%d)",argc);
    return (MathFunc1Arg)f;
}

MathFunc2Args cFunctionType::mathFunc2Args()  
{
    if (argc!=2)
        throw new cException(this,"mathFunc2Args(): arg count mismatch (argc=%d)",argc);
    return (MathFunc2Args)f;
}

MathFunc3Args cFunctionType::mathFunc3Args()  
{
    if (argc!=3)
        throw new cException(this,"mathFunc3Args(): arg count mismatch (argc=%d)",argc);
    return (MathFunc3Args)f;
}

MathFunc4Args cFunctionType::mathFunc4Args()  
{
    if (argc!=4)
        throw new cException(this,"mathFunc4Args(): arg count mismatch (argc=%d)",argc);
    return (MathFunc4Args)f;
}

cFunctionType *findfunctionbyptr(MathFunc f)
{
    cIterator i(functions);
    while( !i.end() )
    {
        cFunctionType *ff = (cFunctionType *) i++;
        if (ff->mathFunc() == f)
            return ff;
    }
    return NULL;
}

//=========================================================================

cClassRegister::cClassRegister(const char *name, void *(*f)()) : cObject(name)
{
    creatorfunc = f;
}

void *createOne(const char *classname)
{
    cClassRegister *p = (cClassRegister *)classes.find( classname );
    if (!p)
        throw new cException("Registration object for class \"%s\" not found", classname);

    return p->createOne();
}

//=========================================================================
