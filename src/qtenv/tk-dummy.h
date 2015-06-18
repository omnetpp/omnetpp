//==========================================================================
//  TKDEFS.H - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  General defines for the Tkenv library
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_TKDUMMY_H
#define __OMNETPP_QTENV_TKDUMMY_H

#include <cstdlib>  // nullptr

struct Tcl_Interp {};
typedef void *ClientData;
struct Tcl_Namespace {};
struct Tcl_Obj {};
struct XErrorEvent {};
struct Tk_PhotoImageBlock {};
struct Tk_Window {};

#define TCL_OK          0
#define TCL_ERROR       1
#define TCL_RETURN      2
#define TCL_BREAK       3
#define TCL_CONTINUE    4

#define TCL_VOLATILE    1
#define TCL_STATIC      0
#define TCL_DYNAMIC     3

#define TCL_GLOBAL_ONLY      1
#define TCL_NAMESPACE_ONLY   2
#define TCL_APPEND_VALUE     4
#define TCL_LIST_ELEMENT     8
#define TCL_TRACE_READS      0x10
#define TCL_TRACE_WRITES     0x20
#define TCL_TRACE_UNSETS     0x40
#define TCL_TRACE_DESTROYED  0x80
#define TCL_INTERP_DESTROYED     0x100
#define TCL_LEAVE_ERR_MSG    0x200
#define TCL_TRACE_ARRAY      0x800
#define TCL_TRACE_RESULT_DYNAMIC 0x8000
#define TCL_TRACE_RESULT_OBJECT  0x10000

#define TK_ALL_EVENTS 0

typedef int Tcl_ObjCmdProc(ClientData clientData, Tcl_Interp *interp, int objc, struct Tcl_Obj * const * objv);
typedef int Tcl_CmdProc(ClientData clientData, Tcl_Interp *interp, int argc, const char *argv[]);
typedef void Tcl_CmdDeleteProc(ClientData clientData);


typedef struct Tcl_CmdInfo {
    int isNativeObjectProc;
    Tcl_ObjCmdProc *objProc;
    ClientData objClientData;
    Tcl_CmdProc *proc;
    ClientData clientData;
    Tcl_CmdDeleteProc *deleteProc;
    ClientData deleteData;
    Tcl_Namespace *namespacePtr;
} Tcl_CmdInfo;


inline int Tcl_Eval(Tcl_Interp *, const char *) {return 0;}
inline int Tcl_VarEval(Tcl_Interp *, const char *,...) {return 0;}
inline int Tcl_EvalFile(Tcl_Interp *, const char *) {return 0;}
inline int Tcl_SetResult(Tcl_Interp *, const char *, int) {return 0;}
inline const char *Tcl_GetStringResult(Tcl_Interp *) {return "";}

inline const char *Tcl_GetVar(Tcl_Interp *,...) {return "";}
inline const char *Tcl_GetVar2(Tcl_Interp *,...) {return "";}
inline int Tcl_SetVar(Tcl_Interp *,...) {return 0;}
inline int Tcl_SetVar2(Tcl_Interp *,...) {return 0;}
inline int Tcl_SetVar2Ex(Tcl_Interp *,...) {return 0;}


inline Tcl_Obj *Tcl_NewListObj(int, ...) {return nullptr;}
inline Tcl_Obj *Tcl_NewIntObj(int) {return nullptr;}
inline Tcl_Obj *Tcl_NewLongObj(long) {return nullptr;}
inline Tcl_Obj *Tcl_NewDoubleObj(double) {return nullptr;}
inline Tcl_Obj *Tcl_NewStringObj(const char *, int) {return nullptr;}
inline void Tcl_ListObjAppendElement(Tcl_Interp *, ...) {}
inline void Tcl_SetObjResult(Tcl_Interp *, ...) {}


inline Tcl_Interp *Tcl_CreateInterp() {return nullptr;}
inline int Tcl_Init(Tcl_Interp *) {return 0;}
inline int Tk_Init(Tcl_Interp *) {return 0;}
inline int Tcl_StaticPackage(Tcl_Interp *,...) {return 0;}
inline int Tcl_GetCommandInfo(Tcl_Interp *, ...) {return 0;}

inline void Tcl_CreateCommand(Tcl_Interp *, ...) {}
inline void Tk_DoOneEvent(int) {}
inline void Tcl_FindExecutable(const char *) {}

inline char *Tcl_Alloc(size_t size)  {return new char[size];}

#endif
