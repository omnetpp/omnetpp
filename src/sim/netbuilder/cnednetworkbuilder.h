//==========================================================================
//  CNEDNETWORKBUILDER.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `terms' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CNETWORKBUILDER_H
#define __OMNETPP_CNETWORKBUILDER_H

#include <string>
#include <map>
#include <vector>
#include "nedxml/nedelements.h"
#include "nedxml/nedresourcecache.h"
#include "omnetpp/globals.h"
#include "omnetpp/cpar.h"
#include "cneddeclaration.h"
#include "cnedloader.h"

namespace omnetpp {

using namespace omnetpp::nedxml;

class cModule;
class cGate;
class cChannel;

namespace internal { class cParImpl; class cObjectParImpl; class cIntParImpl; };

#define MAX_LOOP_NESTING 32


/**
 * @brief Stateless object to assist in building the network, based on NED files.
 *
 * @ingroup NetworkBuilder
 */
class SIM_API cNedNetworkBuilder
{
  protected:
    class ComponentTypeNames : public NedResourceCache::INedTypeNames {
      public:
        virtual bool contains(const char *qname) const override  {return omnetpp::internal::componentTypes.getInstance()->lookup(qname)!=nullptr;}
        virtual int size() const override  {return omnetpp::internal::componentTypes.getInstance()->size();}
        virtual const char *get(int k) const override  {return omnetpp::internal::componentTypes.getInstance()->get(k)->getFullName();}
    };

    typedef cNedDeclaration::PatternData PatternData;  // abbreviation

  protected:
    typedef internal::cParImpl cParImpl;
    typedef internal::cIntParImpl cIntParImpl;
    typedef internal::cObjectParImpl cObjectParImpl;

    // the current NED declaration we're working with. Stored here to
    // avoid having to pass it around as a parameter.
    cNedDeclaration *currentDecl = nullptr;

  protected:
    void addSubmodulesAndConnections(cModule *modp);
    bool superTypeAllowsUnconnected(cNedDeclaration *decl) const;
    void buildRecursively(cModule *modp, cNedDeclaration *decl);
    std::string resolveComponentType(const NedLookupContext& context, const char *nedTypeName);
    cModuleType *findAndCheckModuleType(const char *modtypename, cModule *modp, const char *submodName);
    cModuleType *findAndCheckModuleTypeLike(const char *modTypeName, const char *likeType, cModule *modp, const char *submodName);
    std::vector<std::string> findTypeWithInterface(const char *nedTypeName, const char *interfaceQName);

    std::string getSubmoduleTypeName(cModule *modp, SubmoduleElement *submod, int index = -1);
    bool getSubmoduleOrChannelTypeNameFromDeepAssignments(cModule *modp, const std::string& submodOrChannelKey, std::string& outTypeName, bool& outIsDefault);
    void addSubmodule(cModule *modp, SubmoduleElement *submod);
    void doAddParametersAndGatesTo(cComponent *component, cNedDeclaration *decl);
    void doAssignParametersFromPatterns(cComponent *component, const std::string& prefix, const std::vector<PatternData>& patterns, cComponent *evalContext);
    void doAssignParameterFromPattern(cPar& par, ParamElement *patternNode, cComponent *evalContext);
    static cPar::Type translateParamType(int t);
    static cGate::Type translateGateType(int t);
    void doParams(cComponent *component, ParametersElement *paramsNode, bool isSubcomponent, bool isParametricSubcomponent);
    void doParam(cComponent *component, ParamElement *paramNode, bool isSubcomponent, bool isParametricSubcomponent);
    void doGates(cModule *component, GatesElement *gatesNode);
    void doGate(cModule *component, GateElement *gateNode);
    void doGateSizes(cModule *component, GatesElement *gatesNode);
    void doGateSize(cModule *component, GateElement *gateNode);
    void assignSubcomponentParams(cComponent *subcomponent, NedElement *subcomponentNode);
    void setupSubmoduleGateVectors(cModule *submodule, NedElement *submoduleNode);

    void addConnectionOrConnectionGroup(cModule *modp, NedElement *connOrConnGroup);
    void doConnOrConnGroupBody(cModule *modp, NedElement *connOrConnGroup, NedElement *loopOrCondition);
    void doLoopOrCondition(cModule *modp, NedElement *loopOrCondition);
    void doAddConnOrConnGroup(cModule *modp, NedElement *connOrConnGroup);
    void doAddConnection(cModule *modp, ConnectionElement *conn);
    void doConnectGates(cModule *modp, cGate *srcg, cGate *destg, ConnectionElement *conn);
    cGate *resolveGate(cModule *parentmodp, const char *modname, const ExprRef& modindexp,
                       const char *gatename, const ExprRef& gateindexp, int subg, bool isplusplus);
    void resolveInoutGate(cModule *parentmodp, const char *modname, const ExprRef& modindexp,
                       const char *gatename, const ExprRef& gateindexp, bool isplusplus,
                       cGate *&gate1, cGate *&gate2);
    cModule *resolveModuleForConnection(cModule *parentmodp, const char *modname, const ExprRef& modindexp);
    std::string getChannelTypeName(cModule *modp, cGate *srcgate, ConnectionElement *conn, const char *channelName);
    cChannel *createChannel(ConnectionElement *conn, cModule *parentmodp, cGate *srcgate);

    cChannelType *findAndCheckChannelType(const char *channelTypeName, cModule *modp);
    cChannelType *findAndCheckChannelTypeLike(const char *channelTypeName, const char *likeType, cModule *modp, int connectionId);
    cDynamicExpression *getOrCreateExpression(const ExprRef& exprRef);
    long evaluateAsLong(const ExprRef& exprRef, cComponent *contextComponent);
    bool evaluateAsBool(const ExprRef& exprRef, cComponent *contextComponent);
    std::string evaluateAsString(const ExprRef& exprRef, cComponent *contextComponent);
    long evaluateAsLong(const ExprRef& exprRef, cExpression::Context *context);
    bool evaluateAsBool(const ExprRef& exprRef, cExpression::Context *context);
    std::string evaluateAsString(const ExprRef& exprRef, cExpression::Context *context);
    bool getBooleanProperty(NedElement *componentNode, const char *name);

  public:
    /** Constructor */
    cNedNetworkBuilder() {}

    /**
     * Adds parameters and gates from the given NED declaration. Gate vectors
     * will be created with zero size, and a further call to setupGateVectors()
     * will be needed once parameter values have been finalized. (This method is
     * also used with channels, with the "gates" part being a no-op then.)
     * Invoked from cDynamicModule.
     */
    void addParametersAndGatesTo(cComponent *component, cNedDeclaration *decl);

    /**
     * Applies NED pattern (a.k.a. deep) parameter assignments to parameters of
     * the component.
     */
    void assignParametersFromPatterns(cComponent *component);

    /**
     * Sets gate vector sizes on the module, using the given NED declaration.
     * This should be called AFTER all parameters have been set, because gate
     * vector sizes may depend on parameter values. Invoked from cDynamicModule.
     */
    void setupGateVectors(cModule *module, cNedDeclaration *decl);

    /**
     * Builds submodules and internal connections, based on the info in the
     * passed NedElement tree. Invoked from cDynamicModule.
     */
    void buildInside(cModule *module, cNedDeclaration *decl);
};

}  // namespace omnetpp


#endif





