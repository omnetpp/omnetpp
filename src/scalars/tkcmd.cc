//==========================================================================
//  TKCMD.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "tklib.h"
#include "tkutil.h"
#include "engine/scalarmanager.h"
#include "engine/util.h"
#include "engine/datasorter.h"



#define CATCH_EXCEPTIONS(statement) \
   try { \
       statement; \
   } catch (TException *e) { \
       Tcl_SetResult(interp, const_cast<char *>(e->message()), TCL_VOLATILE); \
       return TCL_ERROR; \
   }


/**
 * "The" ScalarManager instance.
 */
ScalarManager scalarMgr;


//----------------------------------------------------------------
// Command functions:
int loadScalar_cmd(ClientData, Tcl_Interp *, int, const char **);
int getFileAndRunList_cmd(ClientData, Tcl_Interp *, int, const char **);
int getModuleList_cmd(ClientData, Tcl_Interp *, int, const char **);
int getScalarNameList_cmd(ClientData, Tcl_Interp *, int, const char **);
int getFilteredScalarList_cmd(ClientData, Tcl_Interp *, int, const char **);
int getFilePathOf_cmd(ClientData, Tcl_Interp *, int, const char **);
int getFileNameOf_cmd(ClientData, Tcl_Interp *, int, const char **);
int getDirectoryOf_cmd(ClientData, Tcl_Interp *, int, const char **);
int getRunNoOf_cmd(ClientData, Tcl_Interp *, int, const char **);
int getModuleOf_cmd(ClientData, Tcl_Interp *, int, const char **);
int getNameOf_cmd(ClientData, Tcl_Interp *, int, const char **);
int getValueOf_cmd(ClientData, Tcl_Interp *, int, const char **);
int getListboxLine_cmd(ClientData, Tcl_Interp *, int, const char **);
int groupByRunAndName_cmd(ClientData, Tcl_Interp *, int, const char **);
int groupByModuleAndName_cmd(ClientData, Tcl_Interp *, int, const char **);
int prepareScatterPlot_cmd(ClientData, Tcl_Interp *, int, const char **);
int getModuleAndNamePairs_cmd(ClientData, Tcl_Interp *, int, const char **);
int prepareCopyToClipboard_cmd(ClientData, Tcl_Interp *, int, const char **);


// command table
OmnetTclCommand tcl_commands[] = {
   // basic
   { "opp_loadScalar",            loadScalar_cmd },           // opp_loadScalar $fname
   { "opp_getFileAndRunList",     getFileAndRunList_cmd },    // opp_getFileAndRunList
   { "opp_getModuleList",         getModuleList_cmd },        // opp_getModuleList
   { "opp_getScalarNameList",     getScalarNameList_cmd },    // opp_getScalarNameList
   { "opp_getFilteredScalarList", getFilteredScalarList_cmd },// opp_getFilteredScalarList $fileAndRun $module $name
   // components of a scalar
   { "opp_getFilePathOf",         getFilePathOf_cmd },        // opp_getFilePathOf $id
   { "opp_getFileNameOf",         getFileNameOf_cmd },        // opp_getFileNameOf $id
   { "opp_getDirectoryOf",        getDirectoryOf_cmd },       // opp_getDirectoryOf $id
   { "opp_getRunNoOf",            getRunNoOf_cmd },           // opp_getRunNoOf $id
   { "opp_getModuleOf",           getModuleOf_cmd },          // opp_getModuleOf $id
   { "opp_getNameOf",             getNameOf_cmd },            // opp_getNameOf $id
   { "opp_getValueOf",            getValueOf_cmd },           // opp_getNameOf $id
   // filling the listbox
   { "opp_getListboxLine",        getListboxLine_cmd },       // opp_getListboxLine $id
   // charts
   { "opp_groupByRunAndName",     groupByRunAndName_cmd },    // opp_groupByRunAndName $idlist
   { "opp_groupByModuleAndName",  groupByModuleAndName_cmd }, // opp_groupByModuleAndName $idlist
   { "opp_prepareScatterPlot",    prepareScatterPlot_cmd },   // opp_prepareScatterPlot $idlist $modulename $scalarname
   { "opp_getModuleAndNamePairs", getModuleAndNamePairs_cmd },// opp_getModuleAndNamePairs $idlist $maxcount
   { "opp_prepareCopyToClipboard",prepareCopyToClipboard_cmd},// opp_prepareCopyToClipboard $idlist
   // end of list
   { NULL, },
};


