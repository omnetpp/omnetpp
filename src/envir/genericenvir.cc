//==========================================================================
//  GENERICENVIR.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "genericenvir.h"

#include <cstdio>
#include <csignal>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>
#include "common/stringtokenizer.h"
#include "common/stringutil.h"
#include "common/fileutil.h"
#include "omnetpp/ccoroutine.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/platdep/platmisc.h"
#include "omnetpp/cstatisticbuilder.h"
#include "args.h"
#include "xmldoccache.h"

#ifdef PREFER_SQLITE_RESULT_FILES
#define DEFAULT_OUTPUTVECTORMANAGER_CLASS "omnetpp::envir::SqliteOutputVectorManager"
#define DEFAULT_OUTPUTSCALARMANAGER_CLASS "omnetpp::envir::SqliteOutputScalarManager"
#else
#define DEFAULT_OUTPUTVECTORMANAGER_CLASS "omnetpp::envir::OmnetppOutputVectorManager"
#define DEFAULT_OUTPUTSCALARMANAGER_CLASS "omnetpp::envir::OmnetppOutputScalarManager"
#endif

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

using std::ostream;

Register_GlobalConfigOption(CFGID_WARNINGS, "warnings", CFG_BOOL, "true", "Enables warnings.");  // note: currently unused
Register_GlobalConfigOption(CFGID_EVENTLOGMANAGER_CLASS, "eventlogmanager-class", CFG_STRING, "omnetpp::envir::EventlogFileManager", "Part of the Envir plugin mechanism: selects the eventlog manager class to be used to record data. The class has to implement the `cIEventlogManager` interface.");
Register_GlobalConfigOption(CFGID_OUTPUTVECTORMANAGER_CLASS, "outputvectormanager-class", CFG_STRING, DEFAULT_OUTPUTVECTORMANAGER_CLASS, "Part of the Envir plugin mechanism: selects the output vector manager class to be used to record data from output vectors. The class has to implement the `cIOutputVectorManager` interface.");
Register_GlobalConfigOption(CFGID_OUTPUTSCALARMANAGER_CLASS, "outputscalarmanager-class", CFG_STRING, DEFAULT_OUTPUTSCALARMANAGER_CLASS, "Part of the Envir plugin mechanism: selects the output scalar manager class to be used to record data passed to recordScalar(). The class has to implement the `cIOutputScalarManager` interface.");
Register_GlobalConfigOption(CFGID_SNAPSHOTMANAGER_CLASS, "snapshotmanager-class", CFG_STRING, "omnetpp::envir::FileSnapshotManager", "Part of the Envir plugin mechanism: selects the class to handle streams to which snapshot() writes its output. The class has to implement the `cISnapshotManager` interface.");
Register_GlobalConfigOption(CFGID_IMAGE_PATH, "image-path", CFG_PATH, "./images", "A semicolon-separated list of directories that contain module icons and other resources. This list will be concatenated with the contents of the `OMNETPP_IMAGE_PATH` environment variable or with a compile-time, hardcoded image path if the environment variable is empty.");
Register_GlobalConfigOption(CFGID_DEBUG_ON_ERRORS, "debug-on-errors", CFG_BOOL, "false", "When set to true, runtime errors will cause the simulation program to break into the C++ debugger (if the simulation is running under one, or just-in-time debugging is activated). Once in the debugger, you can view the stack trace or examine variables.");
Register_GlobalConfigOption(CFGID_PRINT_UNDISPOSED, "print-undisposed", CFG_BOOL, "true", "Whether to report objects left (that is, not deallocated by simple module destructors) after network cleanup.");
Register_GlobalConfigOption(CFGID_DEBUGGER_ATTACH_ON_ERROR, "debugger-attach-on-error", CFG_BOOL, "false", "When set to true, runtime errors and crashes will trigger an external debugger to be launched (if not already present), allowing you to perform just-in-time debugging on the simulation process. The debugger command is configurable. Note that debugging (i.e. attaching to) a non-child process needs to be explicitly enabled on some systems, e.g. Ubuntu.");
Register_GlobalConfigOption(CFGID_RECORD_EVENTLOG, "record-eventlog", CFG_BOOL, "false", "Enables recording an eventlog file, which can be later visualized on a sequence chart. See `eventlog-file` option too.");

static cEnvir *f() { return new GenericEnvir(); }
EXECUTE_ON_STARTUP(cSimulation::setEnvirFactoryFunction(f));

GenericEnvir::GenericEnvir()
{
}

