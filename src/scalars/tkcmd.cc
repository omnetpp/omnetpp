//==========================================================================
//  TKCMD.CC -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//#include <blt.h>
//#include <bltVector.h>

#include "tklib.h"
#include "tkutil.h"
#include "engine/scalarmanager.h"
#include "engine/filtering.h"



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
        for (ScalarManager::FileRef i = scalarMgr.getFiles().begin(); i!=scalarMgr.getFiles().end(); ++i)
        {
            const ScalarManager::File& file = i->second;
            for (int j=0; j<(int)file.runNumbers.size(); j++)
            {
                char buf[16];
                sprintf(buf,"%d",file.runNumbers[j]);
                std::string str = file.fileName + " #" + buf;
                Tcl_Obj *strobj = Tcl_NewStringObj(TCLCONST(str.c_str()),-1);
                Tcl_ListObjAppendElement(interp, vectorlist, strobj);
            }
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

    ScalarManager::IntVector vec;
    getFilteredScalarList2(scalarMgr, fileAndRunPattern, modulePattern, namePattern, vec);

    // set vector
    Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
    for (ScalarManager::IntVector::iterator i = vec.begin(); i!=vec.end(); ++i)
    {
        Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewIntObj(*i));
    }
    Tcl_SetObjResult(interp, vectorlist);

    return TCL_OK;
}

int getFilePathOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getFilePathOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).fileRef->second.filePath.c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getFileNameOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getFileNameOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).fileRef->second.fileName.c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getDirectoryOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getDirectoryOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetResult(interp, TCLCONST(scalarMgr.getValue(id).fileRef->second.directory.c_str()), TCL_VOLATILE);
    return TCL_OK;
}

int getRunNoOf_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getModuleOf <id>\"", TCL_STATIC); return TCL_ERROR;}
    int id = atoi(argv[1]);
    if (id<0 || id>=(int)scalarMgr.getValues().size()) {Tcl_SetResult(interp, "id out of range", TCL_STATIC); return TCL_ERROR;}
    Tcl_SetObjResult(interp, Tcl_NewIntObj(scalarMgr.getValue(id).runNumber));
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
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.fileRef->second.directory.c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("file",-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.fileRef->second.fileName.c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("run",3));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewIntObj(d.runNumber));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("module",7));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.moduleNameRef->c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("name",4));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj(TCLCONST(d.scalarNameRef->c_str()),-1));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewStringObj("value",5));
    Tcl_ListObjAppendElement(interp, vectorlist, Tcl_NewDoubleObj(d.value));
    Tcl_SetObjResult(interp, vectorlist);
    return TCL_OK;
}


// lots of helpers for groupByRunAndName_cmd and groupByModuleAndName_cmd follows
// FIXME should probably go somewhere else (into engine/?)

typedef std::vector<ScalarManager::IntVector> IntVectorVector;
typedef bool (*GroupingFunc)(const ScalarManager::Datum&, const ScalarManager::Datum&);
typedef bool (*CompareFunc)(int id1, int id2);

static bool sameGroupFileRunScalar(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2)
{
    return d1.fileRef==d2.fileRef && d1.runNumber==d2.runNumber && d1.scalarNameRef==d2.scalarNameRef;
}

static bool sameGroupModuleScalar(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2)
{
    return d1.moduleNameRef==d2.moduleNameRef && d1.scalarNameRef==d2.scalarNameRef;
}

static bool sameGroupFileRunModule(const ScalarManager::Datum& d1, const ScalarManager::Datum& d2)
{
    return d1.fileRef==d2.fileRef && d1.runNumber==d2.runNumber && d1.moduleNameRef==d2.moduleNameRef;
}

static bool lessByModuleRef(int id1, int id2)
{
    const ScalarManager::Datum& d1 = scalarMgr.getValue(id1);
    const ScalarManager::Datum& d2 = scalarMgr.getValue(id2);
    return *(d1.moduleNameRef) < *(d2.moduleNameRef);
}

static bool equalByModuleRef(int id1, int id2)
{
    const ScalarManager::Datum& d1 = scalarMgr.getValue(id1);
    const ScalarManager::Datum& d2 = scalarMgr.getValue(id2);
    return d1.moduleNameRef == d2.moduleNameRef;
}

static bool lessByFileAndRun(int id1, int id2)
{
    const ScalarManager::Datum& d1 = scalarMgr.getValue(id1);
    const ScalarManager::Datum& d2 = scalarMgr.getValue(id2);
    const std::string& fname1 = d1.fileRef->second.filePath;
    const std::string& fname2 = d2.fileRef->second.filePath;
    if (fname1==fname2)
        return d1.runNumber < d2.runNumber;
    else
        return fname1 < fname2;
}

static bool equalByFileAndRun(int id1, int id2)
{
    const ScalarManager::Datum& d1 = scalarMgr.getValue(id1);
    const ScalarManager::Datum& d2 = scalarMgr.getValue(id2);
    return d1.fileRef==d2.fileRef && d1.runNumber==d2.runNumber;
}

