//==========================================================================
// neddtdvalidator1.cc -
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

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "nederror.h"
#include "neddtdvalidator.h"


void NEDDTDValidator::checkSequence(NEDElement *node, int tags[], char mult[], int n)
{
    NEDElement *p = node->getFirstChild();
    for (int i=0; i<n; i++)
    {
       switch (mult[i])
       {
         case '1':
           if (!p || p->getTagCode()!=tags[i])
               {NEDError(node,"DTD validation error: child element '%s' unexpected", (p?p->getTagName():"")); return;}
           p = p->getNextSibling();
           break;
         case '?':
           if (p && p->getTagCode()==tags[i])
               p = p->getNextSibling();
           break;
         case '+':
           if (!p || p->getTagCode()!=tags[i])
               {NEDError(node, "DTD validation error: child element '%s' unexpected", (p?p->getTagName():"")); return;}
           p = p->getNextSibling();
           while (p && p->getTagCode()==tags[i])
               p = p->getNextSibling();
           break;
         case '*':
           while (p && p->getTagCode()==tags[i])
               p = p->getNextSibling();
           break;
       }
    }
    if (p)
        {NEDError(node, "DTD validation error: child element '%s' unexpected", p->getTagName()); return;}
}

void NEDDTDValidator::checkChoice(NEDElement *node, int tags[], int n, char mult)
{
    NEDElement *p = node->getFirstChild();
    int i;
    if (mult=='1' || mult=='+')
    {
        if (!p)
             {NEDError(node,"DTD validation error: child element '' unexpected\n"); return;}
        for (i=0; i<n; i++)
             if (p->getTagCode()==tags[i])
                 break;
        if (i==n)
             {NEDError(node,"DTD validation error: child element '%s' unexpected", p->getTagName()); return;}
        p = p->getNextSibling();
    }
    if (mult=='+' || mult=='*')
    {
        while (p)
        {
            for (i=0; i<n; i++)
                if (p->getTagCode()==tags[i])
                   break;
            if (i==n)
                {NEDError(node,"DTD validation error: child element '%s' unexpected", p->getTagName()); return;}
            p = p->getNextSibling();
        }
    }
}

void NEDDTDValidator::checkEmpty(NEDElement *node)
{
    if (node->getFirstChild()) {
        NEDError(node,"DTD validation error: EMPTY element has children\n");
    }
}

void NEDDTDValidator::checkRequiredAttribute(NEDElement *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    if (!s || !*s)
        {NEDError(node,"DTD validation error: required attribute %s is empty", attr); return;}
}

void NEDDTDValidator::checkEnumeratedAttribute(NEDElement *node, const char *attr, const char *vals[], int n)
{
    const char *s = node->getAttribute(attr);
    if (!s || !*s)
        {NEDError(node,"DTD validation error: required attribute %s is empty", attr); return;}
    for (int i=0; i<n; i++)
        if (!strcmp(s, vals[i]))
            return;
    if (n==0)
        INTERNAL_ERROR1(node,"no allowed values for enumerated attribute %s", attr);
    NEDError(node,"DTD validation error: invalid value for attribute %s, not one of the "
                  "enumerated values ('%s',...)", attr, vals[0]);
}

void NEDDTDValidator::checkNameAttribute(NEDElement *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    if (!s || !*s)
        return;
    if (!isalpha(*s) && *s!='_')
        NEDError(node,"DTD validation error: attribute %s='%s' starts with invalid character (valid NED identifier expected)", attr, node->getAttribute(attr));
    while (*++s)
        if (!isalpha(*s) && !isdigit(*s) && *s!='_')
            {NEDError(node,"DTD validation error: attribute %s='%s' contains invalid character (valid NED identifier expected)", attr, node->getAttribute(attr)); return;}
}

void NEDDTDValidator::checkCommentAttribute(NEDElement *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    if (!s || !*s)
        return;
    bool incomment = false;
    for (; *s; s++)
    {
        if (!incomment)
        {
            if (*s=='/' && *(s+1)=='/')
                {s++; incomment = true;}
            else if (*s!=' ' && *s!='\t' && *s!='\n' && *s!='\r')
                {NEDError(node,"DTD validation error: attribute %s='%s' does not contain a valid NED comment", attr, node->getAttribute(attr)); return;}
        }
        else if (incomment)
        {
            if (*s=='\n' || *s=='\r')
                incomment = false;
        }
    }
}

void NEDDTDValidator::checkNMTokenAttribute(NEDElement *node, const char *attr)
{
    //
    // DEVIATION FROM W3C STANDARDS. NMTOKEN should allow letters, digits, period,
    // dash, underscore, colon, combining characters, extenders. Letters would include
    // international characters as well. However, in our NED DTD we use NMTOKEN for
    // NED (and C++) identifiers which allow ASCII letters, digits and underscore ONLY.
    //
    const char *s = node->getAttribute(attr);
    if (!s || !*s)
        return;
    for (; *s; s++)
        if (!isalpha(*s) && !isdigit(*s) && *s!='_')
            {NEDError(node,"DTD validation error: attribute %s='%s' contains invalid character", attr, node->getAttribute(attr)); return;}
}