int loadScalar_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_loadScalar <filename>\"", TCL_STATIC); return TCL_ERROR;}
    CATCH_EXCEPTIONS(
        scalarMgr.loadFile(argv[1]);
    )
    return TCL_OK;
}

int getFileAndRunList_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=1) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getFileAndRunList\"", TCL_STATIC); return TCL_ERROR;}
    CATCH_EXCEPTIONS(
        Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
        for (ScalarManager::RunRef i = scalarMgr.getRuns().begin(); i!=scalarMgr.getRuns().end(); ++i)
        {
            Tcl_Obj *strobj = Tcl_NewStringObj(TCLCONST(i->fileAndRunName.c_str()),-1);
            Tcl_ListObjAppendElement(interp, vectorlist, strobj);
        }
        Tcl_SetObjResult(interp, vectorlist);
    )
    return TCL_OK;
}

int getModuleList_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=1) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getModuleList\"", TCL_STATIC); return TCL_ERROR;}

    Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
    for (ScalarManager::StringRef i = scalarMgr.getModuleNames().begin(); i!=scalarMgr.getModuleNames().end(); ++i)
    {
        Tcl_Obj *str = Tcl_NewStringObj(TCLCONST(i->c_str()),-1);
        Tcl_ListObjAppendElement(interp, vectorlist, str);
    }
    Tcl_SetObjResult(interp, vectorlist);

    return TCL_OK;
}

int getScalarNameList_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=1) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getScalarNameList\"", TCL_STATIC); return TCL_ERROR;}

    Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
    for (ScalarManager::StringRef i = scalarMgr.getScalarNames().begin(); i!=scalarMgr.getScalarNames().end(); ++i)
    {
        Tcl_Obj *str = Tcl_NewStringObj(TCLCONST(i->c_str()),-1);
        Tcl_ListObjAppendElement(interp, vectorlist, str);
    }
    Tcl_SetObjResult(interp, vectorlist);
    return TCL_OK;
}

int getFilteredScalarList_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getFilteredScalarList <fileAndRun> <module> <name>\"", TCL_STATIC); return TCL_ERROR;}

    const char *fileAndRunPattern = argv[1];
    const char *modulePattern = argv[2];
    const char *namePattern = argv[3];

    DataSorter sorter(&scalarMgr);
    IntVector vec = sorter.getFilteredScalarList(fileAndRunPattern, modulePattern, namePattern);

    // set vector
    Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
    for (IntVector::iterator i = vec.begin(); i!=vec.end(); ++i)
        Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewIntObj(*i));
    Tcl_SetObjResult(interp, vectorlist);

    return TCL_OK;
}

int getFilePathOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getFilePathOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).runRef->fileRef->filePath.c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getFileNameOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getFileNameOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).runRef->fileRef->fileName.c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getDirectoryOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getDirectoryOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).runRef->fileRef->directory.c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getRunNoOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getRunNoOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).runRef->runName.c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getModuleOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getModuleOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).moduleNameRef->c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getNameOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getNameOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).scalarNameRef->c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getValueOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getValueOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetObjResult(interp, Tcl_NewDoubleObj(scalarMgr.getValue(id).value));
    return TCL_OK;
}

int getListboxLine_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getValueOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}

    const ScalarManager::Datum& d = scalarMgr.getValue(id);

    Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("dir",-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.runRef->fileRef->directory.c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("file",-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.runRef->fileRef->fileName.c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("run",3));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.runRef->runName.c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("module",7));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.moduleNameRef->c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("name",4));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.scalarNameRef->c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("value",5));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewDoubleObj(d.value));
    Tcl_SetObjResult(interp, vectorlist);
    return TCL_OK;
}

