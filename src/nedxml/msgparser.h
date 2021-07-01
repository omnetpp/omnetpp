//==========================================================================
//  MSGPARSER.H - part of
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

#ifndef __OMNETPP_NEDXML_MSGPARSER_H
#define __OMNETPP_NEDXML_MSGPARSER_H

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
 * @brief Parses MSG files into an AST.
 *
 * Elements of the tree are subclassed from ASTNode;
 * ASTNodeFactory is used to actually create the objects.
 * Internally this class uses a bison/flex grammar and SourceDocument.
 *
 * @ingroup MsgParser
 */
class NEDXML_API MsgParser
{
  public:
    ParseContext np;

  protected:
    bool loadFile(const char *osfname, const char *fname);
    bool loadText(const char *nedtext, const char *fname);
    ASTNode *parseMsg();

  public:
    /**
     * Constructor.
     */
    MsgParser(ErrorStore *e);

    /**
     * Destructor.
     */
    ~MsgParser();

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
     * Parse the given MSG (.msg) file and return the result tree.
     * Returns nullptr or partial tree if there was an error.
     * The fname parameter will be used to fill in the source location
     * attributes; it defaults to osfname.
     */
    ASTNode *parseMsgFile(const char *osfname, const char *fname=nullptr);

    /**
     * Parse the given MSG source and return the result tree.
     * Returns nullptr or partial tree if there was an error.
     * The fname parameter will be used to fill in the source location
     * attributes; it defaults to osfname.
     */
    ASTNode *parseMsgText(const char *text, const char *fname=nullptr);
};

}  // namespace nedxml
}  // namespace omnetpp


#endif

