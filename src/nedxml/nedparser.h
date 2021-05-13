//==========================================================================
//  NEDPARSER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_NEDPARSER_H
#define __OMNETPP_NEDXML_NEDPARSER_H

#include <cstdio>
#include "yydefs.h"

namespace omnetpp {
namespace nedxml {

class ASTNode;
class ErrorStore;
class SourceDocument;

/* to EXPR_TYPE: */
#define TYPE_NUMERIC   'N'
#define TYPE_CONST_NUM 'C'
#define TYPE_STRING    'S'
#define TYPE_BOOL      'B'
#define TYPE_XML       'X'
#define TYPE_ANYTYPE   'A'


/**
 * @brief Parses NED files into an AST.
 *
 * Elements of the tree are subclassed from ASTNode;
 * ASTNodeFactory is used to actually create the objects.
 * Internally this class uses a bison/flex grammar and SourceDocument.
 *
 * @ingroup MsgParser
 */
class NEDXML_API NedParser
{
  public:
    ParseContext np;

  protected:
    bool loadFile(const char *osfname, const char *fname);
    bool loadText(const char *nedtext, const char *fname);
    ASTNode *parseNed();

  public:
    /**
     * Constructor.
     */
    NedParser(ErrorStore *e);

    /**
     * Destructor.
     */
    ~NedParser();

    /**
     * Returns a NED source which contains declarations of
     * built-in NED types.
     */
    static const char *getBuiltInDeclarations();

    /**
     * Affects operation of parseFile() and parseText(), specifies whether
     * sourceCode attributes in ASTNodes should be filled out.
     * Default is false.
     */
    void setStoreSource(bool b) {np.storesrc = b;}

    /**
     * Returns the "store source code" flag; see setStoreSource().
     */
    bool getStoreSourceFlag() {return np.storesrc;}

    /**
     * Parses the given NED file (osfname), and returns the result tree.
     * Returns nullptr or partial tree if there was an error.
     * The fname parameter will be used to fill in the source location
     * attributes; it defaults to osfname.
     */
    ASTNode *parseNedFile(const char *osfname, const char *fname=nullptr);

    /**
     * Parse the given NED source and return the result tree.
     * Returns nullptr or partial tree if there was an error.
     * The fname parameter will be used to fill in the source location
     * attributes; it defaults to "buffer".
     */
    ASTNode *parseNedText(const char *nedtext, const char *fname=nullptr);

    /**
     * Returns true if the given string is a syntactically valid NED expression
     * (corresponds to the grammar), and false otherwise.
     */
    bool isValidNedExpression(const char *expr);
};

}  // namespace nedxml
}  // namespace omnetpp


#endif

