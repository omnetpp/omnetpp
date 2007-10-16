//==========================================================================
//  VALUEITERATOR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include "opp_ctype.h"
#include "valueiterator.h"
#include "commonutil.h"
#include "stringtokenizer.h"
#include "cexception.h"


//FIXME use new StringTokenizer from experimental/inifile, otherwise string literals with commas inside won't work!

ValueIterator::ValueIterator(const char *s)
{
    pos = itemIndex = k = 0;
    if (s)
        parse(s);
}

ValueIterator::~ValueIterator()
{
}

void ValueIterator::parse(const char *s)
{
    items.clear();
    StringTokenizer tokenizer(s, ","); //XXX turn on respecting quotes
    while (tokenizer.hasMoreTokens())
    {
        Item item;
        item.text = tokenizer.nextToken();
        parseAsNumericRegion(item);
        item.n = item.isNumeric ? std::max(0, (int)floor((item.to - item.from + item.step) / item.step)) : 1;
        items.push_back(item);
    }
    restart();
}

static const char *PARSEERROR = "Error in numeric range syntax `%s', <number>..<number> or <number>..<number> step <number> expected";

void ValueIterator::parseAsNumericRegion(Item& item)
{
    item.isNumeric = false;
    item.from = item.to = item.step = 0;

    const char *s = item.text.c_str();
    char *endp;

    double from = strtod(s, &endp);
    if (endp==s)
        return; // no number here
    if (*(endp-1)=='.') endp--; // strtod() ate one of our dots!
    s = endp;
    while (opp_isspace(*s)) s++;
    if (*s!='.' || *(s+1)!='.')
        return; // missing ".."
    s+= 2;
    while (opp_isspace(*s)) s++;
    double to = strtod(s, &endp);
    if (endp==s)
        throw cRuntimeError(PARSEERROR, item.text.c_str()); // missing number after ".."
    s = endp;
    while (opp_isspace(*s)) s++;
    if (!*s) {
        item.isNumeric = true;
        item.from = from;
        item.to = to;
        item.step = 1;
        return; // OK
    }
    if (s[0]!='s' || s[1]!='t' || s[2]!='e' || s[3]!='p')
        throw cRuntimeError(PARSEERROR, item.text.c_str()); // "step" is supposed to follow, we encountered some garbage instead
    s+= 4;
    while (opp_isspace(*s)) s++;
    double step = strtod(s, &endp);
    if (endp==s || step==0)
        throw cRuntimeError(PARSEERROR, item.text.c_str()); // no number (or explicit 0) after "step"
    s = endp;
    while (opp_isspace(*s)) s++;
    if (*s)
        throw cRuntimeError(PARSEERROR, item.text.c_str()); // trailing garbage after "step <number>"

    // OK
    item.isNumeric = true;
    item.from = from;
    item.to = to;
    item.step = step;
}

int ValueIterator::length() const
{
    int n = 0;
    for (int i=0; i<(int)items.size(); i++)
        n += items[i].n;
    return n;
}

std::string ValueIterator::get(int index) const
{
    if (index<0 || index>length())
        throw cRuntimeError("ValueIterator: index %d out of bounds", index);

    int k = 0;
    for (int i=0; i<(int)items.size(); i++)
    {
        const Item& item = items[i];
        if (!item.isNumeric)
        {
            if (k==index)
                return item.text;
            k++;
        }
        else if (item.n > 0)
        {
            if (k <= index && index < k+item.n) {
                char buf[32];
                sprintf(buf, "%g", item.from + item.step*(index-k));
                return buf;
            }
            k += item.n;
        }
    }
    Assert(false);
}

void ValueIterator::restart()
{
    pos = itemIndex = k = 0;
    while (itemIndex < (int)items.size() && items[itemIndex].n == 0) 
    	itemIndex++;
}

void ValueIterator::operator++(int)
{
    if (itemIndex >= (int)items.size())
        return;
    pos++;
    const Item& item = items[itemIndex];
    if (k < item.n-1) {
        k++;
    }
    else {
        k = 0;
        while (++itemIndex < (int)items.size() && items[itemIndex].n == 0);
    }
}

std::string ValueIterator::get() const
{
    if (itemIndex >= (int)items.size())
        return "";
    const Item& item = items[itemIndex];
    if (!item.isNumeric) {
        return item.text;
    }
    else {
        char buf[32];
        sprintf(buf, "%g", item.from + item.step*k);
        return buf;
    }
}

bool ValueIterator::end() const
{
    return itemIndex >= (int)items.size();
}

void ValueIterator::dump() const
{
    printf("parsed form: ");
    for (int i=0; i<(int)items.size(); i++)
    {
        const Item& item = items[i];
        if (i>0) printf(", ");
        if (!item.isNumeric)
            printf("\"%s\"", item.text.c_str());
        else
            printf("range(%g..%g step %g)", item.from, item.to, item.step);
    }
    printf("; enumeration: ");
    int n = length();
    for (int i=0; i<n; i++)
    {
        if (i>0) printf(", ");
        printf("%s", get(i).c_str());
    }
    printf(".\n");
}

