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
  Copyright (C) 1992,99 Andras Varga
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

//-------------------------------------------------------------------------
// *** NEW ***
ostream& operator<<(ostream& os, char *s)
{
    return os << (const char *)s;
}

//=========================================================================
//=== cModuleInterface - member functions

cModuleInterface::cModuleInterface( char *namestr, sDescrItem *descr_tab ) :
  cObject(namestr, &modinterfaces)
{
     gatev = NULL;
     paramv = NULL;
     machinev = NULL;

     setup( descr_tab );

     // if no interface has been registered with this name, register ourselves
     if (modinterfaces.find(namestr)!=NULL)
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
            case 'G':  // GATE( namestr, Input/Output )
               s = opp_strdup( tab->namestr );
               v = s && (l=strlen(s))>=3 && s[l-2]=='[' && s[l-1]==']';
               if (v)  s[ strlen(s)-2 ]='\x0';  // cut off '[]'
               gatev[g].namestr = s;
               gatev[g].type = tab->type;
               gatev[g].vect = v;
               g++;
               break;

            case 'P':  // PARAM( namestr, types )
               paramv[p].namestr = opp_strdup( tab->namestr );
               paramv[p].types = opp_strdup( tab->types );
               p++;
               break;

            case 'M':  // MACHINE( namestr )
               machinev[m].namestr = opp_strdup( tab->namestr );
               m++;
               break;
         }
     }
     if (m==0) machinev[m].namestr = opp_strdup( "default" ); // add "default"
}

cModuleInterface::cModuleInterface( cModuleInterface& mi ) :
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
     for (i=0; i<ngate; i++)    {delete gatev[i].namestr;}
     for (i=0; i<nparam; i++)   {delete paramv[i].namestr;delete paramv[i].types;}
     for (i=0; i<nmachine; i++) {delete machinev[i].namestr;}
     delete[] gatev;
     delete[] paramv;
     delete[] machinev;
}

cModuleInterface& cModuleInterface::operator=(cModuleInterface&)
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
        module->addMachinePar( machinev[i].namestr );
     for (i=0;i<ngate;i++)
        module->addGate( gatev[i].namestr, gatev[i].type );
     for (i=0;i<nparam;i++)
     {
        module->addPar( paramv[i].namestr );
        if (opp_strcmp(paramv[i].types,"S")==0)
        {
           module->par( paramv[i].namestr ) = "";
           module->par( paramv[i].namestr ).setInput(TRUE);
        }
     }
}

void cModuleInterface::checkParametersOf( cModule *mod )
{
     // check parameter types and convert CONST parameters to constant
     for (int i=0;i<nparam;i++)
     {
         cPar *par = &(mod->par( paramv[i].namestr ));

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
        if (opp_strcmp(machinev[i].namestr,m->machinev[i].namestr)!=0)
           {what="names for machine par";id=i;which=machinev[i].namestr;goto error2;}
     }
     for (i=0;i<nparam;i++)
     {
        if (opp_strcmp(paramv[i].namestr,m->paramv[i].namestr)!=0)
           {what="names for parameter";id=i;which=paramv[i].namestr;goto error2;}
        if (paramv[i].types!=NULL || m->paramv[i].types!=NULL)
           if ((paramv[i].types==NULL && m->paramv[i].types!=NULL) ||
               (paramv[i].types!=NULL && m->paramv[i].types==NULL) ||
               strspn(paramv[i].types,m->paramv[i].types)!=strlen(paramv[i].types)||
               strspn(m->paramv[i].types,paramv[i].types)!=strlen(m->paramv[i].types)
              )
               {what="allowed types for parameter";id=i;which=paramv[i].namestr;goto error2;}
     }
     for (i=0;i<ngate;i++)
     {
        if (opp_strcmp(gatev[i].namestr,m->gatev[i].namestr)!=0)
           {what="names for gate #";id=i;which=gatev[i].namestr;goto error2;}
        if (gatev[i].type!=m->gatev[i].type)
           {what="direction (in/out) for gate";id=i;which=gatev[i].namestr;goto error2;}
        if (gatev[i].vect!=m->gatev[i].vect)
           {what="vector/scalar type for gate";id=i;which=gatev[i].namestr;goto error2;}
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

cModuleType::cModuleType( char *classname, char *interf_name, ModuleCreateFunc cf):
  cObject(classname, &modtypes)
{
     // create_func:
     //   Ptr to a small function that creates a module of type classname.
     //   E.g, if classname is "Processor", create_func points to a function
     //   like this:
     //      static cModule *Processor__create(char *namestr, cModule *parentmod )
     //      {
     //           return (cModule *) new Processor(namestr, parentmod);
     //      }
     //  For each module type, such a function is automatically created by
     //  the Define_Module( classname ) macro.

     create_func = cf;

     // We cannot find() the interface object (and store its pointer) yet,
     // because it might not have been created yet.
     interface_name = opp_strdup(interf_name);
     interface = NULL;
}


cModuleType::cModuleType( cModuleType& mi ) :
  cObject( NULL, &modtypes)
{
    setName(mi.name());
    operator=( mi );
}

cModuleType::~cModuleType()
{
    delete interface_name;
}

cModuleType& cModuleType::operator=(cModuleType& mt)
{
    create_func = mt.create_func;
    interface_name = opp_strdup(mt.interface_name);
    interface = mt.interface;
    return *this;
}

cModule *cModuleType::create(char *namestr, cModule *parentmod, bool local)
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
        mod = create_func(namestr, parentmod);
    else
        mod = new cCompoundModule(namestr, parentmod);

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

cModule *cModuleType::createAndInit(char *namestr, cModule *parentmod)
{
    // this is a convenience function...
    cModule *mod = create(namestr, parentmod);
    mod->buildInside();
    mod->callInitialize();
    return mod;
}

//=========================================================================
//=== cLinkType - member functions

cLinkType::cLinkType(char *namestr, cPar *(*d)(), cPar *(*e)(), cPar *(*dr)()) :
cObject(namestr, &linktypes)
{
    delayfunc = d;
    errorfunc = e;
    dataratefunc = dr;
}

cLinkType::cLinkType( cLinkType& li ) :
cObject( NULL, &linktypes)
{
    setName(li.name());
    operator=( li );
}

cLinkType& cLinkType::operator=(cLinkType& li)
{
    cObject::operator=( li );
    delayfunc = li.delayfunc;
    errorfunc = li.errorfunc;
    dataratefunc = li.dataratefunc;
    return *this;
}

cPar *cLinkType::createDelay()
{
    return delayfunc==NULL ? NO(cPar) : delayfunc();
}

cPar *cLinkType::createError()
{
    return errorfunc==NULL ? NO(cPar) : errorfunc();
}

cPar *cLinkType::createDataRate()
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
cObject *createOne(char *classname)
{
    cClassRegister *p = (cClassRegister *)classes.find( classname );
    if (p!=NULL)
        return p->createOne();
    opp_error("Registration object for class \"%s\" not found", classname);
    return NULL;
}

//=========================================================================
//=== cInspectorFactory - all functions inline

cInspectorFactory::cInspectorFactory(char *name, TInspector *(*f)(cObject *,int,void *)) :
  cObject(name,(cObject *)&inspectorfactories)
{
    inspFactoryFunc = f;
}

TInspector *cInspectorFactory::createInspectorFor(cObject *object,int type,void *data)
{
    if (!inspFactoryFunc)
    {
        opp_error("(%s)%s: factory function not set",isA(),fullName());
        return 0;
    }
    return (*inspFactoryFunc)(object,type,data);
}
