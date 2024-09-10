//==========================================================================
//  CNULLENVIR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CNULLENVIR_H
#define __OMNETPP_CNULLENVIR_H

#include "cenvir.h"
#include "cexception.h"
#include "ccomponent.h"
#include "cmersennetwister.h"

namespace omnetpp {


/**
 * @brief A do-nothing cEnvir implementation.
 *
 * This class defines cEnvir's pure virtual methods with an empty body,
 * with a trivial implementation, or to throw an exception with an
 * "unsupported method" message.
 *
 * This class is not used by \opp, it is provided for convenience of writing
 * standalone programs that embed the simulation kernel as a library.
 *
 * You may want to subclass from cNullEnvir, and redefine some the following
 * methods:
 *  - log() to print log messages instead of discarding them;
 *  - readParameter() to set module parameters, or at least apply the
 *    NED default value;
 *  - the RNG methods (getNumRNGs(), getRNG()) to provide more than one
 *    random number generator
 *  - recordScalar() and other statistics recording methods to store
 *    simulation results instead of discarding them
 *
 * @ingroup EnvirAndExtensions
 */
class SIM_API cNullEnvir : public cEnvir
{
  protected:
    int argc;
    char **argv;
    cConfiguration *cfg;
    cRNG *rng;
    unsigned long nextUniqueNumber = 0;
    std::vector<cISimulationLifecycleListener*> listeners;

  protected:
    void unsupported() const {throw cRuntimeError("cNullEnvir: Unsupported method called");}
    virtual void alert(const char *msg) override {::printf("\n<!> %s\n\n", msg);}
    virtual bool askYesNo(const char *prompt) override  {unsupported(); return false;}
    virtual std::ostream& getOutputStream() override {return std::cout;}
    virtual void pausePoint() override {}

  public:
    // constructor, destructor
    cNullEnvir(int ac, char **av, cConfiguration *c);
    virtual ~cNullEnvir();

    // eventlog callback interface
    virtual void objectDeleted(cObject *object) override {}
    virtual void simulationEvent(cEvent *event) override  {}
    virtual void messageScheduled(cMessage *msg) override  {}
    virtual void messageCancelled(cMessage *msg) override  {}
    virtual void beginSend(cMessage *msg, const SendOptions& options) override  {}
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result) override  {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) override  {}
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result) override  {}
    virtual void endSend(cMessage *msg) override  {}
    virtual void messageCreated(cMessage *msg) override  {}
    virtual void messageCloned(cMessage *msg, cMessage *clone) override  {}
    virtual void messageDeleted(cMessage *msg) override  {}
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) override  {}
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent) override  {}
    virtual void componentMethodEnd() override  {}
    virtual void moduleCreated(cModule *newmodule) override  {}
    virtual void moduleDeleted(cModule *module) override  {}
    virtual void gateCreated(cGate *newgate) override  {}
    virtual void gateDeleted(cGate *gate) override  {}
    virtual void connectionCreated(cGate *srcgate) override  {}
    virtual void connectionDeleted(cGate *srcgate) override  {}
    virtual void displayStringChanged(cComponent *component) override  {}
    virtual void undisposedObject(cObject *obj) override {}
    virtual void log(cLogEntry *entry) override {}

    // configuration, model parameters
    virtual void preconfigure(cComponent *component) override  {}
    virtual void configure(cComponent *component) override {}
    virtual void readParameter(cPar *par) override  {unsupported();}
    virtual bool isModuleLocal(cModule *parentmod, const char *modname, int index) override  {return true;}
    virtual cXMLElement *getXMLDocument(const char *filename, const char *xpath=nullptr) override  {unsupported(); return nullptr;}
    virtual cXMLElement *getParsedXMLString(const char *content, const char *xpath=nullptr) override  {unsupported(); return nullptr;}
    virtual void forgetXMLDocument(const char *filename) override {}
    virtual void forgetParsedXMLString(const char *content) override {}
    virtual void flushXMLDocumentCache() override {}
    virtual void flushXMLParsedContentCache() override {}
    virtual unsigned getExtraStackForEnvir() const override  {return 0;}
    virtual cConfiguration *getConfig() override  {return cfg;}
    virtual std::string resolveResourcePath(const char *fileName, cComponentType *context) override {return fileName;}
    virtual bool isGUI() const override  {return false;}
    virtual bool isExpressMode() const override {return false;}

    virtual void getImageSize(const char *imageName, double& outWidth, double& outHeight) override {unsupported();}
    virtual void getTextExtent(const cFigure::Font& font, const char *text, double& outWidth, double& outHeight, double& outAscent) override {unsupported();}
    virtual void appendToImagePath(const char *directory) override {unsupported();}
    virtual void loadImage(const char *fileName, const char *imageName=nullptr) override {unsupported();}
    virtual cFigure::Rectangle getSubmoduleBounds(const cModule *submodule) override {return cFigure::Rectangle(NAN, NAN, NAN, NAN);}
    virtual std::vector<cFigure::Point> getConnectionLine(const cGate *sourceGate) override {return {};}
    virtual double getZoomLevel(const cModule *module) override {return NAN;}
    virtual double getAnimationTime() const override {return 0;}
    virtual double getAnimationSpeed() const override {return 0;}
    virtual double getRemainingAnimationHoldTime() const override {return 0;}

    // UI functions (see also protected ones)
    virtual void bubble(cComponent *component, const char *text) override  {}
    virtual std::string gets(const char *prompt, const char *defaultreply=nullptr) override  {unsupported(); return "";}
    virtual cEnvir& flush()  {::fflush(stdout); return *this;}

    // RNGs
    virtual int getNumRNGs() const override {return 1;}
    virtual cRNG *getRNG(int k) override  {return rng;}

    // output vectors
    virtual void *registerOutputVector(const char *modulename, const char *vectorname, opp_string_map *attributes=nullptr) override  {return (void *)1;}
    virtual void deregisterOutputVector(void *vechandle) override  {}
    virtual bool recordInOutputVector(void *vechandle, simtime_t t, double value) override  {return false;}

    // output scalars
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) override  {}
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) override  {}
    virtual void recordParameter(cPar *par) override {}
    virtual void recordComponentType(cComponent *component) override {}
    virtual void addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty) override {}

    // snapshot file
    virtual std::ostream *getStreamForSnapshot() override  {unsupported(); return nullptr;}
    virtual void releaseStreamForSnapshot(std::ostream *os) override  {unsupported();}

    // misc
    virtual int getArgCount() const override  {return argc;}
    virtual char **getArgVector() const override  {return argv;}
    virtual int getParsimProcId() const override {return 0;}
    virtual int getParsimNumPartitions() const override {return 1;}
    virtual unsigned long getUniqueNumber() override  {return nextUniqueNumber++;}
    virtual void refOsgNode(osg::Node *scene) override {}
    virtual void unrefOsgNode(osg::Node *scene) override {}
    virtual bool idle() override  {return false;}
    virtual bool ensureDebugger(cRuntimeError *) override { return false; }
};


}  // namespace omnetpp

#endif


