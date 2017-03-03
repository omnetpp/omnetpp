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
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_NEDXML_NEDCROSSVALIDATOR_H
#define __OMNETPP_NEDXML_NEDCROSSVALIDATOR_H

#include <cstring>
#include "nedvalidator.h"

namespace omnetpp {
namespace nedxml {

class NEDResourceCache;

// *** CURRENTLY NOT IN USE ***

/**
 * @brief Performs cross validation. Should be called after tree passed
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
    NEDElement *moduleTypeDecl;

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
    virtual void validateElement(FilesElement *node) override;
    virtual void validateElement(NedFileElement *node) override;
    virtual void validateElement(CommentElement *node) override;
    virtual void validateElement(ImportElement *node) override;
    virtual void validateElement(PropertyDeclElement *node) override;
    virtual void validateElement(ExtendsElement *node) override;
    virtual void validateElement(InterfaceNameElement *node) override;
    virtual void validateElement(SimpleModuleElement *node) override;
    virtual void validateElement(ModuleInterfaceElement *node) override;
    virtual void validateElement(CompoundModuleElement *node) override;
    virtual void validateElement(ChannelInterfaceElement *node) override;
    virtual void validateElement(ChannelElement *node) override;
    virtual void validateElement(ParametersElement *node) override;
    virtual void validateElement(ParamElement *node) override;
    virtual void validateElement(PropertyElement *node) override;
    virtual void validateElement(PropertyKeyElement *node) override;
    virtual void validateElement(GatesElement *node) override;
    virtual void validateElement(GateElement *node) override;
    virtual void validateElement(TypesElement *node) override;
    virtual void validateElement(SubmodulesElement *node) override;
    virtual void validateElement(SubmoduleElement *node) override;
    virtual void validateElement(ConnectionsElement *node) override;
    virtual void validateElement(ConnectionElement *node) override;
    virtual void validateElement(ConnectionGroupElement *node) override;
    virtual void validateElement(LoopElement *node) override;
    virtual void validateElement(ConditionElement *node) override;
    virtual void validateElement(ExpressionElement *node) override;
    virtual void validateElement(OperatorElement *node) override;
    virtual void validateElement(FunctionElement *node) override;
    virtual void validateElement(IdentElement *node) override;
    virtual void validateElement(LiteralElement *node) override;
    virtual void validateElement(MsgFileElement *node) override;
    virtual void validateElement(NamespaceElement *node) override;
    virtual void validateElement(CplusplusElement *node) override;
    virtual void validateElement(StructDeclElement *node) override;
    virtual void validateElement(ClassDeclElement *node) override;
    virtual void validateElement(MessageDeclElement *node) override;
    virtual void validateElement(PacketDeclElement *node) override;
    virtual void validateElement(EnumDeclElement *node) override;
    virtual void validateElement(EnumElement *node) override;
    virtual void validateElement(EnumFieldsElement *node) override;
    virtual void validateElement(EnumFieldElement *node) override;
    virtual void validateElement(MessageElement *node) override;
    virtual void validateElement(PacketElement *node) override;
    virtual void validateElement(ClassElement *node) override;
    virtual void validateElement(StructElement *node) override;
    virtual void validateElement(FieldElement *node) override;
    virtual void validateElement(UnknownElement *node) override;
    //@}
};

} // namespace nedxml
}  // namespace omnetpp


#endif

