//==========================================================================
//   INSPECT.CC -
//            part of the Tcl/Tk environment of
//                             OMNeT++
//
//  Implementation of inspector() functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdarg.h>
#include "omnetpp.h"
#include "tkapp.h"
#include "tklib.h"
#include "tkinsp.h"

// force the linker to include this file
void _dummy_for_inspectors() {}

Register_InspectorFactory( cObjectIFC,         createCObjectInspector);
Register_InspectorFactory( cHeadIFC,           createCHeadInspector);
Register_InspectorFactory( cQueueIFC,          createCQueueInspector);
Register_InspectorFactory( cBagIFC,            createCBagInspector);
Register_InspectorFactory( cArrayIFC,          createCArrayInspector);
Register_InspectorFactory( cLinkedListIFC,     createCLinkedListInspector);
Register_InspectorFactory( cParIFC,            createCParInspector);
Register_InspectorFactory( cModuleParIFC,      createCModuleParInspector);
Register_InspectorFactory( cGateIFC,           createCGateInspector);
Register_InspectorFactory( cMessageIFC,        createCMessageInspector);
Register_InspectorFactory( cPacketIFC,         createCPacketInspector);
Register_InspectorFactory( cModuleIFC,         createCModuleInspector);
Register_InspectorFactory( cSimpleModuleIFC,   createCSimpleModuleInspector);
Register_InspectorFactory( cCompoundModuleIFC, createCCompoundModuleInspector);
Register_InspectorFactory( cSimulationIFC,     createCSimulationInspector);
Register_InspectorFactory( cMessageHeapIFC,    createCMessageHeapInspector);
Register_InspectorFactory( cNetModIFC,         createCNetModInspector);
Register_InspectorFactory( cOutVectorIFC,      createCOutVectorInspector);
Register_InspectorFactory( cStatisticIFC,      createCStatisticInspector);
Register_InspectorFactory( cDensityEstBaseIFC, createCDensityEstBaseInspector);
Register_InspectorFactory( cTopologyIFC,       createCTopologyInspector);
Register_InspectorFactory( cWatchIFC,          createCWatchInspector);
Register_InspectorFactory( cFSMIFC,            createCFSMInspector);


