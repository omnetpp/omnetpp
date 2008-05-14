//==========================================================================
// nedcrossvalidator.h - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NEDCrossValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __NEDCROSSVALIDATOR_H
#define __NEDCROSSVALIDATOR_H

#include <string.h>
#include "nedvalidator.h"

NAMESPACE_BEGIN

class NEDResourceCache;

// *** CURRENTLY NOT IN USE ***

/**
 * Performs cross validation. Should be called after tree passed
 * DTD validation and syntax validation.
 *
 * @ingroup Validation
 */
class NEDXML_API NEDCrossValidator : public NEDValidatorBase
{
  protected:
    bool parsedExpressions;
    NEDResourceCache *resolver;

    // temporary variables:
    NEDElement *moduletypedecl;

    NEDElement *findChildWithTagAndAttribute(NEDElement *node, int tag, const char *attr, const char *value);
    void checkGate(GateElement *gate, bool hasGateIndex, bool isInput, NEDElement *conn, bool isSrcGate);
    void validateConnGate(const char *submodName, bool hasSubmodIndex,
                          const char *gateName, bool hasGateIndex,
                          NEDElement *parent, NEDElement *conn, bool isSrc);

    // internal helper
    NEDElement *getXXXDeclaration(const char *name, int tagcode1, int tagcode2=-1);

    // these utility methods look up name in resolver, and add an error if the type doesn't match
    NEDElement *getModuleDeclaration(const char *name);
    NEDElement *getChannelDeclaration(const char *name);
    NEDElement *getModuleInterfaceDeclaration(const char *name);
    NEDElement *getChannelInterfaceDeclaration(const char *name);

    NEDElement *getEnumDeclaration(const char *name);
    NEDElement *getClassDeclaration(const char *name);

  public:
    /** Constructor */
    NEDCrossValidator(bool parsedExpr, NEDResourceCache *resolver, NEDErrorStore *e);

    /** Destructor */
    virtual ~NEDCrossValidator();

  protected:
    /** @name Validator methods */
    //@{
    virtual void validateElement(FilesElement *node);
    virtual void validateElement(NedFileElement *node);
    virtual void validateElement(CommentElement *node);
    virtual void validateElement(ImportElement *node);
    virtual void validateElement(PropertyDeclElement *node);
    virtual void validateElement(ExtendsElement *node);
    virtual void validateElement(InterfaceNameElement *node);
    virtual void validateElement(SimpleModuleElement *node);
    virtual void validateElement(ModuleInterfaceElement *node);
    virtual void validateElement(CompoundModuleElement *node);
    virtual void validateElement(ChannelInterfaceElement *node);
    virtual void validateElement(ChannelElement *node);
    virtual void validateElement(ParametersElement *node);
    virtual void validateElement(ParamElement *node);
    virtual void validateElement(PatternElement *node);
    virtual void validateElement(PropertyElement *node);
    virtual void validateElement(PropertyKeyElement *node);
    virtual void validateElement(GatesElement *node);
    virtual void validateElement(GateElement *node);
    virtual void validateElement(TypesElement *node);
    virtual void validateElement(SubmodulesElement *node);
    virtual void validateElement(SubmoduleElement *node);
    virtual void validateElement(ConnectionsElement *node);
    virtual void validateElement(ConnectionElement *node);
    virtual void validateElement(ChannelSpecElement *node);
    virtual void validateElement(ConnectionGroupElement *node);
    virtual void validateElement(LoopElement *node);
    virtual void validateElement(ConditionElement *node);
    virtual void validateElement(ExpressionElement *node);
    virtual void validateElement(OperatorElement *node);
    virtual void validateElement(FunctionElement *node);
    virtual void validateElement(IdentElement *node);
    virtual void validateElement(LiteralElement *node);
    virtual void validateElement(MsgFileElement *node);
    virtual void validateElement(NamespaceElement *node);
    virtual void validateElement(CplusplusElement *node);
    virtual void validateElement(StructDeclElement *node);
    virtual void validateElement(ClassDeclElement *node);
    virtual void validateElement(MessageDeclElement *node);
    virtual void validateElement(EnumDeclElement *node);
    virtual void validateElement(EnumElement *node);
    virtual void validateElement(EnumFieldsElement *node);
    virtual void validateElement(EnumFieldElement *node);
    virtual void validateElement(MessageElement *node);
    virtual void validateElement(ClassElement *node);
    virtual void validateElement(StructElement *node);
    virtual void validateElement(FieldElement *node);
    virtual void validateElement(UnknownElement *node);
    //@}
};

NAMESPACE_END


#endif

