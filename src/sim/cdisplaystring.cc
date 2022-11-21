//==========================================================================
//   CDISPLAYSTRING.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdio>
#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cmodelchange.h"

using namespace omnetpp::common;

namespace omnetpp {

cDisplayString::cDisplayString(const char *displaystr) :  assembledString(opp_strdup(displaystr))
{
    doParse();
}

cDisplayString::cDisplayString(const cDisplayString& ds)
{
    copy(ds);
}

cDisplayString::~cDisplayString()
{
    delete[] assembledString;
    clearTags();
}

cDisplayString& cDisplayString::operator=(const cDisplayString& ds)
{
    if (this == &ds)
        return *this;
    copy(ds);
    return *this;
}

void cDisplayString::beforeChange()
{
    // notify pre-change listeners
    if (ownerComponent && ownerComponent->hasListeners(PRE_MODEL_CHANGE)) {
        cPreDisplayStringChangeNotification tmp;
        tmp.displayString = this;
        ownerComponent->emit(PRE_MODEL_CHANGE, &tmp);
    }
}

void cDisplayString::afterChange()
{
    assembledStringValid = false;

    if (ownerComponent) {
#ifdef SIMFRONTEND_SUPPORT
        ownerComponent->updateLastChangeSerial();
#endif
        EVCB.displayStringChanged(ownerComponent);

        // notify post-change listeners
        if (ownerComponent->hasListeners(POST_MODEL_CHANGE)) {
            cPostDisplayStringChangeNotification tmp;
            tmp.displayString = this;
            ownerComponent->emit(POST_MODEL_CHANGE, &tmp);
        }
    }
}

const char *cDisplayString::str() const
{
    if (!assembledStringValid)
        assemble();
    return assembledString ? assembledString : "";
}

void cDisplayString::parse(const char *displaystr)
{
    beforeChange();
    doParse(displaystr);
    afterChange();
}

void cDisplayString::doParse(const char *displaystr)
{
    // if it's the same, nothing to do
    if (!assembledStringValid)
        assemble();
    if (!opp_strcmp(assembledString, displaystr))
        return;

    // parse and store new string
    delete[] assembledString;
    clearTags();
    assembledString = opp_strdup(displaystr);
    doParse();
}

void cDisplayString::updateWith(const char *s)
{
    cDisplayString ds(s);
    updateWith(ds);
}

void cDisplayString::updateWith(const cDisplayString& ds)
{
    beforeChange();
    doUpdateWith(ds);
    afterChange();
}

void cDisplayString::doUpdateWith(const cDisplayString& ds)
{
    // elements in "ds" take precedence
    int n = ds.getNumTags();
    for (int i = 0; i < n; i++) {
        int m = ds.getNumArgs(i);
        for (int j = 0; j < m; j++) {
            const char *arg = ds.getTagArg(i, j);
            if (arg[0] == '-' && !arg[1])  // "-" is the "antivalue"
                doSetTagArg(ds.getTagName(i), j, "");
            else if (arg[0])
                doSetTagArg(ds.getTagName(i), j, arg);
        }
    }

    // optimize storage
    doParse(str());
}

bool cDisplayString::containsTag(const char *tagname) const
{
    int t = getTagIndex(tagname);
    return t != -1;
}

int cDisplayString::getNumArgs(const char *tagname) const
{
    return getNumArgs(getTagIndex(tagname));
}

const char *cDisplayString::getTagArg(const char *tagname, int index) const
{
    return getTagArg(getTagIndex(tagname), index);
}

bool cDisplayString::setTagArg(const char *tagname, int index, long value)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%ld", value);
    return setTagArg(tagname, index, buf);
}

bool cDisplayString::setTagArg(const char *tagname, int index, const char *value)
{
    int tagIndex = getTagIndex(tagname);
    if (opp_strcmp(getTagArg(tagIndex, index), value) != 0) {
        beforeChange();
        if (tagIndex == -1)
            tagIndex = doInsertTag(tagname);
        bool result = doSetTagArg(tagIndex, index, value);
        afterChange();
        return result;
    }
    return false;
}

bool cDisplayString::doSetTagArg(const char *tagname, int index, const char *value)
{
    int t = getTagIndex(tagname);
    if (t == -1)
        t = doInsertTag(tagname);
    return doSetTagArg(t, index, value);
}

bool cDisplayString::removeTag(const char *tagname)
{
    return removeTag(getTagIndex(tagname));
}

int cDisplayString::getNumTags() const
{
    return numTags;
}

const char *cDisplayString::getTagName(int tagindex) const
{
    if (tagindex < 0 || tagindex >= numTags)
        return nullptr;
    return tags[tagindex].name;
}

