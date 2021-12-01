//==========================================================================
//   DISPLAYSTRING.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdio>
#include "opp_ctype.h"
#include "displaystring.h"
#include "stringutil.h"

namespace omnetpp {
namespace common {

DisplayString::DisplayString(const char *displaystr) : assembledString(opp_strdup(displaystr))
{
    parse();
}

DisplayString::DisplayString(const DisplayString& ds)
{
    copy(ds);
}

DisplayString::~DisplayString()
{
    delete[] assembledString;
    clearTags();
}

DisplayString& DisplayString::operator=(const DisplayString& other)
{
    if (this == &other)
        return *this;
    copy(other);
    return *this;
}

const char *DisplayString::str() const
{
    if (assembledStringValid)
        assemble();
    return assembledString ? assembledString : "";
}

bool DisplayString::parse(const char *displaystr)
{
    // if it's the same, nothing to do
    if (assembledStringValid)
        assemble();
    if (!opp_strcmp(assembledString, displaystr))
        return true;

    // parse and store new string
    delete[] assembledString;
    clearTags();
    assembledString = opp_strdup(displaystr);
    bool fullyOK = parse();

    return fullyOK;
}

void DisplayString::updateWith(const DisplayString& ds)
{
    // elements in "ds" take precedence
    int n = ds.getNumTags();
    for (int i = 0; i < n; i++) {
        int m = ds.getNumArgs(i);
        for (int j = 0; j < m; j++) {
            const char *arg = ds.getTagArg(i, j);
            if (arg[0])
                setTagArg(ds.getTagName(i), j, arg);
        }
    }

    // optimize storage
    parse(str());
}

void DisplayString::updateWith(const char *s)
{
    DisplayString ds(s);
    updateWith(ds);
}

bool DisplayString::containsTag(const char *tagname) const
{
    int t = getTagIndex(tagname);
    return t != -1;
}

int DisplayString::getNumArgs(const char *tagname) const
{
    return getNumArgs(getTagIndex(tagname));
}

const char *DisplayString::getTagArg(const char *tagname, int index) const
{
    return getTagArg(getTagIndex(tagname), index);
}

bool DisplayString::setTagArg(const char *tagname, int index, const char *value)
{
    int t = getTagIndex(tagname);
    if (t == -1)
        t = insertTag(tagname);
    return setTagArg(t, index, value);
}

bool DisplayString::setTagArg(const char *tagname, int index, long value)
{
    char buf[32];
    sprintf(buf, "%ld", value);
    return setTagArg(tagname, index, buf);
}

bool DisplayString::removeTag(const char *tagname)
{
    return removeTag(getTagIndex(tagname));
}

int DisplayString::getNumTags() const
{
    return numTags;
}

const char *DisplayString::getTagName(int tagindex) const
{
    if (tagindex < 0 || tagindex >= numTags)
        return nullptr;
    return tags[tagindex].name;
}

int DisplayString::getNumArgs(int tagindex) const
{
    if (tagindex < 0 || tagindex >= numTags)
        return -1;
    return tags[tagindex].numArgs;
}

const char *DisplayString::getTagArg(int tagindex, int index) const
{
    if (tagindex < 0 || tagindex >= numTags)
        return "";
    if (index < 0 || index >= tags[tagindex].numArgs)
        return "";
    return opp_nulltoempty(tags[tagindex].args[index]);
}

bool DisplayString::setTagArg(int tagindex, int index, const char *value)
{
    // check indices
    if (tagindex < 0 || tagindex >= numTags)
        return false;
    if (index < 0 || index >= MAXARGS)
        return false;
    Tag& tag = tags[tagindex];

    // adjust numargs if necessary
    if (index >= tag.numArgs)
        tag.numArgs = index+1;

    // if it's the same, nothing to do
    char *& slot = tag.args[index];
    if (!opp_strcmp(slot, value))
        return true;

    // set value
    if (slot && !isInBuffer(slot))
        delete[] slot;
    slot = opp_strdup(value);

    // get rid of possible empty trailing args
    while (tag.numArgs > 0 && tag.args[tag.numArgs-1] == nullptr)
        tag.numArgs--;

    assembledStringValid = true;
    return true;
}

int DisplayString::insertTag(const char *tagname, int atindex)
{
    // check uniqueness
    int t = getTagIndex(tagname);
    if (t != -1)
        return t;

    // check index
    if (atindex < 0)
        atindex = 0;
    if (atindex > numTags)
        atindex = numTags;

    // create new tags[] array with hole at atindex
    Tag *newtags = new Tag[numTags+1];
    for (int s = 0, d = 0; s < numTags; s++, d++) {
        if (d == atindex)
            d++;  // make room for new item
        newtags[d] = tags[s];
    }
    delete[] tags;
    tags = newtags;
    numTags++;

    // fill in new tag
    tags[atindex].name = opp_strdup(tagname);
    tags[atindex].numArgs = 0;
    for (auto & arg : tags[atindex].args)
        arg = nullptr;

    // success
    assembledStringValid = true;
    return atindex;
}

bool DisplayString::removeTag(int tagindex)
{
    if (tagindex < 0 || tagindex >= numTags)
        return false;

    // dealloc strings in tag
    if (!isInBuffer(tags[tagindex].name))
        delete[] tags[tagindex].name;
    for (int i = 0; i < tags[tagindex].numArgs; i++)
        if (!isInBuffer(tags[tagindex].args[i]))
            delete[] tags[tagindex].args[i];


    // eliminate hole in tags[] array
    for (int t = tagindex; t < numTags-1; t++)
        tags[t] = tags[t+1];
    numTags--;

    // success
    assembledStringValid = true;
    return true;
}

int DisplayString::getTagIndex(const char *tagname) const
{
    for (int t = 0; t < numTags; t++)
        if (!strcmp(tagname, tags[t].name))
            return t;

    return -1;
}

void DisplayString::clearTags()
{
    // delete tags array. string pointers that do not point inside the
    // buffer were allocated individually via new char[] and have to be
    // deleted.
    for (int t = 0; t < numTags; t++) {
        if (!isInBuffer(tags[t].name))
            delete[] tags[t].name;
        for (int i = 0; i < tags[t].numArgs; i++)
            if (!isInBuffer(tags[t].args[i]))
                delete[] tags[t].args[i];

    }
    delete[] tags;
    tags = nullptr;
    numTags = 0;

    // must be done after deleting tags[] because of isinbuffer()
    delete[] buffer;
    buffer = bufferEnd = nullptr;
    assembledStringValid = true;
}

bool DisplayString::parse()
{
    clearTags();
    if (assembledString == nullptr)
        return true;

    bool fully_ok = true;

    buffer = new char[opp_strlen(assembledString)+1];
    bufferEnd = buffer + opp_strlen(assembledString);

    // count tags (#(';')+1) & allocate tags[] array
    int n = 1;
    for (char *s1 = assembledString; *s1; s1++)
        if (*s1 == ';')
            n++;

    tags = new Tag[n];

    // parse string into tags[]. To avoid several small allocations,
    // pointers in tags[] will point into the buffer.
    numTags = 1;
    tags[0].name = buffer;
    tags[0].numArgs = 0;
    for (auto & arg : tags[0].args)
        arg = nullptr;

    char *s, *d;
    for (s = assembledString, d = buffer; *s; s++, d++) {
        if (*s == '\\' && *(s+1)) {
            // allow escaping display string special chars (=,;) with backslash.
            // No need to deal with "\t", "\n" etc here, since they already got
            // interpreted by opp_parsequotedstr().
            *d = *++s;
        }
        else if (*s == ';') {
            // new tag begins
            *d = '\0';
            numTags++;
            tags[numTags-1].name = d+1;
            tags[numTags-1].numArgs = 0;
            for (auto & arg : tags[numTags-1].args)
                arg = nullptr;
        }
        else if (*s == '=') {
            // first argument of new tag begins
            *d = '\0';
            tags[numTags-1].numArgs = 1;
            tags[numTags-1].args[0] = d+1;
        }
        else if (*s == ',') {
            // new argument of current tag begins
            *d = '\0';
            if (tags[numTags-1].numArgs >= MAXARGS) {
                fully_ok = false;  // extra args ignored (there were too many)
            }
            else {
                tags[numTags-1].numArgs++;
                tags[numTags-1].args[tags[numTags-1].numArgs-1] = d+1;
            }
        }
        else {
            *d = *s;
        }
    }
    *d = '\0';

    // check tag names are OK (matching [a-zA-Z0-9:]+)
    for (int i = 0; i < numTags; i++) {
        if (!tags[i].name[0])
            fully_ok = false;  // empty string as tagname
        for (const char *s = tags[i].name; *s; s++)
            if (!opp_isalnum(*s) && *s != ':')
                fully_ok = false; // tagname contains invalid character
    }
    return fully_ok;
}

void DisplayString::assemble() const
{
    // calculate length of display string
    int size = 0;
    for (int t0 = 0; t0 < numTags; t0++) {
        size += opp_strlen(tags[t0].name)+2;
        for (int i = 0; i < tags[t0].numArgs; i++)
            size += opp_strlen(tags[t0].args[i])+1;
    }
    size = 2*size+1;  // 2* for worst case if every char has to be escaped

    // allocate display string
    delete[] assembledString;
    assembledString = new char[size];
    assembledString[0] = '\0';

    // assemble string
    for (int t = 0; t < numTags; t++) {
        if (t != 0)
            strcat(assembledString, ";");
        strcatEscaped(assembledString, tags[t].name);
        if (tags[t].numArgs > 0)
            strcat(assembledString, "=");
        for (int i = 0; i < tags[t].numArgs; i++) {
            if (i != 0)
                strcat(assembledString, ",");
            strcatEscaped(assembledString, tags[t].args[i]);
        }
    }
    assembledStringValid = false;
}

void DisplayString::strcatEscaped(char *d, const char *s)
{
    if (!s)
        return;

    d += strlen(d);
    while (*s) {
        // quoting \t, \n etc is the job of opp_quotestr()
        if (*s == ';' || *s == ',' || *s == '=')
            *d++ = '\\';
        *d++ = *s++;
    }
    *d = '\0';
}

}  // namespace common
}  // namespace omnetpp

