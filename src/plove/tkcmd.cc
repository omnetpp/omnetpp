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

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tklib.h"
#include <bltVector.h>

#include "engine/node.h"
#include "engine/nodetype.h"
#include "engine/nodetyperegistry.h"
#include "engine/dataflowmanager.h"
#include "engine/arraybuilder.h"
#include "engine/compoundfilter.h"
#include "engine/getvectors.h"


#ifndef _WIN32
#include <sys/stat.h>  // mknod()
#endif


#define CATCH_EXCEPTIONS(statement) \
   try { \
       statement; \
   } catch (Exception *e) { \
       Tcl_SetResult(interp, const_cast<char *>(e->message()), TCL_VOLATILE); \
       return TCL_ERROR; \
   }

#define TCLCONST(x)  const_cast<char *>(x)

//----------------------------------------------------------------
// Command functions:
int getVectorList_cmd(ClientData, Tcl_Interp *, int, const char **);

int createNetwork_cmd(ClientData, Tcl_Interp *, int, const char **);
int deleteNetwork_cmd(ClientData, Tcl_Interp *, int, const char **);
int executeNetwork_cmd(ClientData, Tcl_Interp *, int, const char **);

int getNodeTypes_cmd(ClientData, Tcl_Interp *, int, const char **);
int nodeType_cmd(ClientData, Tcl_Interp *, int, const char **);
int createNode_cmd(ClientData, Tcl_Interp *, int, const char **);
int getPort_cmd(ClientData, Tcl_Interp *, int, const char **);
int connect_cmd(ClientData, Tcl_Interp *, int, const char **);

int arraybuilder_cmd(ClientData, Tcl_Interp *, int, const char **);
int makeNamedPipe_cmd(ClientData, Tcl_Interp *, int, const char **);
int checkmemory_cmd(ClientData, Tcl_Interp *, int, const char **);

int compoundFilterTypeCreate_cmd(ClientData, Tcl_Interp *, int, const char **);
int compoundFilterType_cmd(ClientData, Tcl_Interp *, int, const char **);


// command table
OmnetTclCommand tcl_commands[] = {
   // Commands invoked from the menu
   { "opp_getvectorlist",    getVectorList_cmd  }, // args: <filename>

   { "opp_createnetwork",    createNetwork_cmd  }, // args: -
   { "opp_deletenetwork",    deleteNetwork_cmd  }, // args: <network>
   { "opp_executenetwork",   executeNetwork_cmd }, // args: <network>

   { "opp_getnodetypes",     getNodeTypes_cmd   }, // args: <nodetype> ?category?
   { "opp_nodetype",         nodeType_cmd       }, // args: <nodetype> category|description|attrs
   { "opp_createnode",       createNode_cmd     }, // args: <network> <nodetype> ?args?
   { "opp_getport",          getPort_cmd        }, // args: <node> <portname>
   { "opp_connect",          connect_cmd        }, // args: <node1> <portname1> <node2> <portname2>

   { "opp_arraybuilder",     arraybuilder_cmd   }, // args: <arraybuildernode> <command> ...
   { "opp_makenamedpipe",    makeNamedPipe_cmd  }, // args: <name>
   { "opp_checkmemory",      checkmemory_cmd    }, // args: <numpoints>

   { "opp_compoundfiltertype_create", compoundFilterTypeCreate_cmd}, // args: <nodetype>
   { "opp_compoundfiltertype", compoundFilterType_cmd }, // args: <nodetype> <command> ...

   // end of list
   { NULL, },
};