int cDisplayString::getNumArgs(int tagindex) const
{
    if (tagindex < 0 || tagindex >= numTags)
        return 0;
    return tags[tagindex].numArgs;
}

const char *cDisplayString::getTagArg(int tagindex, int index) const
{
    if (tagindex < 0 || tagindex >= numTags)
        return "";
    if (index < 0 || index >= tags[tagindex].numArgs)
        return "";
    return opp_nulltoempty(tags[tagindex].args[index]);
}

bool cDisplayString::setTagArg(int tagindex, int index, const char *value)
{
    beforeChange();
    bool result = doSetTagArg(tagindex, index, value);
    afterChange();
    return result;
}

bool cDisplayString::doSetTagArg(int tagindex, int index, const char *value)
{
    // check indices
    if (tagindex < 0 || tagindex >= numTags)
        return false;
    if (index < 0 || index >= MAXARGS)
        return false;
    Tag& tag = tags[tagindex];

    // adjust numargs if necessary
    if (index >= tag.numArgs)
        tag.numArgs = index + 1;

    // if it's the same, nothing to do
    char *& slot = tag.args[index];
    if (!opp_strcmp(slot, value))
        return true;

    // set value
    if (slot && !pointsIntoBuffer(slot))
        delete[] slot;
    slot = opp_strdup(value);

    // get rid of possible empty trailing args, throw out tag if it became empty
    while (tag.numArgs > 0 && tag.args[tag.numArgs - 1] == nullptr)
        tag.numArgs--;
    if (tag.numArgs == 0)
        doRemoveTag(tagindex);
    return true;
}

int cDisplayString::insertTag(const char *tagname, int atindex)
{
    beforeChange();
    int result = doInsertTag(tagname, atindex);
    afterChange();
    return result;
}

int cDisplayString::doInsertTag(const char *tagname, int atindex)
{
    // check name validity
    if (!tagname || !tagname[0])
        throw cRuntimeError("cDisplayString::insertTag(): Tag name cannot be empty");
    for (const char *s = tagname; *s; s++)
        if (!opp_isalnum(*s) && *s != ':')
            throw cRuntimeError("cDisplayString::insertTag(): Name \"%s\" contains illegal character '%c'", tagname, *s);

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
    Tag *newtags = new Tag[numTags + 1];
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
    return atindex;
}

bool cDisplayString::removeTag(int tagindex)
{
    beforeChange();
    bool result = doRemoveTag(tagindex);
    afterChange();
    return result;
}

bool cDisplayString::doRemoveTag(int tagindex)
{
    if (tagindex < 0 || tagindex >= numTags)
        return false;

    // dealloc strings in tag
    if (!pointsIntoBuffer(tags[tagindex].name))
        delete[] tags[tagindex].name;
    for (int i = 0; i < tags[tagindex].numArgs; i++)
        if (!pointsIntoBuffer(tags[tagindex].args[i]))
            delete[] tags[tagindex].args[i];


    // eliminate hole in tags[] array
    for (int t = tagindex; t < numTags-1; t++)
        tags[t] = tags[t+1];
    numTags--;

    // success
    return true;
}

int cDisplayString::getTagIndex(const char *tagname) const
{
    for (int t = 0; t < numTags; t++)
        if (!strcmp(tagname, tags[t].name))
            return t;

    return -1;
}

void cDisplayString::clearTags()
{
    // delete tags array. string pointers that do not point inside the
    // buffer were allocated individually via new char[] and have to be
    // deleted.
    for (int t = 0; t < numTags; t++) {
        if (!pointsIntoBuffer(tags[t].name))
            delete[] tags[t].name;
        for (int i = 0; i < tags[t].numArgs; i++)
            if (!pointsIntoBuffer(tags[t].args[i]))
                delete[] tags[t].args[i];

    }
    delete[] tags;
    tags = nullptr;
    numTags = 0;

    // must be done after deleting tags[] because of pointsIntoBuffer()
    delete[] buffer;
    buffer = bufferEnd = nullptr;
}

