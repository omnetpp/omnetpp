//==========================================================================
//   NEDPARSER.H -
//            part of OMNeT++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDPARSER_H
#define __NEDPARSER_H

#include <stdio.h>

#include "nedgrammar.h"
#include "nedelement.h"

class NEDFileBuffer;
class NEDParser;
extern NEDParser *np;

/* to EXPR_TYPE: */
#define TYPE_NUMERIC   'N'
#define TYPE_CONST_NUM 'C'
#define TYPE_STRING    'S'
#define TYPE_BOOL      'B'
#define TYPE_ANYTYPE   'A'


/**
 * Parses NED files into a NED object tree.
 * Elements of the tree are subclassed from NEDElement;
 * NEDElementFactory is used to actually create the objects.
 * Internally this class uses a bison/flex grammar and NEDFileBuffer.
 *
 * @ingroup NEDParser
 */
class NEDParser
{
  public:
    NEDElement *tree;          // tree to build
    NEDFileBuffer *nedsource;  // NED File object

  protected:
    const char *filename;  // file being parsed
    int num_errors;        // number of errors so far

  public:
    NEDParser();
    ~NEDParser();

    // entry points:
    bool parseFile(const char *fname,bool parseexpr);
    bool parseText(const char *nedtext,bool parseexpr);

    NEDElement *getTree();

    // error and debug handling
    void error(const char *msg, int line);
    void dbg(YYLTYPE lc, const char *what);
};

#endif