int getVectorList_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getvectorlist fileName\"", TCL_STATIC); return TCL_ERROR;}
   const char *fname = argv[1];
   OutVectorArray outvectors;
   CATCH_EXCEPTIONS(
       getVectors(fname, outvectors);
   );

   Tcl_Obj *vectorlist = Tcl_NewListObj(0, NULL);
   for (OutVectorArray::iterator it = outvectors.begin(); it!=outvectors.end(); ++it)
   {
       Tcl_Obj *vecv[3];
       vecv[0] = Tcl_NewIntObj(it->vectorId);
       const char *s1 = it->moduleName.c_str();
       vecv[1] = Tcl_NewStringObj(TCLCONST(s1), strlen(s1));
       const char *s2 = it->vectorName.c_str();
       vecv[2] = Tcl_NewStringObj(TCLCONST(s2), strlen(s2));
       Tcl_Obj *vec = Tcl_NewListObj(3, vecv);
       Tcl_ListObjAppendElement(interp, vectorlist, vec);
   }
   Tcl_SetObjResult(interp, vectorlist);
   return TCL_OK;
}

int getNodeTypes_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=1 && (argc!=2 || strcmp(argv[1],"-all")))
       {Tcl_SetResult(interp, "wrong # args: should be \"opp_getnodetypes ?-all?\"", TCL_STATIC); return TCL_ERROR;}

   bool all = (argc==2);

   Tcl_Obj *resultlist = Tcl_NewListObj(0, NULL);
   NodeTypeVector v;
   NodeTypeRegistry::instance()->getNodeTypes(v);
   for (NodeTypeVector::iterator it=v.begin(); it!=v.end(); ++it)
   {
       if (all || !(*it)->isHidden())
       {
           const char *s = (*it)->name();
           Tcl_Obj *obj = Tcl_NewStringObj(TCLCONST(s), strlen(s));
           Tcl_ListObjAppendElement(interp, resultlist, obj);
       }
   }
   Tcl_SetObjResult(interp, resultlist);
   return TCL_OK;
}

int nodeType_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_nodetype nodeType exists|category|description|hidden|attrs\"", TCL_STATIC); return TCL_ERROR;}
   const char *nodetypename = argv[1];
   const char *field = argv[2];

   if (!strcmp(field,"exists"))
   {
       bool exists = NodeTypeRegistry::instance()->exists(nodetypename);
       Tcl_SetResult(interp, (exists?"1":"0"), TCL_STATIC);
       return TCL_OK;
   }

   NodeType *nodetype;
   CATCH_EXCEPTIONS(
       nodetype = NodeTypeRegistry::instance()->getNodeType(nodetypename);
   );

   if (!strcmp(field,"category"))
   {
       Tcl_SetResult(interp, TCLCONST(nodetype->category()), TCL_VOLATILE);
   }
   else if (!strcmp(field,"description"))
   {
       Tcl_SetResult(interp, TCLCONST(nodetype->description()), TCL_VOLATILE);
   }
   else if (!strcmp(field,"hidden"))
   {
       Tcl_SetResult(interp, (nodetype->isHidden()?"1":"0"), TCL_STATIC);
   }
   else if (!strcmp(field,"attrs"))
   {
       StringMap attrs;
       StringMap defaults;
       nodetype->getAttributes(attrs);
       nodetype->getAttrDefaults(defaults);

       Tcl_Obj *resultlist = Tcl_NewListObj(0, NULL);
       for (StringMap::iterator i=attrs.begin(); i!=attrs.end(); ++i)
       {
           Tcl_Obj *vecv[3];
           vecv[0] = Tcl_NewStringObj(TCLCONST(i->first.c_str()), -1);
           vecv[1] = Tcl_NewStringObj(TCLCONST(i->second.c_str()), -1);
           vecv[2] = Tcl_NewStringObj(TCLCONST(defaults[i->first].c_str()), -1);
           Tcl_Obj *vec = Tcl_NewListObj(3, vecv);
           Tcl_ListObjAppendElement(interp, resultlist, vec);
       }
       Tcl_SetObjResult(interp, resultlist);
   }
   else
   {
       Tcl_SetResult(interp, "2nd arg should be \"category\" or \"description\" or \"attrs\"", TCL_STATIC);
       return TCL_ERROR;
   }
   return TCL_OK;
}

