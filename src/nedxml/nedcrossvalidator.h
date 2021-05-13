//==========================================================================
// nedcrossvalidator.h - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
// Contents:
//   class NedCrossValidator
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

class NedResourceCache;


// *** CURRENTLY NOT IN USE ***

/**
 * @brief Performs cross validation. Should be called after tree passed
 * DTD validation and syntax validation.
 *
 * @ingroup Validation
 */
class NEDXML_API NedCrossValidator : public NedValidatorBase
{
  protected:
    NedResourceCache *resolver;

    // temporary variables:
    ASTNode *moduleTypeDecl;

    ASTNode *findChildWithTagAndAttribute(ASTNode *node, int tag, const char *attr, const char *value);
    void checkGate(GateElement *gate, bool hasGateIndex, bool isInput, ASTNode *conn, bool isSrcGate);
    void validateConnGate(const char *submodName, bool hasSubmodIndex,
                          const char *gateName, bool hasGateIndex,
                          ASTNode *parent, ASTNode *conn, bool isSrc);

    // internal helper
    ASTNode *getXXXDeclaration(const char *name, int tagcode1, int tagcode2=-1);

    // these utility methods look up name in resolver, and add an error if the type doesn't match
    ASTNode *getModuleDeclaration(const char *name);
    ASTNode *getChannelDeclaration(const char *name);
    ASTNode *getModuleInterfaceDeclaration(const char *name);
    ASTNode *getChannelInterfaceDeclaration(const char *name);

  public:
    /** Constructor */
    NedCrossValidator(NedResourceCache *resolver, ErrorStore *e);

    /** Destructor */
    virtual ~NedCrossValidator() {}

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
    virtual void validateElement(LiteralElement *node) override;
    virtual void validateElement(UnknownElement *node) override;
    //@}
};

}  // namespace nedxml
}  // namespace omnetpp


#endif