static bool lessByScalarNameRef(int id1, int id2)
{
    const ScalarManager::Datum& d1 = scalarMgr.getValue(id1);
    const ScalarManager::Datum& d2 = scalarMgr.getValue(id2);
    return *(d1.scalarNameRef) < *(d2.scalarNameRef);
}

static bool equalByScalarNameRef(int id1, int id2)
{
    const ScalarManager::Datum& d1 = scalarMgr.getValue(id1);
    const ScalarManager::Datum& d2 = scalarMgr.getValue(id2);
    return d1.scalarNameRef == d2.scalarNameRef;
}

static bool lessByValue(int id1, int id2)
{
    const ScalarManager::Datum& d1 = scalarMgr.getValue(id1);
    const ScalarManager::Datum& d2 = scalarMgr.getValue(id2);
    return d1.value < d2.value;
}

static void doGrouping(const char *idlist, GroupingFunc sameGroup, IntVectorVector& vv)
{
    // parse idlist and do grouping as well, on the fly
    const char *s = idlist;
    while (*s==' ') s++;
    while (*s)
    {
        // parse id
        int id = atoi(s);
        while (*s>='0' && *s<='9') s++;
        while (*s==' ') s++;

        // check of this id shares fileRef, runNumber & scalarName with one of the
        // IntVectors already in vv
        const ScalarManager::Datum& d = scalarMgr.getValue(id);
        IntVectorVector::iterator i;
        for (i=vv.begin(); i!=vv.end(); ++i)
        {
            int vvid = (*i)[0];  // first element in IntVector selected by i
            const ScalarManager::Datum& vvd = scalarMgr.getValue(vvid);
            if (sameGroup(d,vvd))
                break;
        }
        if (i==vv.end())
        {
            // not found -- new one has to be added
            vv.push_back(ScalarManager::IntVector());
            i = vv.end()-1;
        }

        // insert
        i->push_back(id);
    }
}

static void sortAndAlign(IntVectorVector& vv, CompareFunc less, CompareFunc equal)
{
    // order each group by module name
    for (IntVectorVector::iterator i=vv.begin(); i!=vv.end(); ++i)
        std::sort(i->begin(), i->end(), less);

    // now insert "null" elements (id=-1) so that every group is of same length,
    // and same indices are "equal()"
    for (int pos=0; ; pos++)
    {
        // determine "smallest" element in all vectors, on position "pos"
        int minId = -1;
        IntVectorVector::iterator i;
        for (i=vv.begin(); i!=vv.end(); ++i)
            if ((int)i->size()>pos)
                minId = (minId==-1) ? (*i)[pos] : less((*i)[pos],minId) ? (*i)[pos] : minId;

        // if pos is past the end of all vectors, we're done
        if (minId==-1)
            break;

        // if a vector has something different on this position, add a "null" element here
        for (i=vv.begin(); i!=vv.end(); ++i)
            if ((int)i->size()<=pos || !equal((*i)[pos],minId))
                i->insert(i->begin()+pos,-1);
    }
}

static Tcl_Obj *doConvertToTcl(Tcl_Interp *interp, const IntVectorVector& vv)
{
    // convert vv to Tcl list of lists
    Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
    for (IntVectorVector::const_iterator i=vv.begin(); i!=vv.end(); ++i)
    {
        Tcl_Obj *idlist = Tcl_NewListObj(0, NULL);
        const ScalarManager::IntVector& v = *i;
        for (ScalarManager::IntVector::const_iterator j = v.begin(); j!=v.end(); ++j)
            Tcl_ListObjAppendElement(interp, idlist, Tcl_NewIntObj(*j));
        Tcl_ListObjAppendElement(interp, vectorlist, idlist);
    }
    return vectorlist;
}

int groupByRunAndName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_groupByRunAndName <idlist>\"", TCL_STATIC); return TCL_ERROR;}

    // form groups (IntVectors) by fileRef+runNumber+scalarName
    IntVectorVector vv;
    doGrouping(argv[1],sameGroupFileRunScalar,vv);

    // order each group by module name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same moduleNameRefs
    sortAndAlign(vv,lessByModuleRef,equalByModuleRef);

    // convert result to Tcl list of lists
    Tcl_Obj *vvobj = doConvertToTcl(interp, vv);
    Tcl_SetObjResult(interp, vvobj);
    return TCL_OK;
}

int groupByModuleAndName_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_groupByModuleAndName <idlist>\"", TCL_STATIC); return TCL_ERROR;}

    // form groups (IntVectors) by moduleName+scalarName
    IntVectorVector vv;
    doGrouping(argv[1],sameGroupModuleScalar,vv);

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv,lessByFileAndRun,equalByFileAndRun);

    // convert result to Tcl list of lists
    Tcl_Obj *vvobj = doConvertToTcl(interp, vv);
    Tcl_SetObjResult(interp, vvobj);
    return TCL_OK;
}