int createNetwork_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<1) {Tcl_SetResult(interp, "wrong # args: should be \"opp_createnetwork\"", TCL_STATIC); return TCL_ERROR;}
   Tcl_SetResult(interp, ptrToStr(new DataflowManager()), TCL_VOLATILE);
   return TCL_OK;
}

int deleteNetwork_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_deletenetwork network\"", TCL_STATIC); return TCL_ERROR;}
   DataflowManager *mgr = (DataflowManager *)strToPtr( argv[1] );
   if (!mgr) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   delete mgr;
   return TCL_OK;
}

int executeNetwork_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_executenetwork network\"", TCL_STATIC); return TCL_ERROR;}
   DataflowManager *mgr = (DataflowManager *)strToPtr( argv[1] );
   if (!mgr) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   CATCH_EXCEPTIONS(
       mgr->execute();
   )
   return TCL_OK;
}

int createNode_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_createnode network nodetype ?args?\"", TCL_STATIC); return TCL_ERROR;}
   if (argc&1!=1) {Tcl_SetResult(interp, "odd number of args expected", TCL_STATIC); return TCL_ERROR;}

   DataflowManager *mgr = (DataflowManager *)strToPtr( argv[1] );
   if (!mgr) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   const char *nodetypename = argv[2];

   CATCH_EXCEPTIONS(
       NodeType *nodetype = NodeTypeRegistry::instance()->getNodeType(nodetypename);
       StringMap attrs;
       for (int i=3; i<argc; i+=2)
       {
           if (argv[i][0]!='-') {Tcl_SetResult(interp, "invalid arg", TCL_STATIC); return TCL_ERROR;}
           attrs[argv[i]+1] = argv[i+1];
       }
       Node *node = nodetype->create(mgr,attrs);
       mgr->addNode(node);
       Tcl_SetResult(interp, ptrToStr(node), TCL_VOLATILE);
   )
   return TCL_OK;
}

int getPort_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_getport node portname\"", TCL_STATIC); return TCL_ERROR;}
   Node *node = (Node *)strToPtr( argv[1] );
   if (!node) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
   const char *portname = argv[2];

   CATCH_EXCEPTIONS(
       NodeType *nodetype = node->nodeType();
       Port *port = nodetype->getPort(node,portname);
       Tcl_SetResult(interp, ptrToStr(port), TCL_VOLATILE);
   )
   return TCL_OK;
}

int connect_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=5) {Tcl_SetResult(interp, "wrong # args: should be \"opp_connect node1 portname1 node2 portname2\" or ", TCL_STATIC); return TCL_ERROR;}

   CATCH_EXCEPTIONS(
       Node *node1 = (Node *)strToPtr( argv[1] );
       if (!node1) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
       const char *portname1 = argv[2];
       NodeType *nodetype1 = node1->nodeType();
       Port *port1 = nodetype1->getPort(node1,portname1);

       Node *node2 = (Node *)strToPtr( argv[3] );
       if (!node2) {Tcl_SetResult(interp, "null or malformed pointer", TCL_STATIC); return TCL_ERROR;}
       const char *portname2 = argv[4];
       NodeType *nodetype2 = node2->nodeType();
       Port *port2 = nodetype2->getPort(node2,portname2);

       if (node1->dataflowManager()!=node2->dataflowManager())  {Tcl_SetResult(interp, "nodes belong to different networks", TCL_STATIC); return TCL_ERROR;}
       DataflowManager *mgr = node1->dataflowManager();
       mgr->connect(port1,port2);
   )
   return TCL_OK;
}

// helper: Tcl_FreeProc needed for arraybuilder_cmd()
static void deleteDoubleArray(char *p)
{
    double *array = (double *)p;
    delete [] array;
}

