//==========================================================================
//   CPAR.H   - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cPar       : general value holding class
//    cModulePar : specialized cPar that serves as module parameter
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include "cdispstr.h"


cDisplayStringParser::cDisplayStringParser()
{
    dispstr = NULL;
    buffer = NULL;
    bufferend = NULL;
    tags = NULL;
    numtags = 0;
}


cDisplayStringParser::cDisplayStringParser(const char *displaystr)
{
    dispstr = opp_strdup(displaystr);
    buffer = NULL;
    bufferend = NULL;
    tags = NULL;
    numtags = 0;
    parse();
}


cDisplayStringParser::~cDisplayStringParser()
{
    delete [] dispstr;
    cleartags();
}


const char *cDisplayStringParser::getString()
{
    assemble();
    return dispstr;
}


bool cDisplayStringParser::parse(const char *displaystr)
{
    delete [] dispstr;
    dispstr = opp_strdup(displaystr);
    return parse();
}


bool cDisplayStringParser::existsTag(const char *tagname)
{
    int t = gettagindex(tagname);
    return t!=-1;
}


int cDisplayStringParser::getNumArgs(const char *tagname)
{
    return getNumArgs(gettagindex(tagname));
}


const char *cDisplayStringParser::getTagArg(const char *tagname, int index)
{
    return getTagArg(gettagindex(tagname), index);
}


bool cDisplayStringParser::setTagArg(const char *tagname, int index, const char *value)
{
    return setTagArg(gettagindex(tagname), index, value);
}


bool cDisplayStringParser::removeTag(const char *tagname)
{
    return removeTag(gettagindex(tagname));
}


int cDisplayStringParser::getNumTags()
{
    return numtags;
}


const char *cDisplayStringParser::getTagName(int tagindex)
{
    if (tagindex<0 || tagindex>=numtags) return NULL;
    return tags[tagindex].name;
}


int cDisplayStringParser::getNumArgs(int tagindex)
{
    if (tagindex<0 || tagindex>=numtags) return -1;
    return tags[tagindex].numargs;
}


const char *cDisplayStringParser::getTagArg(int tagindex, int index)
{
    if (tagindex<0 || tagindex>=numtags) return NULL;
    if (index<0 || index>=tags[tagindex].numargs) return NULL;
    return tags[tagindex].args[index];
}


bool cDisplayStringParser::setTagArg(int tagindex, int index, const char *value)
{
    // check indices
    if (tagindex<0 || tagindex>=numtags) return false;
    if (index<0 || index>=MAXARGS) return false;

    // adjust numargs if necessary
    if (index>=tags[tagindex].numargs)
        tags[tagindex].numargs = index+1;

    // set value
    if (tags[tagindex].args[index] && !isinbuffer(tags[tagindex].args[index]))
        delete [] tags[tagindex].args[index];
    tags[tagindex].args[index] = opp_strdup(value);

    // success
    return true;
}


bool cDisplayStringParser::insertTag(const char *tagname, int atindex)
{
    // check uniqueness
    if (gettagindex(tagname)!=-1)
        return false;

    // check index
    if (atindex<0) atindex=0;
    if (atindex>numtags) atindex=numtags;

    // create new tags[] array with hole at atindex
    Tag *newtags = new Tag[numtags+1];
    for (int s=0,d=0; s<numtags; s++,d++)
    {
       if (d==atindex) d++; // make room for new item
       newtags[d] = tags[s];
    }
    delete [] tags;
    tags = newtags;
    numtags++;

    // fill in new tag
    tags[atindex].name = opp_strdup(tagname);
    tags[atindex].numargs = 0;
    for (int i=0; i<MAXARGS; i++) tags[atindex].args[i] = NULL;

    // success
    return true;
}


bool cDisplayStringParser::removeTag(int tagindex)
{
    if (tagindex<0 || tagindex>=numtags) return false;

    // dealloc strings in tag
    if (!isinbuffer(tags[tagindex].name))
        delete [] tags[tagindex].name;
    for (int i=0; i<tags[tagindex].numargs; i++)
        if (!isinbuffer(tags[tagindex].args[i]))
            delete [] tags[tagindex].args[i];

    // eliminate hole in tags[] array
    for (int t=tagindex; t<numtags-1; t++)
        tags[t] = tags[t+1];
    numtags--;

    // success
    return true;
}


