//==========================================================================
//  NEDPARSER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

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

    parseexpr = true;
    storesrc = false;
}

NEDParser::~NEDParser()
{
    delete tree;
}

bool NEDParser::parseFile(const char *fname)
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

    // store file name -- with slashes always, even on Windows
    std::string slashfname = fname;
    for (char *s=const_cast<char *>(slashfname.data()); *s; s++)
        if (*s=='\\')
            *s='/';
    nedfile->setFilename(slashfname.c_str());

    tree = nedfile;

    // init and call parser
    yyout = stdout;
    yyin = fopen(newfilename,"r");
    if (!yyin)
        {NEDError(NULL, "cannot read %s", fname); return false;}
    runparse(this,nedfile,parseexpr,storesrc,fname);
    fclose(yyin);

    // num_errors contains number of parse errors
    return true;
}

bool NEDParser::parseText(const char *nedtext)
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
    runparse(this,nedfile,parseexpr,storesrc,"generated-code");
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

