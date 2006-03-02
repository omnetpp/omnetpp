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

    tree = tryParse();

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

    tree = tryParse();

    // true if OK
    return tree!=NULL;
}

NEDElement *NEDParser::tryParse()
{
guessFileType(nedsource->getFullText());

    NEDElement *tmp;

    clearErrors();
    tmp = doParseNED2(this, nedsource->getFullText());

    if (errorsOccurred())
    {
        delete tmp;
        clearErrors();
        tmp = doParseNED(this, nedsource->getFullText());
    }
    if (errorsOccurred())
    {
        delete tmp;
        clearErrors();
        tmp = doParseMSG2(this, nedsource->getFullText());
    }
    if (errorsOccurred())
    {
        delete tmp;
        tmp = NULL;
    }
    return tmp;
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

int NEDParser::guessFileType(const char *txt)
{
    // first, remove all comments
    char *buf = new char [strlen(txt)+1];
    const char *s;
    char *d;
    for (s=txt, d=buf; *s; )
    {
        if (*s=='/' && *(s+1)=='/') {
            // if there's a comment, skip rest of the line
            s += 2;
            while (*s && *s!='\r' && *s!='\n')
                s++;
        }
        else if (*s=='"') {
            // leave out string literals as well
            s++;
            while (*s && *s!='\r' && *s!='\n' && *s!='"')
                if (*s++=='\\')
                    s++;
            if (*s=='"')
                s++;
        }
        else {
            // copy everything else
            *d++ = *s++;
        }
    }
    *d = '\0';

    // try to recognize things in it
    bool hasCurlyBrace = strchr(buf,'{') || strchr(buf,'}');

    // FIXME only if it's "whole word"; not bulletproof because old NED keywords are not
    // reserved in NED2
    bool hasOldNedKeyword = strstr(buf,"endmodule") || strstr(buf,"endsimple") ||
                            strstr(buf,"endchannel") || strstr(buf,"endnetwork");
    bool hasMsgKeyword = strstr(buf,"{{") || strstr(buf,"}}") || strstr(buf,"cplusplus") ||
                         strstr(buf,"message") || strstr(buf,"struct") || strstr(buf,"class") ||
                         strstr(buf,"enum");

    int ret = hasCurlyBrace ? FT_NED2 : hasOldNedKeyword ? FT_NED : FT_NED2; // may be msg as well

    // cleanup
    delete [] buf;
    return ret;
}

