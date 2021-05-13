//==========================================================================
//  NEDGENERATOR.H - part of
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

#ifndef __OMNETPP_NEDXML_NEDGENERATOR_H
#define __OMNETPP_NEDXML_NEDGENERATOR_H

#include <iostream>

#include "errorstore.h"
#include "nedelements.h"

namespace omnetpp {
namespace nedxml {


/**
 * @brief Simple front-end to NedGenerator.
 *
 * @ingroup NedGenerator
 */
NEDXML_API void generateNed(std::ostream& out, ASTNode *node);

/**
 * @brief Generates NED code from a NED AST.
 *
 * Assumes that the object tree has already passed all validation stages (DTD,
 * syntax, semantic).
 *
 * @ingroup NedGenerator
 */
class NEDXML_API NedGenerator
{
  protected:
    int indentSize = 4;
    std::ostream *outp = nullptr;

  public:
    /**
     * Constructor.
     */
    NedGenerator() {}

    /**
     * Destructor.
     */
    ~NedGenerator() {}

    /**
     * Sets the indent size in the generated NED code. Default is 4 spaces.
     */
    void setIndentSize(int indentsize);

    /**
     * Generates NED code. Takes an output stream where the generated NED code
     * will be written, the object tree and the base indentation.
     */
    void generate(std::ostream& out, ASTNode *node, const char *indent);

    /**
     * Generates NED code and returns it as a string.
     */
    std::string generate(ASTNode *node, const char *indent);

  protected:
    /** @name Change indentation level */
    //@{
    const char *increaseIndent(const char *indent);
    const char *decreaseIndent(const char *indent);
    //@}

    /** Dispatch to various doXXX() methods according to node type */
    void generateNedItem(ASTNode *node, const char *indent, bool islast, const char *arg=nullptr);;

    /** Invoke generateNedItem() on all children */
    void generateChildren(ASTNode *node, const char *indent, const char *arg=nullptr);

    /** Invoke generateNedItem() on all children of the given tagcode */
    void generateChildrenWithType(ASTNode *node, int tagcode, const char *indent, const char *arg=nullptr);

    /** Invoke generateNedItem() on children of the given tagcodes (NED_NULL-terminated array) */
    void generateChildrenWithTypes(ASTNode *node, int tagcodes[], const char *indent, const char *arg=nullptr);

    /** @name Utilities */
    //@{
    void printInheritance(ASTNode *node, const char *indent);
    bool hasExpression(ASTNode *node, const char *attr);
    void printExpression(ASTNode *node, const char *attr, const char *indent);
    void printOptVector(ASTNode *node, const char *attr, const char *indent);
    void doMsgClassOrStructBody(ASTNode *msgclassorstruct, const char *indent);
    //@}

    /** @name Getters for comments */
    //@{
    std::string concatInnerComments(ASTNode *node);
    std::string getBannerComment(ASTNode *node, const char *indent);
    std::string getRightComment(ASTNode *node);
    std::string getInlineRightComment(ASTNode *node);
    std::string getTrailingComment(ASTNode *node);
    //@}

    /** @name Generate NED code from the given element */
    //@{
    void doFiles(FilesElement *node, const char *indent, bool islast, const char *);
    void doNedFile(NedFileElement *node, const char *indent, bool islast, const char *);
    void doPackage(PackageElement *node, const char *indent, bool islast, const char *);
    void doImport(ImportElement *node, const char *indent, bool islast, const char *);
    void doPropertyDecl(PropertyDeclElement *node, const char *indent, bool islast, const char *);
    void doExtends(ExtendsElement *node, const char *indent, bool islast, const char *);
    void doInterfaceName(InterfaceNameElement *node, const char *indent, bool islast, const char *);
    void doSimpleModule(SimpleModuleElement *node, const char *indent, bool islast, const char *);
    void doModuleInterface(ModuleInterfaceElement *node, const char *indent, bool islast, const char *);
    void doCompoundModule(CompoundModuleElement *node, const char *indent, bool islast, const char *);
    void doChannelInterface(ChannelInterfaceElement *node, const char *indent, bool islast, const char *);
    void doChannel(ChannelElement *node, const char *indent, bool islast, const char *);
    void doParameters(ParametersElement *node, const char *indent, bool islast, const char *);
    void doParam(ParamElement *node, const char *indent, bool islast, const char *);
    void doProperty(PropertyElement *node, const char *indent, bool islast, const char *);
    void doPropertyKey(PropertyKeyElement *node, const char *indent, bool islast, const char *);
    void doGates(GatesElement *node, const char *indent, bool islast, const char *);
    void doGate(GateElement *node, const char *indent, bool islast, const char *);
    void doTypes(TypesElement *node, const char *indent, bool islast, const char *);
    void doSubmodules(SubmodulesElement *node, const char *indent, bool islast, const char *);
    void doSubmodule(SubmoduleElement *node, const char *indent, bool islast, const char *);
    void doConnections(ConnectionsElement *node, const char *indent, bool islast, const char *);
    void doConnection(ConnectionElement *node, const char *indent, bool islast, const char *);
    void doConnectionGroup(ConnectionGroupElement *node, const char *indent, bool islast, const char *);
    void doLoop(LoopElement *node, const char *indent, bool islast, const char *);
    void doCondition(ConditionElement *node, const char *indent, bool islast, const char *);
    void printConnectionGate(ASTNode *conn, const char *modname, const char *modindexattr,
                             const char *gatename, const char *gateindexattr, bool isplusplus,
                             int gatesubg, const char *indent);
    void doLiteral(LiteralElement *node, const char *indent, bool islast, const char *);
    void doComment(CommentElement *node, const char *indent, bool islast, const char *);
    //@}
};

}  // namespace nedxml
}  // namespace omnetpp


#endif