static IntVector parseIdList(const char *idlist)
{
    IntVector v;
    char *s = const_cast<char *>(idlist); // strtol 2nd arg is without const
    while (*s==' ') s++;
    while (*s)
    {
        int id = strtol(s,&s,10);
        while (*s==' ') s++;

        v.push_back(id);
    }
    return v;
}

static Tcl_Obj *doConvertToTcl(Tcl_Interp *interp, const IntVectorVector& vv)
{
    // convert vv to Tcl list of lists
    Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
    for (IntVectorVector::const_iterator i=vv.begin(); i!=vv.end(); ++i)
    {
        Tcl_Obj *idlist = Tcl_NewListObj(0, NULL);
        const IntVector& v = *i;
        for (IntVector::const_iterator j = v.begin(); j!=v.end(); ++j)
            Tcl_ListObjAppendElement(interp, idlist, Tcl_NewIntObj(*j));
        Tcl_ListObjAppendElement(interp, vectorlist, idlist);
    }
    return vectorlist;
}

int groupByRunAndName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_groupByRunAndName <idlist>\"", TCL_STATIC); return TCL_ERROR;}
    const char *idlist = argv[1];

    // do it
    DataSorter sorter(&scalarMgr);
    IntVectorVector vv = sorter.groupByRunAndName(parseIdList(idlist));

    // convert result to Tcl list of lists
    Tcl_Obj *vvobj = doConvertToTcl(interp, vv);
    Tcl_SetObjResult(interp, vvobj);
    return TCL_OK;
}

int groupByModuleAndName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_groupByModuleAndName <idlist>\"", TCL_STATIC); return TCL_ERROR;}
    const char *idlist = argv[1];

    // do it
    DataSorter sorter(&scalarMgr);
    IntVectorVector vv = sorter.groupByModuleAndName(parseIdList(idlist));

    // convert result to Tcl list of lists
    Tcl_Obj *vvobj = doConvertToTcl(interp, vv);
    Tcl_SetObjResult(interp, vvobj);
    return TCL_OK;
}

int prepareScatterPlot_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_prepareScatterPlot <idlist> <modulename> <name>\"", TCL_STATIC); return TCL_ERROR;}
    const char *idlist = argv[1];
    const char *moduleName = argv[2];
    const char *scalarName = argv[3];

    // do it
    DataSorter sorter(&scalarMgr);
    IntVectorVector vv = sorter.prepareScatterPlot(parseIdList(idlist), moduleName, scalarName);

    // convert result to Tcl list of lists
    Tcl_Obj *vvobj = doConvertToTcl(interp, vv);
    Tcl_SetObjResult(interp, vvobj);
    return TCL_OK;
}

int getModuleAndNamePairs_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getModuleAndNamePairs <idlist> <maxcount>\"", TCL_STATIC); return TCL_ERROR;}
    const char *idlist = argv[1];
    int maxcount = atoi(argv[2]);

    // do it
    DataSorter sorter(&scalarMgr);
    IntVector vec = sorter.getModuleAndNamePairs(parseIdList(idlist), maxcount);

    // return vec[]
    Tcl_Obj *vobj = Tcl_NewListObj(0, NULL);
    for (IntVector::iterator i = vec.begin(); i!=vec.end(); ++i)
        Tcl_ListObjAppendElement(interp, vobj, Tcl_NewIntObj(*i));
    Tcl_SetObjResult(interp, vobj);
    return TCL_OK;
}

int prepareCopyToClipboard_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_prepareCopyToClipboard <idlist>\"", TCL_STATIC); return TCL_ERROR;}
    const char *idlist = argv[1];

    // do it
    DataSorter sorter(&scalarMgr);
    IntVectorVector vv = sorter.prepareCopyToClipboard(parseIdList(idlist));

    // convert result to Tcl list of lists
    Tcl_Obj *vvobj = doConvertToTcl(interp, vv);
    Tcl_SetObjResult(interp, vvobj);
    return TCL_OK;
}