GenericEnvir::~GenericEnvir()
{
    delete xmlCache;
    delete eventlogManager;
    delete outVectorManager;
    delete outScalarManager;
    delete snapshotManager;
}

void GenericEnvir::setSimulation(cSimulation *simulation)
{
    cEnvir::setSimulation(simulation);
    simulation->addLifecycleListener(this);
}

void GenericEnvir::configure(cConfiguration *cfg)
{
    this->cfg = cfg;

    setAttachDebuggerOnErrors(cfg->getAsBool(CFGID_DEBUGGER_ATTACH_ON_ERROR));

    xmlCache = new XMLDocCache();

    // install eventlog manager
    std::string eventlogManagerClass = cfg->getAsString(CFGID_EVENTLOGMANAGER_CLASS);
    cIEventlogManager *eventlogManager = createByClassName<cIEventlogManager>(eventlogManagerClass.c_str(), "eventlog manager");
    setEventlogManager(eventlogManager);

    // install output vector manager
    std::string outputVectorManagerClass = cfg->getAsString(CFGID_OUTPUTVECTORMANAGER_CLASS);
    cIOutputVectorManager *outVectorManager = createByClassName<cIOutputVectorManager>(outputVectorManagerClass.c_str(), "output vector manager");
    setOutVectorManager(outVectorManager);

    // install output scalar manager
    std::string outputScalarManagerClass = cfg->getAsString(CFGID_OUTPUTSCALARMANAGER_CLASS);
    cIOutputScalarManager *outScalarManager = createByClassName<cIOutputScalarManager>(outputScalarManagerClass.c_str(), "output scalar manager");
    setOutScalarManager(outScalarManager);

    // install snapshot manager
    std::string snapshotmanagerClass = cfg->getAsString(CFGID_SNAPSHOTMANAGER_CLASS);
    cISnapshotManager *snapshotManager = createByClassName<cISnapshotManager>(snapshotmanagerClass.c_str(), "snapshot manager");
    setSnapshotManager(snapshotManager);

    eventlogManager->configure(simulation, cfg);
    outVectorManager->configure(simulation, cfg);
    outScalarManager->configure(simulation, cfg);
    snapshotManager->configure(simulation, cfg);

    // settings
    setImagePath(extractImagePath(cfg, argList).c_str());
    setDebugOnErrors(cfg->getAsBool(CFGID_DEBUG_ON_ERRORS));  // note: handling overridden in Qtenv::readPerRunOptions() due to interference with GUI
    setPrintUndisposed(cfg->getAsBool(CFGID_PRINT_UNDISPOSED));
    recordEventlog = cfg->getAsBool(CFGID_RECORD_EVENTLOG);  // TODO tmp solution: cannot call setEventlogRecording(), because it calls eventlogRecorder->suspend()/resume(), which is NOT what we want here
}

std::string GenericEnvir::extractImagePath(cConfiguration *cfg, ArgList *args)
{
    std::string imagePath;
    if (args)
        for (std::string opt : args->optionValues('i'))
            imagePath = opp_join(";", imagePath, opt);
    if (cfg)
        imagePath = opp_join(";", imagePath, cfg->getAsPath(CFGID_IMAGE_PATH));
    std::string builtinImagePath = opp_removestart(OMNETPP_IMAGE_PATH, "/;"); // strip away the /; sequence from the beginning (a workaround for MinGW path conversion). See #785
    imagePath = opp_join(";", imagePath, opp_emptytodefault(getenv("OMNETPP_IMAGE_PATH"), builtinImagePath.c_str()));
    return imagePath;
}

void GenericEnvir::unsupported(const char *method) const
{
    throw new cRuntimeError("Unsupported method '%s()' -- override method in a subclass to implement it", method);
}

void GenericEnvir::setEventlogManager(cIEventlogManager *obj)
{
    delete eventlogManager;
    eventlogManager = obj;
}

void GenericEnvir::setOutVectorManager(cIOutputVectorManager *obj)
{
    delete outVectorManager;
    outVectorManager = obj;
}

void GenericEnvir::setOutScalarManager(cIOutputScalarManager *obj)
{
    delete outScalarManager;
    outScalarManager = obj;
}

void GenericEnvir::setSnapshotManager(cISnapshotManager *obj)
{
    delete snapshotManager;
    snapshotManager = obj;
}

void GenericEnvir::preconfigureComponent(cComponent *component)
{
}

void GenericEnvir::configureComponent(cComponent *component)
{
}

void GenericEnvir::addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty)
{
    cStatisticBuilder(getConfig()).addResultRecorders(component, signal, statisticName, statisticTemplateProperty);
}

