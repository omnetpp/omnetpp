//==========================================================================
// neddtdvalidator1.cc - part of the OMNeT++ Discrete System Simulation System
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002 Andras Varga

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
               {NEDError(node, "DTD validation error: child element '%s' unexpected", (p?p->getTagName():"")); return;} // FIXME error!
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
        {NEDError(node, "DTD validation error: child element '%s' unexpected", p->getTagName()); return;} // FIXME error!
}

void NEDDTDValidator::checkChoice(NEDElement *node, int tags[], int n, char mult)
{
    NEDElement *p = node->getFirstChild();
    int i;
    if (mult=='1' || mult=='+')
    {
        if (!p)
             {NEDError(node,"DTD validation error: child element '' unexpected\n"); return;} // FIXME error!
        for (i=0; i<n; i++)
             if (p->getTagCode()==tags[i])
                 break;
        if (i==n)
             {NEDError(node,"DTD validation error: child element '%s' unexpected", p->getTagName()); return;} // FIXME error!
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
                {NEDError(node,"DTD validation error: child element '%s' unexpected", p->getTagName()); return;} // FIXME error!
            p = p->getNextSibling();
        }
    }
}

void NEDDTDValidator::checkEmpty(NEDElement *node)
{
    if (node->getFirstChild()) {
        NEDError(node,"DTD validation error: EMPTY element has children\n"); // FIXME error!
    }
}

void NEDDTDValidator::checkRequiredAttribute(NEDElement *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    if (!s)
        {NEDError(node,"DTD validation error: required attribute %s is empty", attr); return;} // FIXME error!
}

void NEDDTDValidator::checkEnumeratedAttribute(NEDElement *node, const char *attr, const char *vals[], int n)
{
    // FIXME to be implemented
}

void NEDDTDValidator::checkNameAttribute(NEDElement *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    if (!s)
        return;
    if (!isalpha(*s) && *s!='_')
        {NEDError(node,"DTD validation error: attribute %s='%s' starts with invalid character", attr, node->getAttribute(attr)); return;} // FIXME error!
    while (*++s)
        if (!isalpha(*s) && !isdigit(*s) && *s!='_')
            {NEDError(node,"DTD validation error: attribute %s='%s' contains invalid character", attr, node->getAttribute(attr)); return;} // FIXME error!
}

void NEDDTDValidator::checkCommentAttribute(NEDElement *node, const char *attr)
{
    const char *s = node->getAttribute(attr);
    if (!s)
        return;
    bool incomment = false;
    for (; *s; s++)
    {
        if (!incomment)
        {
            if (*s=='/' && *(s+1)=='/')
                {s++; incomment = true;}
            else if (*s!=' ' && *s!='\t' && *s!='\n' && *s!='\r')
                {NEDError(node,"DTD validation error: attribute %s='%s' does not contain a valid NED comment", attr, node->getAttribute(attr)); return;} // FIXME error!
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
    const char *s = node->getAttribute(attr);
    if (!s)
        return;
    for (; *s; s++)
        if (!isalpha(*s) && !isdigit(*s) && !strchr("!#$%&()*+,-./:;<=>?@[\\]^_`{|}~",*s))  //FIXME check against XML spec!
            {NEDError(node,"DTD validation error: attribute %s='%s' contains invalid character", attr, node->getAttribute(attr)); return;} // FIXME error!
}