int cDisplayStringParser::gettagindex(const char *tagname)
{
    for (int t=0; t<numtags; t++)
        if (!strcmp(tagname,tags[t].name))
            return t;
    return -1;
}

void cDisplayStringParser::cleartags()
{
    // delete tags array. string pointers that do not point inside the
    // buffer were allocated individually via new char[] and have to be
    // deleted.
    for (int t=0; t<numtags; t++)
    {
        if (!isinbuffer(tags[t].name))
            delete [] tags[t].name;
        for (int i=0; i<tags[t].numargs; i++)
            if (!isinbuffer(tags[t].args[i]))
                delete [] tags[t].args[i];
    }
    delete [] tags;
    tags = NULL;
    numtags = 0;

    // must be done after deleting tags[] because of isinbuffer()
    delete [] buffer;
    buffer = bufferend = NULL;
}

bool cDisplayStringParser::parse()
{
    cleartags();
    if (dispstr==NULL) return true;

    bool fully_ok = true;

    buffer = new char[opp_strlen(dispstr)+1];
    bufferend = buffer + opp_strlen(dispstr);

    // count tags (#(';')+1) & allocate tags[] array
    int n = 1;
    for (char *s1 = dispstr; *s1; s1++)
        if (*s1==';')
            n++;
    tags = new Tag[n];

    // parse string into tags[]. To avoid several small allocations,
    // pointers in tags[] will point into the buffer.
    numtags = 1;
    tags[0].name = buffer;
    tags[0].numargs = 0;
    for (int i=0; i<MAXARGS; i++)
        tags[0].args[i] = NULL;

    char *s, *d;
    for (s=dispstr,d=buffer; *s; s++,d++)
    {
        if (*s=='\\')   //FIXME: backslash parsing BAD!!!!!!!
        {
            // allow backslash as quote character
            s++;
            *d = *s;
        }
        else if (*s==';')
        {
            // new tag begins
            *d = '\0';
            numtags++;
            tags[numtags-1].name = d+1;
            tags[numtags-1].numargs = 0;
            for (int i=0; i<MAXARGS; i++) tags[numtags-1].args[i] = NULL;
        }
        else if (*s=='=')
        {
            // first argument of new tag begins
            *d = '\0';
            tags[numtags-1].numargs = 1;
            tags[numtags-1].args[0] = d+1;
        }
        else if (*s==',')
        {
            // new argument of current tag begins
            *d = '\0';
            if (tags[numtags-1].numargs>=MAXARGS)
            {
                fully_ok = false; // extra args ignored (there were too many)
            }
            else
            {
                tags[numtags-1].numargs++;
                tags[numtags-1].args[ tags[numtags-1].numargs-1 ] = d+1;
            }
        }
        else
        {
            *d = *s;
        }
    }
    *d = '\0';

    return fully_ok;
}

void cDisplayStringParser::assemble()
{
    // calculate length of display string
    int size = 0;
    for (int t0=0; t0<numtags; t0++)
    {
        size += opp_strlen(tags[t0].name)+2;
        for (int i=0; i<tags[t0].numargs; i++)
            size += opp_strlen(tags[t0].args[i])+1;
    }
    size = 2*size+1;  // 2* for worst case if every char has to be escaped

    // allocate display string
    delete [] dispstr;
    dispstr = new char[size];
    dispstr[0] = '\0';

    // assemble string
    for (int t=0; t<numtags; t++)
    {
        if (t!=0)
            strcat(dispstr, ";");
        strcatescaped(dispstr, tags[t].name);
        if (tags[t].numargs>0)
            strcat(dispstr, "=");
        for (int i=0; i<tags[t].numargs; i++)
        {
            if (i!=0) strcat(dispstr, ",");
            strcatescaped(dispstr, tags[t].args[i]);
        }
    }
}

void cDisplayStringParser::strcatescaped(char *d, const char *s)
{
    if (!s) return;

    d += strlen(d);
    while (*s)
    {
        if (*s==';' || *s==',' || *s=='=' || *s=='\\') *d++ = '\\';
        *d++ = *s++;
    }
    *d = '\0';
}


