//==========================================================================
//   PARSENED.CC -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "nedparser.h"
#include "nedfilebuffer.h"
#include "nedelements.h"
#include "nederror.h"


//--------

NEDParser::NEDParser()
{
    tree = 0;
    nedsource = 0;
}

NEDParser::~NEDParser()
{
    delete tree;
}

bool NEDParser::parseFile(const char *fname, bool parseexpr)
{
    // init class members
    NEDFileBuffer nf;
    nedsource = &nf;
    num_errors = 0;
    filename = fname;

    // cosmetics on file name: substitute "~"
    char newfilename[1000];
    if (fname[0]=='~') {
        sprintf(newfilename,"%s%s",getenv("HOME"),fname+1);
    } else {
        strcpy(newfilename,fname);
    }

    // load whole file into memory
    if (!nedsource->readFile(newfilename))
        {NEDError(NULL, "cannot read %s", fname); return false;}

    // get file comment
    NedFileNode *nedfile = new NedFileNode();
    nedfile->setBannerComment(nedsource->getFileComment());
    nedfile->setFilename(fname);
    tree = nedfile;

    // init and call parser
    yyout = stdout;
    yyin = fopen(newfilename,"r");
    if (!yyin)
        {NEDError(NULL, "cannot read %s", fname); return false;}
    int perr = runparse(this,nedfile,parseexpr,fname);
    fclose(yyin);

    // num_errors contains number of parse errors
    return true;
}

bool NEDParser::parseText(const char *nedtext,bool parseexpr)
{
    // init global vars
    NEDFileBuffer nf;
    nedsource = &nf;
    num_errors = 0;
    filename = "buffer";

    // load whole file into memory
    if (!nedsource->setData(nedtext))
        {NEDError(NULL, "unable to allocate work memory"); return false;}

    // get file comment
    NedFileNode *nedfile = new NedFileNode();
    nedfile->setBannerComment(nedsource->getFileComment());
    tree = nedfile;

    // init and call parser
    yyin = NULL;
    yyout = stdout;
    struct yy_buffer_state *handle = yy_scan_string(nedtext);
    if (!handle)
        {NEDError(NULL, "unable to allocate work memory"); return false;}
    int perr = runparse(this,nedfile,parseexpr,"generated-code");
    yy_delete_buffer(handle);

    // num_errors contains number of parse errors
    return true;
}


NEDElement *NEDParser::getTree()
{
    NEDElement *ret = tree;
    tree = 0;
    return ret;
}


void NEDParser::error(const char *msg, int line)
{
    NEDError(NULL, "%s:%d: %s", filename, line, msg);
    num_errors++;
}

/*
void NEDParser::dbg(YYLTYPE lc, char *what)
{
    printf("%s: %d,%d,%d,%d=%s.\n",
            what,
            lc.first_line,lc.first_column,lc.last_line,lc.last_column,
            nedsource->get(lc));
}
*/

//-----------------------------------------------------------------------
// Usage:
//
//    NEDParser _np;
//    np = &_np;
//    return np->parseFile(interp, fname, nedfilekey, nedarray, errorsarray);
//
//    NEDParser _np;
//    np = &_np;
//    return np->parseText(interp, nedtext, nedfilekey, nedarray, errorsarray);