void GenericEnvir::readParameter(cPar *par)
{
    ASSERT(!par->isSet());  // must be unset at this point

    // get it from the ini file
    std::string moduleFullPath = par->getOwner()->getFullPath();
    const cConfiguration::KeyValue& entry = getConfig()->getParameterEntry(moduleFullPath.c_str(), par->getName(), par->containsValue());
    const char *str = entry.getValue();

    if (opp_strcmp(str, "default") == 0) {
        ASSERT(par->containsValue());  // cConfiguration should not return "=default" lines for params that have no default value
        par->acceptDefault();
    }
    else if (opp_strcmp(str, "ask") == 0) {
        askParameter(par, false);
    }
    else if (!opp_isempty(str)) {
        try {
            par->parse(str, entry.getBaseDirectory(), entry.getSourceLocation());
        }
        catch (std::exception& e) {
            throw cRuntimeError("%s -- at %s", e.what(), entry.getSourceLocation().str().c_str());
        }
    }
    else {
        // str empty: no value in the ini file
        if (par->containsValue())
            par->acceptDefault();
        else
            askParameter(par, true);
    }

    ASSERT(par->isSet());  // and must be set after
}

void GenericEnvir::askParameter(cPar *par, bool unassigned)
{
    unsupported("askParameter");
}

cXMLElement *GenericEnvir::getXMLDocument(const char *filename, const char *path)
{
    cXMLElement *documentnode = xmlCache->getDocument(filename);
    return resolveXMLPath(documentnode, path);
}

cXMLElement *GenericEnvir::getParsedXMLString(const char *content, const char *path)
{
    cXMLElement *documentnode = xmlCache->getParsed(content);
    return resolveXMLPath(documentnode, path);
}

cXMLElement *GenericEnvir::resolveXMLPath(cXMLElement *documentnode, const char *path)
{
    ASSERT(documentnode);
    if (path) {
        ModNameParamResolver resolver(getSimulation()->getContextModule());  // resolves $MODULE_NAME etc in XPath expr.
        return cXMLElement::getDocumentElementByPath(documentnode, path, &resolver);
    }
    else {
        // returns the root element (child of the document node)
        return documentnode->getFirstChild();
    }
}

void GenericEnvir::forgetXMLDocument(const char *filename)
{
    xmlCache->forgetDocument(filename);
}

void GenericEnvir::forgetParsedXMLString(const char *content)
{
    xmlCache->forgetParsed(content);
}

void GenericEnvir::flushXMLDocumentCache()
{
    xmlCache->flushDocumentCache();
}

void GenericEnvir::flushXMLParsedContentCache()
{
    xmlCache->flushParsedContentCache();
}

unsigned GenericEnvir::getExtraStackForEnvir() const
{
    return extraStack;
}

cConfiguration *GenericEnvir::getConfig()
{
    return cfg;
}

std::string GenericEnvir::resolveResourcePath(const char *fileName, cComponentType *context)
{
    // search in current directory (or with absolute path)
    if (fileExists(fileName))
        return fileName;

    // search folder of the primary ini file
    const char *inifile = cfg->getFileName();
    if (!opp_isempty(inifile)) {
        std::string iniDir = directoryOf(inifile);
        std::string path = concatDirAndFile(iniDir.c_str(), fileName);
        if (fileExists(path.c_str()))
            return tidyFilename(path.c_str());
    }

    // search in the NED folder of the context component type
    const char *nedFile = context ? context->getSourceFileName() : nullptr;
    if (nedFile) {
        std::string dir = directoryOf(nedFile);
        std::string path = concatDirAndFile(dir.c_str(), fileName);
        if (fileExists(path.c_str()))
            return tidyFilename(path.c_str());
    }

    // search the NED path
    for (std::string dir : opp_splitpath(nedPath)) {
        std::string path = concatDirAndFile(dir.c_str(), fileName);
        if (fileExists(path.c_str()))
            return tidyFilename(path.c_str());
    }

    // search the image path
    for (std::string dir : opp_splitpath(imagePath)) {
        std::string path = concatDirAndFile(dir.c_str(), fileName);
        if (fileExists(path.c_str()))
            return tidyFilename(path.c_str());
    }

    return ""; // not found
}

void GenericEnvir::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
    case LF_PRE_NETWORK_SETUP: clearCurrentEventInfo(); break;
    case LF_POST_NETWORK_SETUP: getEventlogManager()->flush(); break;
    default: break;
    }
}

void GenericEnvir::bubble(cComponent *component, const char *text)
{
    if (recordEventlog)
        eventlogManager->bubble(component, text);
}