int prepareScatterPlot_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_groupByModuleAndName <idlist> <modulename> <name>\"", TCL_STATIC); return TCL_ERROR;}

    // form groups (IntVectors) by moduleName+scalarName
    IntVectorVector vv;
    doGrouping(argv[1],sameGroupModuleScalar,vv);
    if (vv.size()==0) return TCL_OK;

    // order each group by fileRef+runNumber, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same fileRef+runNumber
    sortAndAlign(vv,lessByFileAndRun,equalByFileAndRun);

    // find series for X axis (modulename, scalarname)...
    const char *moduleName = argv[2];
    const char *scalarName = argv[3];
    int xpos = -1;
    for (IntVectorVector::iterator i=vv.begin(); i!=vv.end(); ++i)
    {
        int id = -1;
        for (ScalarManager::IntVector::iterator j=i->begin(); j!=i->end(); ++j)
            if (*j!=-1)
                {id = *j;break;}
        if (id==-1)
            continue;
        const ScalarManager::Datum& d = scalarMgr.getValue(id);
        if (*d.moduleNameRef==moduleName && *d.scalarNameRef==scalarName)
            {xpos = i-vv.begin();break;}
    }
    if (xpos==-1) {Tcl_SetResult(interp, "data for X axis not found", TCL_STATIC); return TCL_ERROR;}

    // ... and bring X series to 1st place
    if (xpos!=0)
        std::swap(vv[0], vv[xpos]);

    // sort x axis, moving elements in all other vectors as well.
    // Strategy: we'll construct the result in vv2. First we sort X axis, then
    // move elements of the other vectors to the same positions where the
    // X values went.

    // step one: sort X axis
    IntVectorVector vv2;
    vv2.resize(vv.size());
    vv2[0] = vv[0];
    std::sort(vv2[0].begin(), vv2[0].end(), lessByValue);

    // step two: remove id=-1 elements from the beginning of X series
    ScalarManager::IntVector::iterator firstvalue=vv2[0].begin();
    while (*firstvalue==-1 && firstvalue!=vv2[0].end()) ++firstvalue;
    vv2[0].erase(vv2[0].begin(),firstvalue);

    // step three: allocate all other vectors in vv2 to be the same length
    // (necessary because we'll fill them in via assignment, NOT push_back() or insert())
    for (int k=1; k<(int)vv2.size(); k++)
        vv2[k].resize(vv2[0].size());

    // step four: copy over elements
    for (int pos=0; pos<(int)vv[0].size(); pos++)
    {
        int id = vv[0][pos];
        if (id==-1) continue;
        int destpos = std::find(vv2[0].begin(),vv2[0].end(),id) - vv2[0].begin();
        for (int k=1; k<(int)vv.size(); k++)
            vv2[k][destpos] = vv[k][pos];
    }

    // convert result (vv2) to Tcl list of lists
    Tcl_Obj *vvobj = doConvertToTcl(interp, vv2);
    Tcl_SetObjResult(interp, vvobj);
    return TCL_OK;
}

int getModuleAndNamePairs_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getModuleAndNamePairs <idlist> <maxcount>\"", TCL_STATIC); return TCL_ERROR;}

    const char *idlist = argv[1];
    int maxcount = atoi(argv[2]);
    ScalarManager::IntVector vec;

    // parse idlist and pick ids that represent a new (module, name pair)
    const char *s = idlist;
    while (*s==' ') s++;
    while (*s)
    {
        // parse id
        int id = atoi(s);
        while (*s>='0' && *s<='9') s++;
        while (*s==' ') s++;

        // check if module and name of this id is already in vec[]
        const ScalarManager::Datum& d = scalarMgr.getValue(id);
        ScalarManager::IntVector::iterator i;
        for (i=vec.begin(); i!=vec.end(); ++i)
        {
            const ScalarManager::Datum& vd = scalarMgr.getValue(*i);
            if (d.moduleNameRef==vd.moduleNameRef && d.scalarNameRef==vd.scalarNameRef)
                break;
        }

        // not yet -- then add it
        if (i==vec.end())
        {
            vec.push_back(id);
            if ((int)vec.size()>=maxcount)
                break; // enough is enough
        }
    }

    // return vec[]
    Tcl_Obj *vobj = Tcl_NewListObj(0, NULL);
    for (ScalarManager::IntVector::iterator i = vec.begin(); i!=vec.end(); ++i)
        Tcl_ListObjAppendElement(interp, vobj, Tcl_NewIntObj(*i));
    Tcl_SetObjResult(interp, vobj);
    return TCL_OK;
}

int prepareCopyToClipboard_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
    if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_prepareCopyToClipboard <idlist>\"", TCL_STATIC); return TCL_ERROR;}

    // form groups (IntVectors) by fileRef+runNumber+moduleNameRef
    IntVectorVector vv;
    doGrouping(argv[1],sameGroupFileRunModule,vv);

    // order each group by scalar name, and insert "null" elements (id=-1) so that
    // every group is of same length, and same indices contain same scalarNameRefs
    sortAndAlign(vv,lessByScalarNameRef,equalByScalarNameRef);

    // convert result to Tcl list of lists
    Tcl_Obj *vvobj = doConvertToTcl(interp, vv);
    Tcl_SetObjResult(interp, vvobj);
    return TCL_OK;
}

