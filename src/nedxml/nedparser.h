//==========================================================================
//  NEDPARSER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDPARSER_H
#define __NEDPARSER_H

#include <stdio.h>
#include "nedelement.h"
#include "nedyydefs.h"
#include "nederror.h"

NAMESPACE_BEGIN

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
class NEDXML_API NEDParser
{
  public:
    // INTERNAL: error and debug handling, called from grammar file
    void error(const char *msg, int line);

    NEDErrorStore *getErrors() {return errors;}
    NEDFileBuffer *getSource() {return nedsource;}

  protected:
    bool parseexpr;            // whether to parse expressions or not
    bool storesrc;             // whether to fill in sourceCode attributes
    const char *filename;      // file being parsed
    NEDErrorStore *errors;     // accumulates error messages
    NEDFileBuffer *nedsource;  // represents the source file

    bool loadFile(const char *fname);
    bool loadText(const char *nedtext, const char *fname);
    NEDElement *parseNED();
    NEDElement *parseMSG();
    static bool guessIsNEDInNewSyntax(const char *txt);

  public:
    /**
     * Constructor.
     */
    NEDParser(NEDErrorStore *e);

    /**
     * Destructor.
     */
    ~NEDParser();

    /**
     * Returns a NED source which contains declarations of
     * built-in NED types.
     */
    static const char *getBuiltInDeclarations();

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
     * Returns the "parse expressions" flag; see setParseExpressions().
     */
    bool getParseExpressionsFlag() {return parseexpr;}

    /**
     * Returns the "store source code" flag; see setStoreSource().
     */
    bool getStoreSourceFlag() {return storesrc;}

    /**
     * May only be called during parsing. It returns the name of the source file being parsed.
     */
    const char *getFileName() {return filename;}

    /**
     * Parse the given NED file and return the result tree.
     * Returns NULL or partial tree if there was an error.
     */
    NEDElement *parseNEDFile(const char *fname);

    /**
     * Parse the given NED source and return the result tree.
     * Returns NULL or partial tree if there was an error.
     * Filename will be used to fill in the source location attribute.
     */
    NEDElement *parseNEDText(const char *nedtext, const char *fname);

    /**
     * Parse the given text as a NED expression, and return the result tree.
     * Returns NULL or partial tree if there was an error.
     */
    NEDElement *parseNEDExpression(const char *nedexpression);

    /**
     * Parse the given MSG (.msg) file and return the result tree.
     * Returns NULL or partial tree if there was an error.
     */
    NEDElement *parseMSGFile(const char *fname);

    /**
     * Parse the given MSG source and return the result tree.
     * Returns NULL or partial tree if there was an error.
     * Filename will be used to fill in the source location attribute.
     */
    NEDElement *parseMSGText(const char *nedtext, const char *fname);
};

NAMESPACE_END


#endif

