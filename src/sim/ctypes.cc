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
//    cInspectorFactory: inspector creation
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>          // strlen
#include <stdarg.h>          // va_list
#include <iostream.h>        // ostream
#include "cpar.h"
#include "cmodule.h"
#include "macros.h"
#include "ctypes.h"

//=== Functions  (register them for findFunction())
Define_Function( acos,  1 )
Define_Function( asin,  1 )
Define_Function( atan,  1 )
Define_Function( atan2, 1 )

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
  cObject(name, &modinterfaces)
{
    gatev = NULL;
    paramv = NULL;
    machinev = NULL;

    setup( descr_tab );

    // if no interface has been registered with this name, register ourselves
    if (modinterfaces.find(name)!=NULL)
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
  cObject( NULL, &modtypes),
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
    delete[] gatev;
    delete[] paramv;
    delete[] machinev;
}

cModuleInterface& cModuleInterface::operator=(const cModuleInterface&)
{
    opp_error("cModuleInterface cannot copy itself!");
    return *this;
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
        opp_error(eNOMEM);
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
           opp_error("Module declaration doesn't allow type `%c'"
                            " for parameter `%s'",
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
    opp_error("Inconsistent module interfaces (Interface..End) for `%s':"
                     " %s does not match",
                     name(), what);
    return;

    error2:
    opp_error("Inconsistent module interfaces (Interface..End) for `%s':"
                     " %s `%s' (#%d) do not match",
                     name(), what, which, id);
    return;
}

//=========================================================================
//=== cModuleType - member functions

cModuleType::cModuleType(const char *classname,
                         const char *interf_name,
                         ModuleCreateFunc cf) :
  cObject(classname, &modtypes)
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
    interface = NULL;
}


cModuleType::cModuleType(const cModuleType& mi) :
  cObject( NULL, &modtypes)
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
    interface = mt.interface;
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
    while ((p=i())!=NULL)  {i++; p->setOwner( &(mod->members) );}
    simulation.setLocalList( oldl );

    mod->setModuleType( this );

    simulation.add( mod );

    if (parentmod!=NULL)
    {
         mod->setOwner( parentmod );
    }
    else
    {
         mod->setOwner( &simulation );
         simulation.setSystemModule( mod );
    }

    // find the module interface
    if (!interface)
        interface = findModuleInterface( interface_name );
    if (!interface)
        {opp_error(eNOMODIF, interface_name, name()); return mod;}

    // add parameters and gates to the new module
    interface->addParametersGatesTo( mod );
    return mod;
}

void cModuleType::buildInside(cModule *mod)
{
    cModule *oldcontext = simulation.contextModule();
    simulation.setContextModule( mod );

    if (!interface)
        interface = findModuleInterface( interface_name );
    if (!interface)
        {opp_error(eNOMODIF, interface_name, name()); return;}

    interface->checkParametersOf( mod );
    if (!simulation.ok()) return;

    mod->buildInside();

    if (oldcontext)
        simulation.setContextModule( oldcontext );
    else
        simulation.setGlobalContext();
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

//=========================================================================
//=== cLinkType - member functions

cLinkType::cLinkType(const char *name, cPar *(*d)(), cPar *(*e)(), cPar *(*dr)()) :
  cObject(name, &linktypes)
{
    delayfunc = d;
    errorfunc = e;
    dataratefunc = dr;
}

cLinkType::cLinkType(const cLinkType& li) :
  cObject( NULL, &linktypes)
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
    return delayfunc==NULL ? NO(cPar) : delayfunc();
}

cPar *cLinkType::createError() const
{
    return errorfunc==NULL ? NO(cPar) : errorfunc();
}

cPar *cLinkType::createDataRate() const
{
    return dataratefunc==NULL ? NO(cPar) : dataratefunc();
}

//=========================================================================
//=== cNetworkType - all functions inline

//=========================================================================
//=== cFunctionType - all functions inline

cFunctionType *findfunctionbyptr(MathFunc f)
{
    cIterator i(functions);
    while( !i.end() )
    {
        cFunctionType *ff = (cFunctionType *) i++;
        if (ff->f == f)
            return ff;
    }
    return NO(cFunctionType);
}

//=========================================================================

//=== creates an object of type passed in a string
//    e.g: cObject *param = createOne( "cPar" );
//    used when objects are restored from network data packet or file
cObject *createOne(const char *classname)
{
    cClassRegister *p = (cClassRegister *)classes.find( classname );
    if (p!=NULL)
        return p->createOne();
    opp_error("Registration object for class \"%s\" not found", classname);
    return NULL;
}

//=========================================================================
//=== cInspectorFactory - all functions inline

cInspectorFactory::cInspectorFactory(const char *name, TInspector *(*f)(cObject *,int,void *)) :
  cObject(name,(cObject *)&inspectorfactories)
{
    inspFactoryFunc = f;
}

TInspector *cInspectorFactory::createInspectorFor(cObject *object,int type,void *data)
{
    if (!inspFactoryFunc)
    {
        opp_error("(%s)%s: factory function not set",className(),fullName());
        return 0;
    }
    return (*inspFactoryFunc)(object,type,data);
}
