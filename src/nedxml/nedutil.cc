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
#include "displaystring.h"

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
    ds.parse(s);
    renameTag(ds, "p",  "bgp");
    renameTag(ds, "b",  "bgb");
    renameTag(ds, "tt", "bgtt");
    if (ds.existsTag("o"))
    {
        ds.setTagArg("bgb", 3, ds.getTagArg("o",0));
        ds.setTagArg("bgb", 4, ds.getTagArg("o",1));
        ds.setTagArg("bgb", 5, ds.getTagArg("o",2));
        ds.removeTag("o");
    }
    return ds.toString();
}

std::string DisplayStringUtil::upgradeSubmoduleDisplayString(const char *s)
{
    DisplayString ds;
    ds.parse(s);
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
    ds.parse(s);
    renameTag(ds, "o", "ls");
    return ds.toString();
}

std::string DisplayStringUtil::toOldBackgroundDisplayStringQ(const char *s)
{
    //FIXME remove/add quotes!
    DisplayString ds;
    ds.parse(s);
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

std::string DisplayStringUtil::toOldSubmoduleDisplayStringQ(const char *s)
{
    //FIXME remove/add quotes!
    DisplayString ds;
    ds.parse(s);
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

std::string DisplayStringUtil::toOldConnectionDisplayStringQ(const char *s)
{
    //FIXME remove/add quotes!
    DisplayString ds;
    ds.parse(s);
    renameTag(ds, "ls", "o");
    return ds.toString();
}