void GenericEnvir::objectDeleted(cObject *object)
{
}

void GenericEnvir::simulationEvent(cEvent *event)
{
    if (logFormatUsesEventName)
        currentEventName = event->getFullName();
    if (logFormatUsesEventClassName)
        currentEventClassName = event->getClassName();
    currentModuleId = event->isMessage() ? (static_cast<cMessage *>(event))->getArrivalModule()->getId() : -1;
    if (recordEventlog)
        eventlogManager->simulationEvent(event);
}

void GenericEnvir::componentInitBegin(cComponent *component, int stage)
{
    // To make initialization similar to processed events in this regard.
    // Unfortunately, for modules created (initialized) dynamically,
    // this moduleId will be stuck like this for the rest of the event,
    // processed by a different module.
    currentModuleId = (component && component->isModule()) ? component->getId() : -1;
}

void GenericEnvir::beginSend(cMessage *msg, const SendOptions& options)
{
    if (recordEventlog)
        eventlogManager->beginSend(msg, options);
}

void GenericEnvir::messageScheduled(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageScheduled(msg);
}

void GenericEnvir::messageCancelled(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageCancelled(msg);
}

void GenericEnvir::messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result)
{
    if (recordEventlog)
        eventlogManager->messageSendDirect(msg, toGate, result);
}

void GenericEnvir::messageSendHop(cMessage *msg, cGate *srcGate)
{
    if (recordEventlog)
        eventlogManager->messageSendHop(msg, srcGate);
}

void GenericEnvir::messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result)
{
    if (recordEventlog)
        eventlogManager->messageSendHop(msg, srcGate, result);
}

void GenericEnvir::endSend(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->endSend(msg);
}

void GenericEnvir::messageCreated(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageCreated(msg);
}

void GenericEnvir::messageCloned(cMessage *msg, cMessage *clone)
{
    if (recordEventlog)
        eventlogManager->messageCloned(msg, clone);
}

void GenericEnvir::messageDeleted(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageDeleted(msg);
}

void GenericEnvir::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent)
{
    if (recordEventlog)
        eventlogManager->componentMethodBegin(from, to, methodFmt, va);
}

void GenericEnvir::componentMethodEnd()
{
    if (recordEventlog)
        eventlogManager->componentMethodEnd();
}

void GenericEnvir::moduleCreated(cModule *newmodule)
{
    if (recordEventlog)
        eventlogManager->moduleCreated(newmodule);
}

void GenericEnvir::moduleDeleted(cModule *module)
{
    if (recordEventlog)
        eventlogManager->moduleDeleted(module);
}

void GenericEnvir::moduleReparented(cModule *module, cModule *oldparent, int oldId)
{
    if (recordEventlog)
        eventlogManager->moduleReparented(module, oldparent, oldId);
}

void GenericEnvir::gateCreated(cGate *newgate)
{
    if (recordEventlog)
        eventlogManager->gateCreated(newgate);
}

void GenericEnvir::gateDeleted(cGate *gate)
{
    if (recordEventlog)
        eventlogManager->gateDeleted(gate);
}

void GenericEnvir::connectionCreated(cGate *srcgate)
{
    if (recordEventlog)
        eventlogManager->connectionCreated(srcgate);
}

void GenericEnvir::connectionDeleted(cGate *srcgate)
{
    if (recordEventlog)
        eventlogManager->connectionDeleted(srcgate);
}

void GenericEnvir::displayStringChanged(cComponent *component)
{
    if (recordEventlog)
        eventlogManager->displayStringChanged(component);
}

void GenericEnvir::log(cLogEntry *entry)
{
    if (recordEventlog) {
        std::string prefix = logFormatter.formatPrefix(entry);
        eventlogManager->logLine(prefix.c_str(), entry->text, entry->textLength);
    }
}

void GenericEnvir::undisposedObject(cObject *obj)
{
    if (printUndisposed)
        std::cerr << "undisposed object: (" << obj->getClassName() << ") " << obj->getFullPath() << " -- check module destructor" << endl;
}

void GenericEnvir::setEventlogRecording(bool enabled)
{
    // NOTE: eventlogManager must be non-nullptr when recordEventlog is true
    if (recordEventlog != enabled) {
        if (enabled)
            eventlogManager->resume();
        else
            eventlogManager->suspend();
        recordEventlog = enabled;
    }
}

void GenericEnvir::setLogLevel(LogLevel logLevel)
{
    cLog::logLevel = logLevel;
}