int arraybuilder_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_arraybuilder <arraybuilderNode> <command> ...\"", TCL_STATIC); return TCL_ERROR;}

   ArrayBuilderNode *node = dynamic_cast<ArrayBuilderNode *>((Node *)strToPtr(argv[1]));
   if (!node) {Tcl_SetResult(interp, "null or malformed pointer, or not an arraybuilder node", TCL_STATIC); return TCL_ERROR;}
   const char *cmd = argv[2];

   if (!strcmp(cmd,"getvectors"))
   {
       if (argc!=5) {Tcl_SetResult(interp, "wrong # args: should be \"opp_arraybuilder <arraybuilderNode> getvectors <xvec> <yvec>\"", TCL_STATIC); return TCL_ERROR;}
       const char *bltvectornameX = argv[3];
       const char *bltvectornameY = argv[4];
       double *x; double *y;
       size_t length;
       node->extractVector(x, y, length);
       CATCH_EXCEPTIONS(
           Blt_Vector *vecX;
           Blt_Vector *vecY;
           Blt_CreateVector(interp, const_cast<char*>(bltvectornameX), 1, &vecX);
           Blt_CreateVector(interp, const_cast<char*>(bltvectornameY), 1, &vecY);
           Blt_ResetVector(vecX, x, length, length, deleteDoubleArray);
           Blt_ResetVector(vecY, y, length, length, deleteDoubleArray);
       )
       return TCL_OK;
   }
   else if (!strcmp(cmd,"length"))
   {
       Tcl_SetObjResult(interp, Tcl_NewIntObj(node->length()));
       return TCL_OK;
   }
   else if (!strcmp(cmd,"sort"))
   {
       node->sort();
       return TCL_OK;
   }
   else
   {
       Tcl_SetResult(interp, "2nd arg should be: getvectors, length or sort", TCL_STATIC);
       return TCL_ERROR;
   }
}

int makeNamedPipe_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
#ifdef _WIN32
   Tcl_SetResult(interp, "not supported on Windows", TCL_STATIC);
   return TCL_ERROR;
#else
   if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_makenamedpipe pathname\"", TCL_STATIC); return TCL_ERROR;}
   const char *fname = argv[1];
   if (mknod(fname, S_IFIFO, 0))
   {
       Tcl_SetResult(interp, "cannot create named pipe", TCL_STATIC);
       return TCL_ERROR;
   }
   return TCL_OK;
#endif
}


int checkmemory_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_checkmemory <numpoints>\"", TCL_STATIC); return TCL_ERROR;}
   int numpoints = atoi(argv[1]);
   // BLT seems to allocate a Point2D array (2x double) plus an int array for every "line".
   // Plus it temporarily might need the same amount of memory.
   // Hints: bltGrLine.c, look for "screenPts" and "indices".
   // Empirical measurements suggest BLT might need about 40 bytes per point, but
   // process size varies a lot (also on graph settings: lines yes/no, symbols yes/no etc)
   // so this is not a very reliable estimate. However, better than nothing.
   size_t pointsize = 2*sizeof(double)+sizeof(int);
   size_t memrequired = (size_t)(numpoints*pointsize*2);
   try {
       char *tmp = new char[memrequired];
       delete tmp;
   } catch (...) {
       Tcl_SetResult(interp, "BLT might not have enough memory for the plot", TCL_STATIC);
       return TCL_ERROR;
   }
   return TCL_OK;
}

int compoundFilterTypeCreate_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc!=2) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype_create <nodetype>\"", TCL_STATIC); return TCL_ERROR;}
   const char *nodetypename = argv[1];
   DBG(("creating type %s\n", nodetypename));
   CompoundFilterType *nodetype = new CompoundFilterType();
   nodetype->setName(nodetypename);
   NodeTypeRegistry::instance()->add(nodetype);
   return TCL_OK;
}