TInspector *createCObjectInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,INSP_CONTAINER,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     case INSP_CONTAINER: return new TContainerInspector( object, INSP_CONTAINER, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCHeadInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_CONTAINER,INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     case INSP_DEFAULT:
     case INSP_CONTAINER: return new TContainerInspector( object, INSP_CONTAINER, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCBagInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCLinkedListInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCArrayInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_CONTAINER,INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     case INSP_DEFAULT:
     case INSP_CONTAINER: return new TContainerInspector( object, INSP_CONTAINER, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCQueueInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_CONTAINER,INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     case INSP_DEFAULT:
     case INSP_CONTAINER: return new TContainerInspector( object, INSP_CONTAINER, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCMessageHeapInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_CONTAINER,INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     case INSP_DEFAULT:
     case INSP_CONTAINER: return new TContainerInspector( object, INSP_CONTAINER, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCWatchInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TWatchInspector( object, INSP_OBJECT, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCParInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TParInspector( object, INSP_OBJECT, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCModuleParInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TParInspector( object, INSP_OBJECT, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCMessageInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,INSP_CONTAINER,INSP_PARAMETERS,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TMessageInspector( object, INSP_OBJECT, data );
     case INSP_CONTAINER: return new TContainerInspector( object, INSP_CONTAINER, data );
     case INSP_PARAMETERS:return ((cPacket *)object)->parList().inspector(INSP_DEFAULT, NULL);
     default:             return NO(TInspector);
   }
}

TInspector *createCSimulationInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_CONTAINER,INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     case INSP_DEFAULT:
     case INSP_CONTAINER: return new TContainerInspector( object, INSP_CONTAINER, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCGateInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_GRAPHICAL,INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_GRAPHICAL: return new TGraphicalGateWindow( object, INSP_GRAPHICAL, data );
     case INSP_OBJECT:    return new TGateInspector( object, INSP_OBJECT, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCModuleInspector(cObject *, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, -1);
                          return NO(TInspector);
     default:             return NO(TInspector);
   }
}

TInspector *createCSimpleModuleInspector(cObject *object, int type, void *data)
{
   cSimpleModule *mod = (cSimpleModule *)object;
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_MODULEOUTPUT, INSP_OBJECT,
                                         INSP_PARAMETERS,INSP_GATES,
                                         INSP_LOCALVARS,INSP_CLASSMEMBERS,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TSimpleModInspector( object, INSP_OBJECT, data );
     case INSP_MODULEOUTPUT: return new TModuleWindow( object, INSP_MODULEOUTPUT, data);
     case INSP_GATES:     return mod->gatev.inspector(INSP_DEFAULT, NULL);
     case INSP_PARAMETERS:return mod->paramv.inspector(INSP_DEFAULT, NULL);
     case INSP_LOCALVARS: return mod->locals.inspector(INSP_DEFAULT, NULL);
     case INSP_CLASSMEMBERS:return mod->members.inspector(INSP_DEFAULT, NULL);
     default:             return NO(TInspector);
   }
}

TInspector *createCCompoundModuleInspector(cObject *object, int type, void *data)
{
   cCompoundModule *mod = (cCompoundModule *)object;
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_GRAPHICAL,INSP_OBJECT,INSP_MODULEOUTPUT,
                                         INSP_PARAMETERS,INSP_GATES,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_GRAPHICAL: return new TGraphicalModWindow( object, INSP_GRAPHICAL, data );
     case INSP_OBJECT:    return new TCompoundModInspector( object, INSP_OBJECT, data );
     case INSP_MODULEOUTPUT: return new TModuleWindow( object, INSP_MODULEOUTPUT, data);
     case INSP_GATES:     return mod->gatev.inspector(INSP_DEFAULT, NULL);
     case INSP_PARAMETERS:return mod->paramv.inspector(INSP_DEFAULT, NULL);
     default:             return NO(TInspector);
   }
}

TInspector *createCNetModInspector(cObject *object, int type, void *data)
{
   cNetMod *mod = (cNetMod *)object;
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,INSP_CONTAINER,INSP_GATES,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     case INSP_GATES:     return mod->gatev.inspector(INSP_DEFAULT, NULL);
     default:             return NO(TInspector);
   }
}

TInspector *createCOutVectorInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_GRAPHICAL,INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     case INSP_DEFAULT:
     case INSP_GRAPHICAL: return new TOutVectorWindow( object, INSP_GRAPHICAL, data, 2048 );
     default:             return NO(TInspector);
   }
}

TInspector *createCStatisticInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TStatisticInspector( object, INSP_OBJECT, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCDensityEstBaseInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_GRAPHICAL,INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_OBJECT:    return new TStatisticInspector( object, INSP_OBJECT, data );
     case INSP_DEFAULT:
     case INSP_GRAPHICAL: return new THistogramWindow( object, INSP_GRAPHICAL, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCTopologyInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TObjInspector( object, INSP_OBJECT, data );
     default:             return NO(TInspector);
   }
}

TInspector *createCFSMInspector(cObject *object, int type, void *data)
{
   return createCObjectInspector(object,type,data);
}

TInspector *createCPacketInspector(cObject *object, int type, void *data)
{
   switch (type) {
     case INSP_GETTYPES:  setTypes(data, INSP_OBJECT,INSP_CONTAINER,INSP_PARAMETERS,-1);
                          return NO(TInspector);
     case INSP_DEFAULT:
     case INSP_OBJECT:    return new TPacketInspector( object, INSP_OBJECT, data );
     case INSP_CONTAINER: return new TContainerInspector( object, INSP_CONTAINER, data );
     case INSP_PARAMETERS:return ((cPacket *)object)->parList().inspector(INSP_DEFAULT, NULL);
     default:             return NO(TInspector);
   }
}
