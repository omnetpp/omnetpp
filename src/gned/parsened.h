//==========================================================================
//  PARSENED.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __PARSENED_H
#define __PARSENED_H

#include <stdio.h>
#include <tcl.h>

#include "ebnf.h"

class NEDFile;
class NEDParser;

extern FILE *yyin;
extern FILE *yyout;

struct yy_buffer_state;

struct yy_buffer_state *yy_scan_string(const char *str);
void yy_delete_buffer(struct yy_buffer_state *);

int runparse (void);

extern NEDParser *np;


/* to EXPR_TYPE: */
#define TYPE_NUMERIC   'N'
#define TYPE_CONST_NUM 'C'
#define TYPE_STRING    'S'
#define TYPE_BOOL      'B'
#define TYPE_XML       'X'
#define TYPE_ANYTYPE   'A'

//
// the parser interface
//
int parseNedFile(Tcl_Interp *interp, const char *fname, int nedfilekey,
                 const char *nedarray, const char *errorsarray);
int parseNedText(Tcl_Interp *interp, const char *nedtext, int nedfilekey,
                 const char *nedarray, const char *errorsarray);

//
// parser class
//
class NEDParser
{
  public:
    // key variables
    int freekey;

    int nedfile_key;
    int imports_key;
    int import_key;
    int channel_key;
    int chanattr_key;
    int network_key;
    int module_key;  // also plays the role of simple_key
    int params_key;
    int param_key;
    int gates_key;
    int gate_key;
    int machines_key;
    int machine_key;
    int submod_key;
    int submods_key;
    int substparams_key;
    int substparam_key;
    int gatesizes_key;
    int gatesize_key;
    int substmachines_key;
    int substmachine_key;
    int conns_key;
    int conn_key;
    int connattr_key;
    int forloop_key;
    int loopvar_key;

    // module position
    int curx,cury;

    // TCL Interpreter
    Tcl_Interp *interp;

    // Global NED File object
    NEDFile *nedsource;

    // TCL array names; to be changed to TCL object references:
    const char *tmp_ned;
    const char *tmp_errors;
    int num_errors;

    char *tmpbuf;
    int tmpbuflen;

    const char *storeInTempBuf(const char *s);

  public:
    NEDParser();
    ~NEDParser();

    // entry points:
    int parseFile(Tcl_Interp *interp, const char *fname, int nedfilekey,
                  const char *nedarray, const char *errorsarray);
    int parseText(Tcl_Interp *interp, const char *nedtext, int nedfilekey,
                  const char *nedarray, const char *errorsarray);

    // create new item: returns item key
    int create(char *type, int parentkey);

    // setting, getting and swapping tmp_ned array elements
    void set(int key, char *attr, long value);
    void set(int key, char *attr, char *value);
    void set(int key, char *attr, YYLTYPE valuepos);
    const char *get(int key, char *attr);
    void swap(int key, char *attr1, char *attr2);

    // findChild: returns item key, -1 not found, -2 not unique
    int findChild(int parentkey, char *attr, char *value);

    // error and debug handling
    void error(char type, char *msg, int line, int col);
    void dbg(YYLTYPE lc, char *what);

};


#endif

