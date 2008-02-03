//==========================================================================
// nedsemanticvalidator.h - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDSemanticValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDSEMANTICVALIDATOR_H
#define __NEDSEMANTICVALIDATOR_H

#include <string.h>
#include "nedvalidator.h"

NAMESPACE_BEGIN

class NEDResourceCache;

/**
 * Performs semantic validation. Should be called after tree passed
 * DTD validation and syntax validation.
 *
 * @ingroup Validation
 */
class NEDXML_API NEDSemanticValidator : public NEDValidatorBase
{
  protected:
    bool parsedExpressions;
    NEDResourceCache *nedresourcecache;

    // temporary variables:
    NEDElement *moduletypedecl;

    NEDElement *findChildWithTagAndAttribute(NEDElement *node, int tag, const char *attr, const char *value);
    void checkGate(GateNode *gate, bool hasGateIndex, bool isInput, NEDElement *conn, bool isSrcGate);
    void validateConnGate(const char *submodName, bool hasSubmodIndex,
                          const char *gateName, bool hasGateIndex,
                          NEDElement *parent, NEDElement *conn, bool isSrc);

    // internal helper
    NEDElement *getXXXDeclaration(const char *name, int tagcode1, int tagcode2=-1);

    // these utility methods look up name in nedresourcecache, and add an error if the type doesn't match
    NEDElement *getModuleDeclaration(const char *name);
    NEDElement *getChannelDeclaration(const char *name);
    NEDElement *getModuleInterfaceDeclaration(const char *name);
    NEDElement *getChannelInterfaceDeclaration(const char *name);

    NEDElement *getEnumDeclaration(const char *name);
    NEDElement *getClassDeclaration(const char *name);

  public:
    /** Constructor */
    NEDSemanticValidator(bool parsedExpr, NEDResourceCache *nedcache, NEDErrorStore *e);

    /** Destructor */
    virtual ~NEDSemanticValidator();

  protected:
    /** @name Validator methods */
    //@{
    virtual void validateElement(FilesNode *node);
    virtual void validateElement(NedFileNode *node);
    virtual void validateElement(CommentNode *node);
    virtual void validateElement(ImportNode *node);
    virtual void validateElement(PropertyDeclNode *node);
    virtual void validateElement(ExtendsNode *node);
    virtual void validateElement(InterfaceNameNode *node);
    virtual void validateElement(SimpleModuleNode *node);
    virtual void validateElement(ModuleInterfaceNode *node);
    virtual void validateElement(CompoundModuleNode *node);
    virtual void validateElement(ChannelInterfaceNode *node);
    virtual void validateElement(ChannelNode *node);
    virtual void validateElement(ParametersNode *node);
    virtual void validateElement(ParamNode *node);
    virtual void validateElement(PatternNode *node);
    virtual void validateElement(PropertyNode *node);
    virtual void validateElement(PropertyKeyNode *node);
    virtual void validateElement(GatesNode *node);
    virtual void validateElement(GateNode *node);
    virtual void validateElement(TypesNode *node);
    virtual void validateElement(SubmodulesNode *node);
    virtual void validateElement(SubmoduleNode *node);
    virtual void validateElement(ConnectionsNode *node);
    virtual void validateElement(ConnectionNode *node);
    virtual void validateElement(ChannelSpecNode *node);
    virtual void validateElement(ConnectionGroupNode *node);
    virtual void validateElement(LoopNode *node);
    virtual void validateElement(ConditionNode *node);
    virtual void validateElement(ExpressionNode *node);
    virtual void validateElement(OperatorNode *node);
    virtual void validateElement(FunctionNode *node);
    virtual void validateElement(IdentNode *node);
    virtual void validateElement(LiteralNode *node);
    virtual void validateElement(MsgFileNode *node);
    virtual void validateElement(NamespaceNode *node);
    virtual void validateElement(CplusplusNode *node);
    virtual void validateElement(StructDeclNode *node);
    virtual void validateElement(ClassDeclNode *node);
    virtual void validateElement(MessageDeclNode *node);
    virtual void validateElement(EnumDeclNode *node);
    virtual void validateElement(EnumNode *node);
    virtual void validateElement(EnumFieldsNode *node);
    virtual void validateElement(EnumFieldNode *node);
    virtual void validateElement(MessageNode *node);
    virtual void validateElement(ClassNode *node);
    virtual void validateElement(StructNode *node);
    virtual void validateElement(FieldNode *node);
    virtual void validateElement(UnknownNode *node);
    //@}
};

NAMESPACE_END


#endif

