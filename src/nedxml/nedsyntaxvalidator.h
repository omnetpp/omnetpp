//==========================================================================
// nedsyntaxvalidator.h -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NedSyntaxValidator
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __OMNETPP_NEDXML_NEDSYNTAXVALIDATOR_H
#define __OMNETPP_NEDXML_NEDSYNTAXVALIDATOR_H

#include "nedvalidator.h"

namespace omnetpp {
namespace nedxml {

/**
 * @brief Performs structural validation of components.
 *
 * It assumes that the tree already passed DTD validation. Every compound
 * module and other component is viewed in isolation: syntax validation
 * does NOT process imports or examines validity of references to other
 * module types, channel types, etc.
 *
 * @ingroup Validation
 */
class NEDXML_API NedSyntaxValidator : public NedValidatorBase
{
  protected:
    // internal helpers: verify attributes
    void checkExpressionAttributes(ASTNode *node, const char *attrs[], bool optional[], int n);
    void checkEnumAttribute(ASTNode *node, const char *attr, const char *values[], int n);
    void checkDottedNameAttribute(ASTNode *node, const char *attr, bool wildcardsAllowed);
    void checkPropertyNameAttribute(ASTNode *node, const char *attr);
    void checkPropertyIndexAttribute(ASTNode *node, const char *attr);

    // helper: whether the given node is inside the BODY of a submodule or a connection
    bool isWithinSubcomponent(ASTNode *node);

    // whether the given node is inside an inner type
    bool isWithinInnerType(ASTNode *node);

  public:
    /**
     * Constructor.
     */
    NedSyntaxValidator(ErrorStore *e) : NedValidatorBase(e) {}

    /**
     * Destructor.
     */
    virtual ~NedSyntaxValidator() {}

  protected:
    /** @name Validator methods */
    //@{
    virtual void validateElement(FilesElement *node) override;
    virtual void validateElement(NedFileElement *node) override;
    virtual void validateElement(CommentElement *node) override;
    virtual void validateElement(PackageElement *node) override;
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
    virtual void validateElement(LiteralElement *node) override;
    virtual void validateElement(UnknownElement *node) override;
    //@}
};

}  // namespace nedxml
}  // namespace omnetpp


#endif