void GenericEnvir::setLogFormat(const char *logFormat)
{
    logFormatter.setFormat(logFormat);
    logFormatUsesEventName = logFormatter.usesEventName();
    logFormatUsesEventClassName = logFormatter.usesEventClassName();
}

void GenericEnvir::clearCurrentEventInfo()
{
    currentEventName = "";
    currentEventClassName = nullptr;
    currentModuleId = -1;
}

void *GenericEnvir::registerOutputVector(const char *modulename, const char *vectorname)
{
    ASSERT(outVectorManager);
    return outVectorManager->registerVector(modulename, vectorname);
}

void GenericEnvir::deregisterOutputVector(void *vechandle)
{
    ASSERT(outVectorManager);
    outVectorManager->deregisterVector(vechandle);
}

void GenericEnvir::setVectorAttribute(void *vechandle, const char *name, const char *value)
{
    ASSERT(outVectorManager);
    outVectorManager->setVectorAttribute(vechandle, name, value);
}

bool GenericEnvir::recordInOutputVector(void *vechandle, simtime_t t, double value)
{
    ASSERT(outVectorManager);
    if (getSimulation()->getFingerprintCalculator())
        // TODO: determine component and result name if possible
        getSimulation()->getFingerprintCalculator()->addVectorResult(nullptr, "", t, value);
    return outVectorManager->record(vechandle, t, value);
}

void GenericEnvir::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    ASSERT(outScalarManager);
    outScalarManager->recordScalar(component, name, value, attributes);
    if (getSimulation()->getFingerprintCalculator())
        getSimulation()->getFingerprintCalculator()->addScalarResult(component, name, value);
}

void GenericEnvir::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    ASSERT(outScalarManager);
    outScalarManager->recordStatistic(component, name, statistic, attributes);
    if (getSimulation()->getFingerprintCalculator())
        getSimulation()->getFingerprintCalculator()->addStatisticResult(component, name, statistic);
}

void GenericEnvir::recordParameter(cPar *par)
{
    assert(outScalarManager);
    outScalarManager->recordParameter(par);
}

void GenericEnvir::recordComponentType(cComponent *component)
{
    assert(outScalarManager);
    outScalarManager->recordComponentType(component);
}

std::ostream *GenericEnvir::getStreamForSnapshot()
{
    return snapshotManager->getStreamForSnapshot();
}

void GenericEnvir::releaseStreamForSnapshot(std::ostream *os)
{
    snapshotManager->releaseStreamForSnapshot(os);
}

bool GenericEnvir::idle()
{
    return false;
}

bool GenericEnvir::ensureDebugger(cRuntimeError *error)
{
    unsupported("ensureDebugger");
}

bool GenericEnvir::shouldDebugNow(cRuntimeError *error)
{
    return debugOnErrors && ensureDebugger(error);
}

bool GenericEnvir::isGUI() const
{
    return gui;
}

bool GenericEnvir::isExpressMode() const
{
    return expressMode;
}

void GenericEnvir::alert(const char *msg)
{
    unsupported("alert");
}

std::string GenericEnvir::input(const char *prompt, const char *defaultReply)
{
    unsupported("input");
}

bool GenericEnvir::askYesNo(const char *prompt)
{
    unsupported("askYesNo");
}

void GenericEnvir::getImageSize(const char *imageName, double &outWidth, double &outHeight)
{
    unsupported("getImageSize");
}

void GenericEnvir::getTextExtent(const cFigure::Font &font, const char *text,
        double &outWidth, double &outHeight, double &outAscent)
{
    unsupported("getTextExtent");
}

void GenericEnvir::appendToImagePath(const char *directory)
{
    unsupported("appendToImagePath");
}

void GenericEnvir::loadImage(const char *fileName, const char *imageName)
{
    unsupported("loadImage");
}

cFigure::Rectangle GenericEnvir::getSubmoduleBounds(const cModule *submodule)
{
    unsupported("getSubmoduleBounds");
}

std::vector<cFigure::Point> GenericEnvir::getConnectionLine(const cGate *sourceGate)
{
    unsupported("getConnectionLine");
}

double GenericEnvir::getZoomLevel(const cModule *module)
{
    unsupported("getZoomLevel");
}

double GenericEnvir::getAnimationTime() const
{
    unsupported("getAnimationTime");
}

double GenericEnvir::getAnimationSpeed() const
{
    unsupported("getAnimationSpeed");
}

double GenericEnvir::getRemainingAnimationHoldTime() const
{
    unsupported("getRemainingAnimationHoldTime");
}

void GenericEnvir::pausePoint()
{
}

}  // namespace envir
}  // namespace omnetpp

