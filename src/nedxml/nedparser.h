//==========================================================================
//  NEDPARSER.H - part of
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
#define TYPE_XML       'X'
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
    bool parseexpr;            // whether to parse expressions or not
    bool storesrc;             // whether to fill in sourceCode attributes

    const char *filename;      // file being parsed
    int num_errors;            // number of errors so far

  public:
    /**
     * Constructor.
     */
    NEDParser();

    /**
     * Destructor.
     */
    ~NEDParser();

    /**
     * Affects operation of parseFile() and parseText(), specifies whether
     * expressions should be parsed or not.
     * Default is true.
     */
    void setParseExpressions(bool b) {parseexpr = b;}

    /**
     * Affects operation of parseFile() and parseText(), specifies whether
     * sourceCode attributes in NEDElements should be filled out.
     * Default is false.
     */
    void setStoreSource(bool b) {storesrc = b;}

    /**
     * Parse the given file. Result can be obtained from getTree().
     */
    bool parseFile(const char *fname);

    /**
     * Parse the given NED text. Result can be obtained from getTree().
     */
    bool parseText(const char *nedtext);

    /**
     * Returns the object tree which is the result of the parsing.
     * Further calls to getTree() result in NULL pointer to be returned.
     */
    NEDElement *getTree();

    // INTERNAL: error and debug handling, called from grammar file
    void error(const char *msg, int line);
    void dbg(YYLTYPE lc, const char *what);
};

#endif