int compoundFilterType_cmd(ClientData, Tcl_Interp *interp, int argc, const char **argv)
{
   if (argc<3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> <command> ...\"", TCL_STATIC); return TCL_ERROR;}
   const char *nodetypename = argv[1];
   const char *cmd = argv[2];

   CompoundFilterType *nodetype;
   CATCH_EXCEPTIONS(
       NodeType *p = NodeTypeRegistry::instance()->getNodeType(nodetypename);
       nodetype = dynamic_cast<CompoundFilterType *>(p);
       if (!nodetype) {Tcl_SetResult(interp, "object is not a cascadedfilter", TCL_STATIC); return TCL_ERROR;}
   );

   if (!strcmp(cmd,"clone"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> clone <newnodetype>\"", TCL_STATIC); return TCL_ERROR;}
       const char *s = argv[3];
       DBG(("cloning type %s to %s\n", nodetype->name(), s));
       CompoundFilterType *clone = new CompoundFilterType();
       *clone = *nodetype;
       clone->setName(s);
       NodeTypeRegistry::instance()->add(clone);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"rename"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> rename <name>\"", TCL_STATIC); return TCL_ERROR;}
       const char *s = argv[3];
       NodeTypeRegistry::instance()->remove(nodetype);
       DBG(("renaming type %s to %s\n", nodetype->name(), s));
       nodetype->setName(s);
       NodeTypeRegistry::instance()->add(nodetype);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"delete"))
   {
       if (argc!=3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> delete\"", TCL_STATIC); return TCL_ERROR;}
       DBG(("deleting type %s\n", nodetype->name()));
       NodeTypeRegistry::instance()->remove(nodetype);
       delete nodetype;
       return TCL_OK;
   }
   else if (!strcmp(cmd,"equals"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> equals <nodetype2>\"", TCL_STATIC); return TCL_ERROR;}
       const char *nodetypename2 = argv[3];
       CompoundFilterType *nodetype2;
       CATCH_EXCEPTIONS(
           NodeType *p2 = NodeTypeRegistry::instance()->getNodeType(nodetypename2);
           nodetype2 = dynamic_cast<CompoundFilterType *>(p2);
       );
       if (!nodetype2)
       {
           Tcl_SetResult(interp, "0", TCL_STATIC);
           return TCL_OK;
       }
       bool equal = nodetype->equals(*nodetype2);
       DBG(("type %s%s%s\n", nodetype->name(), (equal?"==":"!="), nodetype2->name()));
       Tcl_SetResult(interp, (equal?"1":"0"), TCL_STATIC);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"setDescription"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> setDescription <desc>\"", TCL_STATIC); return TCL_ERROR;}
       const char *s = argv[3];
       nodetype->setDescription(s);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"setHidden"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> setHidden <0|1>\"", TCL_STATIC); return TCL_ERROR;}
       bool hidden = (argv[3][0]!='0');
       nodetype->setHidden(hidden);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"setAttr"))
   {
       if (argc!=6) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> setAddr <name> <desc> <default>\"", TCL_STATIC); return TCL_ERROR;}
       const char *s = argv[3];
       const char *d = argv[4];
       const char *v = argv[5];
       nodetype->setAttr(s,d,v);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"removeAttr"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> removeAddr <name>\"", TCL_STATIC); return TCL_ERROR;}
       const char *s = argv[3];
       nodetype->removeAttr(s);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"numSubfilters"))
   {
       if (argc!=3) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> numSubfilters\"", TCL_STATIC); return TCL_ERROR;}
       Tcl_Obj *res = Tcl_NewIntObj(nodetype->numSubfilters());
       Tcl_SetObjResult(interp, res);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"subfilterType"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> subfilterType <k>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>=nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter index out of range", TCL_STATIC); return TCL_ERROR;}
       Tcl_SetResult(interp, TCLCONST(nodetype->subfilter(k).nodeType()), TCL_VOLATILE);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"subfilterComment"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> subfilterComment <k>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>=nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter index out of range", TCL_STATIC); return TCL_ERROR;}
       Tcl_SetResult(interp, TCLCONST(nodetype->subfilter(k).comment()), TCL_VOLATILE);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"setSubfilter"))
   {
       if (argc!=6) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> setSubfilter <k> <type> <comment>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>=nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter index out of range", TCL_STATIC); return TCL_ERROR;}
       const char *t = argv[4];
       const char *c = argv[5];
       nodetype->subfilter(k).setNodeType(t);
       nodetype->subfilter(k).setComment(c);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"removeSubfilter"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> removeSubfilter <k>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>=nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter index out of range", TCL_STATIC); return TCL_ERROR;}
       nodetype->removeSubfilter(k);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"insertSubfilter"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> insertSubfilter <k>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter insertion index out of range", TCL_STATIC); return TCL_ERROR;}
       // insert blank
       nodetype->insertSubfilter(k, CompoundFilterType::Subfilter());
       return TCL_OK;
   }
   else if (!strcmp(cmd,"subfilterAttrs"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> subfilterAttrs <k>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>=nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter index out of range", TCL_STATIC); return TCL_ERROR;}
       Tcl_Obj *resultlist = Tcl_NewListObj(0, NULL);
       StringMap& attrs = nodetype->subfilter(k).attrAssignments();
       for (StringMap::iterator i=attrs.begin(); i!=attrs.end(); ++i)
       {
           Tcl_Obj *vecv[2];
           vecv[0] = Tcl_NewStringObj(TCLCONST(i->first.c_str()), -1);
           vecv[1] = Tcl_NewStringObj(TCLCONST(i->second.c_str()), -1);
           Tcl_Obj *vec = Tcl_NewListObj(2, vecv);
           Tcl_ListObjAppendElement(interp, resultlist, vec);
       }
       Tcl_SetObjResult(interp, resultlist);
       return TCL_OK;
   }
   else if (!strcmp(cmd,"setSubfilterAttr"))
   {
       if (argc!=6) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> setSubfilterAttr <k> <name> <value>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>=nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter index out of range", TCL_STATIC); return TCL_ERROR;}
       const char *n = argv[4];
       const char *v = argv[5];
       StringMap& attrs = nodetype->subfilter(k).attrAssignments();
       attrs[n] = v;
       return TCL_OK;
   }
   else if (!strcmp(cmd,"removeSubfilterAttr"))
   {
       if (argc!=5) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> removeSubfilterAttr <k> <name>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>=nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter index out of range", TCL_STATIC); return TCL_ERROR;}
       const char *n = argv[4];
       StringMap& attrs = nodetype->subfilter(k).attrAssignments();
       attrs.erase(std::string(n));
       return TCL_OK;
   }
   else if (!strcmp(cmd,"validateSubfilter"))
   {
       if (argc!=4) {Tcl_SetResult(interp, "wrong # args: should be \"opp_compoundfiltertype <nodetype> validateSubfilter <k>\"", TCL_STATIC); return TCL_ERROR;}
       int k = atoi(argv[3]);
       if (k<0 || k>=nodetype->numSubfilters()) {Tcl_SetResult(interp, "subfilter index out of range", TCL_STATIC); return TCL_ERROR;}

       CATCH_EXCEPTIONS(
           const char *subfiltnodetypename = nodetype->subfilter(k).nodeType();
           NodeType *subfiltnodetype = NodeTypeRegistry::instance()->getNodeType(subfiltnodetypename);
           StringMap& attrs = nodetype->subfilter(k).attrAssignments();
           subfiltnodetype->validateAttrValues(attrs); // will throw exception if not OK
       );
       return TCL_OK;
   }
   else
   {
       Tcl_SetResult(interp, "2nd arg should be: setName, setDescription, setHidden, setAttr, "
                             "removeAttr, numSubfilters, subfilterType, subfilterComment, "
                             "setSubfilter, removeSubfilter, insertSubfilter, subfilterAttrs, "
                             "setSubfilterAttr, removeSubfilterAttr or validateSubfilter",
                             TCL_STATIC);
       return TCL_ERROR;
   }
   return TCL_OK;
}

