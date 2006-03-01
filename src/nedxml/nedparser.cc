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

#include "nedyydefs.h"


NEDParser *np;

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

    clearErrors();
    tree = doParseNED2(this, nedsource->getFullText());

    if (errorsOccurred())
    {
        delete tree;
        clearErrors();
        tree = doParseNED(this, nedsource->getFullText());
    }
    if (errorsOccurred())
    {
        delete tree;
        clearErrors();
        tree = doParseMSG2(this, nedsource->getFullText());
    }
    if (errorsOccurred())
    {
        delete tree;
        tree = NULL;
    }

    // true if OK
    return tree!=NULL;
}

bool NEDParser::parseText(const char *nedtext)
{
    // init global vars
    NEDFileBuffer nf;
    nedsource = &nf;
    filename = "buffer";

    // load whole file into memory
    if (!nedsource->setData(nedtext))
        {NEDError(NULL, "unable to allocate work memory"); return false;}

    tree = doParseNED2(this, nedtext);

    // num_errors contains number of parse errors
    return true;
}


NEDElement *NEDParser::getTree()
{
    NEDElement *ret = tree;
    tree = NULL;
    return ret;
}


void NEDParser::error(const char *msg, int line)
{
    NEDError(NULL, "%s:%d: %s", filename, line, msg);
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


