//==========================================================================
//  PARSENED.CC - part of
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


#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <tcl.h>

#include "parsened.h"
#include "nedfile.h"
#include "tklib.h"  // for CHK()

//--------

NEDParser::NEDParser()
{
    tmpbuf = 0;
    tmpbuflen = 0;
}

NEDParser::~NEDParser()
{
    delete tmpbuf;
}

int NEDParser::parseFile(Tcl_Interp *intrp, const char *fname, int nedfilekey,
                         const char *nedarray, const char *errorsarray)
{
    // init class members
    interp = intrp;
    nedfile_key = nedfilekey;
    tmp_ned = nedarray;
    tmp_errors = errorsarray;
    num_errors = 0;

    NEDFile nf;
    nedsource = &nf;

    // cosmetics on file name: substitute "~"
    char newfilename[1000];
    if (fname[0]=='~') {
        sprintf(newfilename,"%s%s",getenv("HOME"),fname+1);
    } else {
        strcpy(newfilename,fname);
    }

    // load whole file into memory
    if (!nedsource->readFile(newfilename))
        {interp->result = "unable to load file";return TCL_ERROR;}

    // get file comment
    set(nedfile_key, "banner-comment", nedsource->getFileComment());

    // init and call parser
    yyout = stdout;
    yyin = fopen(newfilename,"r");
    if (!yyin)
        {interp->result = "unable to open file";return TCL_ERROR;}
    runparse();
    fclose(yyin);

    // return value: number of errors
    sprintf(interp->result, "%d", num_errors);

    return TCL_OK;
}

int NEDParser::parseText(Tcl_Interp *intrp, const char *nedtext, int nedfilekey,
                         const char *nedarray, const char *errorsarray)
{
    // init global vars
    interp = intrp;
    nedfile_key = nedfilekey;
    tmp_ned = nedarray;
    tmp_errors = errorsarray;
    num_errors = 0;
    NEDFile nf;
    nedsource = &nf;

    // load whole file into memory
    if (!nedsource->setData(nedtext))
        {interp->result = "unable to allocate work memory";return TCL_ERROR;}

    // get file comment
    set(nedfile_key, "banner-comment", nedsource->getFileComment());

    // init and call parser
    yyin = NULL;
    yyout = stdout;
    struct yy_buffer_state *handle = yy_scan_string(nedtext);
    if (!handle)
        {interp->result = "unable to allocate work memory";return TCL_ERROR;}
    runparse();
    yy_delete_buffer(handle);

    // return value: number of errors
    sprintf(interp->result, "%d", num_errors);

    return TCL_OK;
}

const char *NEDParser::storeInTempBuf(const char *s)
{
    int l = strlen(s)+1;
    if (l>=tmpbuflen)
    {
        delete tmpbuf;
        tmpbuf = new char [l];
        tmpbuflen=l;
    }
    strcpy(tmpbuf,s);
    return tmpbuf;
}

int NEDParser::create(char *type, int parentkey)
{
    char pkey[32];
    sprintf(pkey,"%d", parentkey);
    CHK(Tcl_VarEval(interp, "NedParser_createNedElement ", tmp_ned, " ",
                        type, " ", pkey, NULL));
    int keyv = atoi(interp->result);
    // should free the result...?
    return keyv;
}

void NEDParser::set(int key, char *attr, long value)
{
    char tmp[64], tmp2[64];
    sprintf(tmp, "%d,%s",key,attr);
    sprintf(tmp2,"%ld",value);
    Tcl_SetVar2(interp,TCLCONST(tmp_ned),tmp,tmp2,TCL_GLOBAL_ONLY);
}

void NEDParser::set(int key, char *attr, char *value)
{
    char tmp[64];
    sprintf(tmp,"%d,%s",key,attr);
    Tcl_SetVar2(interp,TCLCONST(tmp_ned),tmp,value,TCL_GLOBAL_ONLY);
}

void NEDParser::set(int key, char *attr, YYLTYPE valuepos)
{
    set(key,attr,nedsource->get(valuepos));
}

const char *NEDParser::get(int key, char *attr)
{
    char tmp[64];
    sprintf(tmp,"%d,%s",key,attr);
    return Tcl_GetVar2(interp,TCLCONST(tmp_ned),tmp,TCL_GLOBAL_ONLY);
}

void NEDParser::swap(int key, char *attr1, char *attr2)
{
    char tmp1[64], tmp2[64];
    sprintf(tmp1, "%d,%s",key,attr1);
    sprintf(tmp2, "%d,%s",key,attr2);

    const char *oldv1 = Tcl_GetVar2(interp,TCLCONST(tmp_ned),tmp1,TCL_GLOBAL_ONLY);
    const char *oldv2 = Tcl_GetVar2(interp,TCLCONST(tmp_ned),tmp2,TCL_GLOBAL_ONLY);
    assert(oldv1!=0 && oldv2!=0);

    oldv1 = storeInTempBuf(oldv1);
    Tcl_SetVar2(interp,TCLCONST(tmp_ned),tmp1,TCLCONST(oldv2),TCL_GLOBAL_ONLY);
    Tcl_SetVar2(interp,TCLCONST(tmp_ned),tmp2,TCLCONST(oldv1),TCL_GLOBAL_ONLY);
}

//-----------

int NEDParser::findChild(int parentkey, char *attr, char *value)
{
    char pkey[32];
    sprintf(pkey,"%d", parentkey);
    CHK(Tcl_VarEval(interp, "NedParser_findChild ", tmp_ned, " ",
                         pkey, " ", attr, " ", value, NULL));
    int ret;
    if (!interp->result[0])
        ret = -1; // not found
    else if (interp->result[0] == 'n')  // result is "not unique"
        ret = -2; // not unique
    else
        ret = atoi(interp->result);
    // should free the result...?

    return ret;
}

//-----------

static void _setVar(Tcl_Interp *interp, const char *array, int key, char *attr, char *value)
{
    char tmp[64];
    sprintf(tmp,"%d,%s",key,attr);
    Tcl_SetVar2(interp,TCLCONST(array),tmp,value,TCL_GLOBAL_ONLY);
}

void NEDParser::error(char type, char *msg, int line, int col)
{
    //printf("DBG: %c %s, at %d:%d\n", type, msg, line, col);

    char buf[32];
    _setVar(interp, tmp_errors, num_errors, "type",
                const_cast<char*>(type=='E' ? "Error" : type=='W' ? "Warning" : "?" ));
    sprintf(buf,"%d", line);
    _setVar(interp, tmp_errors, num_errors, "line",  buf);
    sprintf(buf,"%d", col);
    _setVar(interp, tmp_errors, num_errors, "column", buf);
    _setVar(interp, tmp_errors, num_errors, "text", msg);

    num_errors++;
}

void NEDParser::dbg(YYLTYPE lc, char *what)
{
    printf("%s: %d,%d,%d,%d=%s.\n",
            what,
                  lc.first_line,lc.first_column,lc.last_line,lc.last_column,
                  nedsource->get(lc));
}

//-----------------------------------------------------------------------

int parseNedFile(Tcl_Interp *interp, const char *fname, int nedfilekey,
                 const char *nedarray, const char *errorsarray)
{
    NEDParser _np;
    np = &_np;

    return np->parseFile(interp, fname, nedfilekey, nedarray, errorsarray);
}

int parseNedText(Tcl_Interp *interp, const char *nedtext, int nedfilekey,
                 const char *nedarray, const char *errorsarray)
{
    NEDParser _np;
    np = &_np;

    return np->parseText(interp, nedtext, nedfilekey, nedarray, errorsarray);
}

