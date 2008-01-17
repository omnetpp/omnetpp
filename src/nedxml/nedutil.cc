//==========================================================================
// nedutil.cc - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   misc util functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nederror.h"
#include "nedutil.h"
#include "stringutil.h"
#include "displaystring.h"

USING_NAMESPACE

static void renameTag(DisplayString& ds, const char *from, const char *to)
{
    int n = ds.getNumArgs(from);
    for (int i=0; i<n; i++)
        ds.setTagArg(to, i, ds.getTagArg(from,i));
    ds.removeTag(from);
}

std::string DisplayStringUtil::upgradeBackgroundDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    renameTag(ds, "p",  "bgp");
    renameTag(ds, "b",  "bgb");
    renameTag(ds, "tt", "bgtt");
    if (ds.existsTag("o"))
    {
        ds.setTagArg("bgb", 2, ds.getTagArg("o",0));
        ds.setTagArg("bgb", 3, ds.getTagArg("o",1));
        ds.setTagArg("bgb", 4, ds.getTagArg("o",2));
        ds.removeTag("o");
    }
    return ds.toString();
}

std::string DisplayStringUtil::upgradeSubmoduleDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    if (ds.existsTag("o"))
    {
        ds.setTagArg("b", 3, ds.getTagArg("o",0));
        ds.setTagArg("b", 4, ds.getTagArg("o",1));
        ds.setTagArg("b", 5, ds.getTagArg("o",2));
        ds.removeTag("o");
    }
    return ds.toString();
}

std::string DisplayStringUtil::upgradeConnectionDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    renameTag(ds, "o", "ls");
    // FIXME TT tag: the color parameter (old format 2nd) should go to the 3rd position in the new format
    // the 2nd par is position (was not supported previously)    
    return ds.toString();
}

void DisplayStringUtil::parseDisplayString(const char *s, DisplayString& ds)
{
    if (!ds.parse(s))
        throw opp_runtime_error("parse error in display string `%s'", s);
}

std::string DisplayStringUtil::toOldBackgroundDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    for (int i=0; i<ds.getNumTags(); i++)
    {
        const char *t = ds.getTagName(i);
        if (strcmp(t,"bgp")!=0 && strcmp(t,"bgb")!=0 && strcmp(t,"bgtt")!=0)
            ds.removeTag(i--);
    }
    renameTag(ds, "bgp",  "p");
    renameTag(ds, "bgb",  "b");
    renameTag(ds, "bgtt", "tt");
    if (ds.getNumArgs("b")>3)
    {
        ds.setTagArg("o", 0, ds.getTagArg("b",3));
        ds.setTagArg("o", 1, ds.getTagArg("b",4));
        ds.setTagArg("o", 2, ds.getTagArg("b",5));
        ds.setTagArg("b", 3, "");
        ds.setTagArg("b", 4, "");
        ds.setTagArg("b", 5, "");
    }
    return ds.toString();
}

std::string DisplayStringUtil::toOldSubmoduleDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    if (ds.getNumArgs("b")>3)
    {
        ds.setTagArg("o", 0, ds.getTagArg("b",3));
        ds.setTagArg("o", 1, ds.getTagArg("b",4));
        ds.setTagArg("o", 2, ds.getTagArg("b",5));
        ds.setTagArg("b", 3, "");
        ds.setTagArg("b", 4, "");
        ds.setTagArg("b", 5, "");
    }
    return ds.toString();
}

std::string DisplayStringUtil::toOldConnectionDisplayString(const char *s)
{
    DisplayString ds;
    parseDisplayString(s, ds);
    renameTag(ds, "ls", "o");
    return ds.toString();
}

