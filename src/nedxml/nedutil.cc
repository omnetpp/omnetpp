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

/*
    public enum Tag { p, b, i, is, i2, r, q, t, tt,  // submodule tags
                      bgp, bgb, bgi, bgtt, bgg, bgl, bgs, // compound module background tags
                      m, a, ls, bp }                 // connection tags
*/

std::string DisplayStringUtil::upgradeBackgroundDisplayString(const char *s)
{
    DisplayString ds;
    ds.parse(s);
    renameTag(ds, "p",  "bgp");
    renameTag(ds, "b",  "bgb");
    renameTag(ds, "tt", "bgtt");
    return ds.toString();
}

std::string DisplayStringUtil::upgradeSubmoduleDisplayString(const char *s)
{
    return s;
}

std::string DisplayStringUtil::upgradeConnectionDisplayString(const char *s)
{
    return s;
}

std::string DisplayStringUtil::toOldBackgroundDisplayString(const char *s)
{
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
    return ds.toString();
}

std::string DisplayStringUtil::toOldSubmoduleDisplayString(const char *s)
{
    return s;
}

std::string DisplayStringUtil::toOldConnectionDisplayString(const char *s)
{
    return s;
}