void cDisplayString::doParse()
{
    clearTags();
    if (assembledString == nullptr)
        return;

    buffer = new char[opp_strlen(assembledString) + 1];
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
    bool insideTagName = true;
    for (s = assembledString, d = buffer; *s; s++, d++) {
        if (*s == '\\') {
            // Allow escaping display string special chars (=,;) with backslash.
            // No need to deal with "\t", "\n" etc here. For display strings in
            // NED files, they are already interpreted during NED parsing of
            // string literals, and one level of backslashes is also removed.
            // The backslashes that remain after don't add any special meaning
            // (like tab or newline) to the character following them, they only
            // take away special meaning they might have otherwise (as value or
            // tag separators). The "$" does not lose its meaning (not even in
            // "${expression(1, b)}"), as that is interpreted in a later stage.
            if (*(s+1) != '\0')
                *d = *++s;
            else
                d--; // neutralize end-loop d++
        }
        else if (*s == ';') {
            // new tag begins
            *d = '\0';
            numTags++;
            tags[numTags-1].name = d+1;
            tags[numTags-1].numArgs = 0;
            for (auto & arg : tags[numTags-1].args)
                arg = nullptr;
            insideTagName = true;
        }
        else if (*s == '$' && *(s+1) == '$' && !insideTagName) {
            *d++ = *s++;
            *d = *s;
        }
        else if (*s == '$' && *(s+1) == '{' && !insideTagName) {
            // skip expression
            const char *end = opp_findmatchingparen(s+1);
            if (!end)
                throw cRuntimeError("Unmatched '{' in '%s'", assembledString);
            while (s != end)
                *d++ = *s++;
            *d = *s;
        }
        else if (*s == '=' && insideTagName) {
            // first argument of new tag begins
            *d = '\0';
            tags[numTags-1].numArgs = 1;
            tags[numTags-1].args[0] = d+1;
            insideTagName = false;
        }
        else if (*s == ',' && !insideTagName) {
            // new argument of current tag begins
            *d = '\0';
            if (tags[numTags-1].numArgs >= MAXARGS)
                throw cRuntimeError("Too many tag arguments (max %d allowed) while parsing display string \"%s\"", MAXARGS, assembledString);
            tags[numTags-1].numArgs++;
            tags[numTags-1].args[tags[numTags-1].numArgs-1] = d+1;
        }
        else {
            *d = *s;
        }
    }
    *d = '\0';

    // check tag names are OK (matching [a-zA-Z0-9:]+)
    for (int i = 0; i < numTags; i++) {
        if (!tags[i].name[0]) {
            if (tags[i].numArgs == 0)
                ;  // empty tag (occurs when there're redundant semicolons, or the display string is empty) -- XXX remove it
            else
                throw cRuntimeError("Missing tag name encountered while parsing display string \"%s\"", assembledString);
        }
        for (const char *s = tags[i].name; *s; s++)
            if (!opp_isalnum(*s) && *s != ':')
                throw cRuntimeError("Illegal character \"%c\" encountered in tag name while parsing display string \"%s\"", *s, assembledString);

    }
}

void cDisplayString::assemble() const
{
    // calculate length of display string
    int size = 0;
    for (int t0 = 0; t0 < numTags; t0++) {
        size += opp_strlen(tags[t0].name) + 2;
        for (int i = 0; i < tags[t0].numArgs; i++)
            size += opp_strlen(tags[t0].args[i])+1;
    }
    size = 2 * size+1;  // 2* for worst case if every char has to be escaped

    // allocate display string
    delete[] assembledString;
    assembledString = new char[size];
    assembledString[0] = '\0';

    // assemble string
    for (int t = 0; t < numTags; t++) {
        if (t != 0)
            strcat(assembledString, ";");
        strcatescaped(assembledString, tags[t].name);
        if (tags[t].numArgs > 0)
            strcat(assembledString, "=");
        for (int i = 0; i < tags[t].numArgs; i++) {
            if (i != 0)
                strcat(assembledString, ",");
            strcatescaped(assembledString, tags[t].args[i]);
        }
    }
    assembledStringValid = true;
}

void cDisplayString::strcatescaped(char *d, const char *s)
{
    if (!s)
        return;

    d += strlen(d);
    while (*s) {
        // skipping param refs
        if (*s == '$' && *(s+1) == '$') {
            *d++ = *s++;
            *d++ = *s++;
        }
        else if (*s == '$' && *(s+1) == '{') {
            const char *end = opp_findmatchingparen(s+1);
            if (!end)
                throw cRuntimeError("Unmatched '{' in '%s'", s);

            while (s != end)
                *d++ = *s++;
            *d++ = *s++; // the }
        }
        else {
            // quoting \t, \n etc is the job of opp_quotestr()
            if (*s == ';' || *s == ',' || *s == '=')
                *d++ = '\\';
            *d++ = *s++;
        }
    }
    *d = '\0';
}

void cDisplayString::dump(std::ostream& out) const
{
    out << "`" << str() << "`" << endl;
    for (int t = 0; t < numTags; t++) {
        out << "  `" << tags[t].name << "` =" << endl;
        for (int i = 0; i < tags[t].numArgs; i++)
            out << "    [" << i << "] `" << tags[t].args[i] << "`" << endl;
    }
}

}  // namespace omnetpp

