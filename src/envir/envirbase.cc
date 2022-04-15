//==========================================================================
//  ENVIRBASE.CC - part of
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

#include <cstdio>
#include <csignal>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>
#include "common/stringtokenizer.h"
#include "common/fnamelisttokenizer.h"
#include "common/stringutil.h"
#include "common/opp_ctype.h"
#include "common/stringtokenizer.h"
#include "common/stringutil.h"
#include "common/fileglobber.h"
#include "common/unitconversion.h"
#include "common/commonutil.h"
#include "common/ver.h"
#include "common/fileutil.h"  // splitFileName
#include "omnetpp/ccoroutine.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cscheduler.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cpar.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"
#include "omnetpp/crngmanager.h"
#include "omnetpp/ccanvas.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/simtime.h"
#include "omnetpp/platdep/platmisc.h"
#include "omnetpp/cstatisticbuilder.h"
#include "omnetpp/fileline.h"
#include "args.h"
#include "envirbase.h"
#include "envirutils.h"
#include "appreg.h"
#include "valueiterator.h"
#include "xmldoccache.h"
#include "iallinone.h"

#ifdef WITH_PARSIM
#include "omnetpp/cparsimcomm.h"
#include "sim/parsim/cparsimpartition.h"
#include "sim/parsim/cparsimsynchr.h"
#endif

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


Register_GlobalConfigOption(CFGID_PARALLEL_SIMULATION, "parallel-simulation", CFG_BOOL, "false", "Enables parallel distributed simulation.");
Register_PerRunConfigOption(CFGID_EVENTLOGMANAGER_CLASS, "eventlogmanager-class", CFG_STRING, "omnetpp::envir::EventlogFileManager", "Part of the Envir plugin mechanism: selects the eventlog manager class to be used to record data. The class has to implement the `cIEventlogManager` interface.");
Register_PerRunConfigOption(CFGID_OUTPUTVECTORMANAGER_CLASS, "outputvectormanager-class", CFG_STRING, DEFAULT_OUTPUTVECTORMANAGER_CLASS, "Part of the Envir plugin mechanism: selects the output vector manager class to be used to record data from output vectors. The class has to implement the `cIOutputVectorManager` interface.");
Register_PerRunConfigOption(CFGID_OUTPUTSCALARMANAGER_CLASS, "outputscalarmanager-class", CFG_STRING, DEFAULT_OUTPUTSCALARMANAGER_CLASS, "Part of the Envir plugin mechanism: selects the output scalar manager class to be used to record data passed to recordScalar(). The class has to implement the `cIOutputScalarManager` interface.");
Register_PerRunConfigOption(CFGID_SNAPSHOTMANAGER_CLASS, "snapshotmanager-class", CFG_STRING, "omnetpp::envir::FileSnapshotManager", "Part of the Envir plugin mechanism: selects the class to handle streams to which snapshot() writes its output. The class has to implement the `cISnapshotManager` interface.");
Register_GlobalConfigOption(CFGID_IMAGE_PATH, "image-path", CFG_PATH, "./images", "A semicolon-separated list of directories that contain module icons and other resources. This list will be concatenated with the contents of the `OMNETPP_IMAGE_PATH` environment variable or with a compile-time, hardcoded image path if the environment variable is empty.");
Register_PerRunConfigOption(CFGID_DEBUG_ON_ERRORS, "debug-on-errors", CFG_BOOL, "false", "When set to true, runtime errors will cause the simulation program to break into the C++ debugger (if the simulation is running under one, or just-in-time debugging is activated). Once in the debugger, you can view the stack trace or examine variables.");
Register_PerRunConfigOption(CFGID_PRINT_UNDISPOSED, "print-undisposed", CFG_BOOL, "true", "Whether to report objects left (that is, not deallocated by simple module destructors) after network cleanup.");
Register_GlobalConfigOption(CFGID_NED_PATH, "ned-path", CFG_PATH, "", "A semicolon-separated list of directories. The directories will be regarded as roots of the NED package hierarchy, and all NED files will be loaded from their subdirectory trees. This option is normally left empty, as the OMNeT++ IDE sets the NED path automatically, and for simulations started outside the IDE it is more convenient to specify it via command-line option (-n) or via environment variable (OMNETPP_NED_PATH, NEDPATH).");
Register_GlobalConfigOption(CFGID_NED_PACKAGE_EXCLUSIONS, "ned-package-exclusions", CFG_CUSTOM, "", "A semicolon-separated list of NED packages to be excluded when loading NED files. Sub-packages of excluded ones are also excluded. Additional items may be specified via the `-x` command-line option and the `OMNETPP_NED_PACKAGE_EXCLUSIONS` environment variable.");
Register_GlobalConfigOption(CFGID_DEBUGGER_ATTACH_ON_ERROR, "debugger-attach-on-error", CFG_BOOL, "false", "When set to true, runtime errors and crashes will trigger an external debugger to be launched (if not already present), allowing you to perform just-in-time debugging on the simulation process. The debugger command is configurable. Note that debugging (i.e. attaching to) a non-child process needs to be explicitly enabled on some systems, e.g. Ubuntu.");
Register_PerRunConfigOption(CFGID_RECORD_EVENTLOG, "record-eventlog", CFG_BOOL, "false", "Enables recording an eventlog file, which can be later visualized on a sequence chart. See `eventlog-file` option too.");


EnvirBase::EnvirBase(IAllInOne *app) : out(std::cout.rdbuf()), app(app)
{
    cfg = nullptr;
    xmlCache = nullptr;

    recordEventlog = false;
    eventlogManager = nullptr;
    outVectorManager = nullptr;
    outScalarManager = nullptr;
    snapshotManager = nullptr;

#ifdef WITH_PARSIM
    parsimPartition = nullptr;
#endif
}

EnvirBase::~EnvirBase()
{
    //delete opt;
    delete cfg;
    delete xmlCache;

    delete eventlogManager;
    delete outVectorManager;
    delete outScalarManager;
    delete snapshotManager;

#ifdef WITH_PARSIM
    delete parsimPartition;
#endif
}

void EnvirBase::initialize(cSimulation *simulation, cConfiguration *cfg, ArgList *args)
{
    this->cfg = cfg;
    this->simulation = simulation;

    // ensure correct numeric format in output files
    setPosixLocale();

    parsim = cfg->getAsBool(CFGID_PARALLEL_SIMULATION);

#ifndef WITH_PARSIM
    if (parsim)
        throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif

    attachDebuggerOnErrors = cfg->getAsBool(CFGID_DEBUGGER_ATTACH_ON_ERROR);

    SimTime::configure(cfg);

    setNedPath(extractNedPath(cfg, args).c_str());
    setNedExcludedPackages(extractNedExcludedPackages(cfg, args).c_str());
    setImagePath(extractImagePath(cfg, args).c_str());

    cCoroutine::configure(cfg);

    // install XML document cache
    xmlCache = new XMLDocCache();

    // parallel simulation
    if (parsim) {
#ifdef WITH_PARSIM
        parsimPartition = new cParsimPartition();
        parsimPartition->configure(getSimulation(), cfg);
#else
        throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
    }
}

void EnvirBase::configure(cConfiguration *cfg)
{
    this->cfg = cfg;

    // get options from ini file
    debugOnErrors = cfg->getAsBool(CFGID_DEBUG_ON_ERRORS);  // note: handling overridden in Qtenv::readPerRunOptions() due to interference with GUI
    printUndisposed = cfg->getAsBool(CFGID_PRINT_UNDISPOSED);

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

    getSimulation()->configure(cfg, parsim);

    // init nextUniqueNumber -- startRun() is too late because simple module ctors have run by then
    setUniqueNumberRange(0, 0); // =until it wraps
#ifdef WITH_PARSIM
    if (parsim) {
        uint64_t myRank = parsimPartition->getProcId();
        uint64_t range = UINT64_MAX / parsimPartition->getNumPartitions();
        setUniqueNumberRange(myRank * range, (myRank+1) * range);
    }
#endif

    recordEventlog = cfg->getAsBool(CFGID_RECORD_EVENTLOG);
}


std::string EnvirBase::extractNedPath(cConfiguration *cfg, ArgList *args)
{
    // NED path. It is taken from the "-n" command-line options, the OMNETPP_NED_PATH
    // environment variable, and the "ned-path=" config option. If the result is still
    // empty, we fall back to "." -- this is needed for single-directory models to work.
    std::string nedPath = opp_join(args->optionValues('n'), ";", true);
    nedPath = opp_join(";", nedPath, getConfig()->getAsPath(CFGID_NED_PATH));
    nedPath = opp_join(";", nedPath, opp_nulltoempty(getenv("OMNETPP_NED_PATH")));
    nedPath = opp_join(";", nedPath, opp_nulltoempty(getenv("NEDPATH")));
    if (nedPath.empty())
        nedPath = ".";
    return nedPath;
}

std::string EnvirBase::extractNedExcludedPackages(cConfiguration *cfg, ArgList *args)
{
    std::string nedExcludedPackages = opp_join(args->optionValues('x'), ";", true);
    nedExcludedPackages = opp_join(";", nedExcludedPackages, opp_nulltoempty(getConfig()->getAsCustom(CFGID_NED_PACKAGE_EXCLUSIONS)));
    nedExcludedPackages = opp_join(";", nedExcludedPackages, opp_nulltoempty(getenv("OMNETPP_NED_PACKAGE_EXCLUSIONS")));
    return nedExcludedPackages;
}

std::string EnvirBase::extractImagePath(cConfiguration *cfg, ArgList *args)
{
    std::string imagePath;
    for (std::string opt : args->optionValues('i'))
        imagePath = opp_join(";", imagePath, opt);
    imagePath = opp_join(";", imagePath, getConfig()->getAsPath(CFGID_IMAGE_PATH));
    std::string builtinImagePath = opp_removestart(OMNETPP_IMAGE_PATH, "/;"); // strip away the /; sequence from the beginning (a workaround for MinGW path conversion). See #785
    imagePath = opp_join(";", imagePath, opp_emptytodefault(getenv("OMNETPP_IMAGE_PATH"), builtinImagePath.c_str()));
    return imagePath;
}

void EnvirBase::setEventlogManager(cIEventlogManager *obj)
{
    delete eventlogManager;
    eventlogManager = obj;
}

void EnvirBase::setOutVectorManager(cIOutputVectorManager *obj)
{
    delete outVectorManager;
    outVectorManager = obj;
}

void EnvirBase::setOutScalarManager(cIOutputScalarManager *obj)
{
    delete outScalarManager;
    outScalarManager = obj;
}

void EnvirBase::setSnapshotManager(cISnapshotManager *obj)
{
    delete snapshotManager;
    snapshotManager = obj;
}

void EnvirBase::preconfigureComponent(cComponent *component)
{
    app->preconfigureComponent(component);
}

void EnvirBase::configureComponent(cComponent *component)
{
    app->configureComponent(component);
}

void EnvirBase::addResultRecorders(cComponent *component, simsignal_t signal, const char *statisticName, cProperty *statisticTemplateProperty)
{
    cStatisticBuilder(getConfig()).addResultRecorders(component, signal, statisticName, statisticTemplateProperty);
}

void EnvirBase::readParameter(cPar *par)
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

void EnvirBase::askParameter(cPar *par, bool unassigned)
{
    app->askParameter(par, unassigned);
}

bool EnvirBase::isModuleLocal(cModule *parentmod, const char *modname, int index)
{
#ifdef WITH_PARSIM
    return parsim ? parsimPartition->isModuleLocal(parentmod, modname, index) : true;
#else
    return true;
#endif
}

cXMLElement *EnvirBase::getXMLDocument(const char *filename, const char *path)
{
    cXMLElement *documentnode = xmlCache->getDocument(filename);
    return resolveXMLPath(documentnode, path);
}

cXMLElement *EnvirBase::getParsedXMLString(const char *content, const char *path)
{
    cXMLElement *documentnode = xmlCache->getParsed(content);
    return resolveXMLPath(documentnode, path);
}

cXMLElement *EnvirBase::resolveXMLPath(cXMLElement *documentnode, const char *path)
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

void EnvirBase::forgetXMLDocument(const char *filename)
{
    xmlCache->forgetDocument(filename);
}

void EnvirBase::forgetParsedXMLString(const char *content)
{
    xmlCache->forgetParsed(content);
}

void EnvirBase::flushXMLDocumentCache()
{
    xmlCache->flushDocumentCache();
}

void EnvirBase::flushXMLParsedContentCache()
{
    xmlCache->flushParsedContentCache();
}

unsigned EnvirBase::getExtraStackForEnvir() const
{
    return app->getExtraStackForEnvir();
}

cConfiguration *EnvirBase::getConfig()
{
    return cfg;
}

cConfigurationEx *EnvirBase::getConfigEx()
{
    return dynamic_cast<cConfigurationEx *>(getConfig());
}

std::string EnvirBase::resolveResourcePath(const char *fileName, cComponentType *context)
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

//-------------------------------------------------------------

void EnvirBase::bubble(cComponent *component, const char *text)
{
    if (recordEventlog)
        eventlogManager->bubble(component, text);
    app->bubble(component, text);
}

void EnvirBase::objectDeleted(cObject *object)
{
    app->objectDeleted(object);
}

void EnvirBase::simulationEvent(cEvent *event)
{
    if (logFormatUsesEventName)
        currentEventName = event->getFullName();
    if (logFormatUsesEventClassName)
        currentEventClassName = event->getClassName();
    currentModuleId = event->isMessage() ? (static_cast<cMessage *>(event))->getArrivalModule()->getId() : -1;
    if (recordEventlog)
        eventlogManager->simulationEvent(event);

    app->simulationEvent(event);
}

void EnvirBase::componentInitBegin(cComponent *component, int stage)
{
    // To make initialization similar to processed events in this regard.
    // Unfortunately, for modules created (initialized) dynamically,
    // this moduleId will be stuck like this for the rest of the event,
    // processed by a different module.
    currentModuleId = (component && component->isModule()) ? component->getId() : -1;
    app->componentInitBegin(component, stage);
}

void EnvirBase::beginSend(cMessage *msg, const SendOptions& options)
{
    if (recordEventlog)
        eventlogManager->beginSend(msg, options);
    app->beginSend(msg, options);
}

void EnvirBase::messageScheduled(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageScheduled(msg);
    app->messageScheduled(msg);
}

void EnvirBase::messageCancelled(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageCancelled(msg);
    app->messageCancelled(msg);
}

void EnvirBase::messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result)
{
    if (recordEventlog)
        eventlogManager->messageSendDirect(msg, toGate, result);
    app->messageSendDirect(msg, toGate, result);
}

void EnvirBase::messageSendHop(cMessage *msg, cGate *srcGate)
{
    if (recordEventlog)
        eventlogManager->messageSendHop(msg, srcGate);
    app->messageSendHop(msg, srcGate);
}

void EnvirBase::messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result)
{
    if (recordEventlog)
        eventlogManager->messageSendHop(msg, srcGate, result);
    app->messageSendHop(msg, srcGate, result);
}

void EnvirBase::endSend(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->endSend(msg);
    app->endSend(msg);
}

void EnvirBase::messageCreated(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageCreated(msg);
    app->messageCreated(msg);
}

void EnvirBase::messageCloned(cMessage *msg, cMessage *clone)
{
    if (recordEventlog)
        eventlogManager->messageCloned(msg, clone);
    app->messageCloned(msg, clone);
}

void EnvirBase::messageDeleted(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageDeleted(msg);
    app->messageDeleted(msg);
}

void EnvirBase::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent)
{
    if (recordEventlog)
        eventlogManager->componentMethodBegin(from, to, methodFmt, va);
    app->componentMethodBegin(from, to, methodFmt, va, silent);
}

void EnvirBase::componentMethodEnd()
{
    if (recordEventlog)
        eventlogManager->componentMethodEnd();
    app->componentMethodEnd();
}

void EnvirBase::moduleCreated(cModule *newmodule)
{
    if (recordEventlog)
        eventlogManager->moduleCreated(newmodule);
    app->moduleCreated(newmodule);
}

void EnvirBase::moduleDeleted(cModule *module)
{
    if (recordEventlog)
        eventlogManager->moduleDeleted(module);
    app->moduleDeleted(module);
}

void EnvirBase::moduleReparented(cModule *module, cModule *oldparent, int oldId)
{
    if (recordEventlog)
        eventlogManager->moduleReparented(module, oldparent, oldId);
    app->moduleReparented(module, oldparent, oldId);
}

void EnvirBase::gateCreated(cGate *newgate)
{
    if (recordEventlog)
        eventlogManager->gateCreated(newgate);
    app->gateCreated(newgate);
}

void EnvirBase::gateDeleted(cGate *gate)
{
    if (recordEventlog)
        eventlogManager->gateDeleted(gate);
    app->gateDeleted(gate);
}

void EnvirBase::connectionCreated(cGate *srcgate)
{
    if (recordEventlog)
        eventlogManager->connectionCreated(srcgate);
    app->connectionCreated(srcgate);
}

void EnvirBase::connectionDeleted(cGate *srcgate)
{
    if (recordEventlog)
        eventlogManager->connectionDeleted(srcgate);
    app->connectionDeleted(srcgate);
}

void EnvirBase::displayStringChanged(cComponent *component)
{
    if (recordEventlog)
        eventlogManager->displayStringChanged(component);
    app->displayStringChanged(component);
}

void EnvirBase::log(cLogEntry *entry)
{
    if (recordEventlog) {
        std::string prefix = logFormatter.formatPrefix(entry);
        eventlogManager->logLine(prefix.c_str(), entry->text, entry->textLength);
    }
    app->log(entry);
}

void EnvirBase::undisposedObject(cObject *obj)
{
    if (printUndisposed)
        out << "undisposed object: (" << obj->getClassName() << ") " << obj->getFullPath() << " -- check module destructor" << endl;
    app->undisposedObject(obj);
}

void EnvirBase::setEventlogRecording(bool enabled)
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

void EnvirBase::setLogLevel(LogLevel logLevel)
{
    cLog::logLevel = logLevel;
}

void EnvirBase::setLogFormat(const char *logFormat)
{
    logFormatter.setFormat(logFormat);
    logFormatUsesEventName = logFormatter.usesEventName();
    logFormatUsesEventClassName = logFormatter.usesEventClassName();
}

void EnvirBase::clearCurrentEventInfo()
{
    currentEventName = "";
    currentEventClassName = nullptr;
    currentModuleId = -1;
}

//-------------------------------------------------------------

void *EnvirBase::registerOutputVector(const char *modulename, const char *vectorname)
{
    ASSERT(outVectorManager);
    return outVectorManager->registerVector(modulename, vectorname);
}

void EnvirBase::deregisterOutputVector(void *vechandle)
{
    ASSERT(outVectorManager);
    outVectorManager->deregisterVector(vechandle);
}

void EnvirBase::setVectorAttribute(void *vechandle, const char *name, const char *value)
{
    ASSERT(outVectorManager);
    outVectorManager->setVectorAttribute(vechandle, name, value);
}

bool EnvirBase::recordInOutputVector(void *vechandle, simtime_t t, double value)
{
    ASSERT(outVectorManager);
    if (getSimulation()->getFingerprintCalculator())
        // TODO: determine component and result name if possible
        getSimulation()->getFingerprintCalculator()->addVectorResult(nullptr, "", t, value);
    return outVectorManager->record(vechandle, t, value);
}

//-------------------------------------------------------------

void EnvirBase::recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes)
{
    ASSERT(outScalarManager);
    outScalarManager->recordScalar(component, name, value, attributes);
    if (getSimulation()->getFingerprintCalculator())
        getSimulation()->getFingerprintCalculator()->addScalarResult(component, name, value);
}

void EnvirBase::recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes)
{
    ASSERT(outScalarManager);
    outScalarManager->recordStatistic(component, name, statistic, attributes);
    if (getSimulation()->getFingerprintCalculator())
        getSimulation()->getFingerprintCalculator()->addStatisticResult(component, name, statistic);
}

void EnvirBase::recordParameter(cPar *par)
{
    assert(outScalarManager);
    outScalarManager->recordParameter(par);
}

void EnvirBase::recordComponentType(cComponent *component)
{
    assert(outScalarManager);
    outScalarManager->recordComponentType(component);
}

//-------------------------------------------------------------

std::ostream *EnvirBase::getStreamForSnapshot()
{
    return snapshotManager->getStreamForSnapshot();
}

void EnvirBase::releaseStreamForSnapshot(std::ostream *os)
{
    snapshotManager->releaseStreamForSnapshot(os);
}

//-------------------------------------------------------------

uint64_t EnvirBase::getUniqueNumber()
{
    uint64_t ret = nextUniqueNumber++;
    if (nextUniqueNumber == uniqueNumbersEnd)
        throw cRuntimeError("getUniqueNumber(): All values have been consumed");
    return ret;
}

bool EnvirBase::idle()
{
    return app->idle();
}

bool EnvirBase::ensureDebugger(cRuntimeError *error)
{
    return app->ensureDebugger(error);
}

bool EnvirBase::shouldDebugNow(cRuntimeError *error)
{
    return debugOnErrors && ensureDebugger(error);
}

int EnvirBase::getParsimProcId() const
{
#ifdef WITH_PARSIM
    return parsimPartition? parsimPartition->getProcId() : 0;
#else
    return 0;
#endif
}

int EnvirBase::getParsimNumPartitions() const
{
#ifdef WITH_PARSIM
    return parsimPartition? parsimPartition->getNumPartitions() : 0;
#else
    return 0;
#endif
}

//-------------------------------------------------------------
bool EnvirBase::isGUI() const
{
    return app->isGUI();
}

bool EnvirBase::isExpressMode() const
{
    return app->isExpressMode();
}

void EnvirBase::alert(const char *msg)
{
    app->alert(msg);
}

std::string EnvirBase::gets(const char *prompt, const char *defaultReply)
{
    return app->gets(prompt, defaultReply);
}

bool EnvirBase::askYesNo(const char *prompt)
{
    return app->askYesNo(prompt);
}

void EnvirBase::getImageSize(const char *imageName, double &outWidth, double &outHeight)
{
    app->getImageSize(imageName, outWidth, outHeight);
}

void EnvirBase::getTextExtent(const cFigure::Font &font, const char *text,
        double &outWidth, double &outHeight, double &outAscent)
{
    app->getTextExtent(font, text, outWidth, outHeight, outAscent);
}

void EnvirBase::appendToImagePath(const char *directory)
{
    app->appendToImagePath(directory);
}

void EnvirBase::loadImage(const char *fileName, const char *imageName)
{
    app->loadImage(fileName, imageName);
}

cFigure::Rectangle EnvirBase::getSubmoduleBounds(const cModule *submodule)
{
    return app->getSubmoduleBounds(submodule);
}

std::vector<cFigure::Point> EnvirBase::getConnectionLine(const cGate *sourceGate)
{
    return app->getConnectionLine(sourceGate);
}

double EnvirBase::getZoomLevel(const cModule *module)
{
    return app->getZoomLevel(module);
}

double EnvirBase::getAnimationTime() const
{
    return app->getAnimationTime();
}

double EnvirBase::getAnimationSpeed() const
{
    return app->getAnimationSpeed();
}

double EnvirBase::getRemainingAnimationHoldTime() const
{
    return app->getRemainingAnimationHoldTime();
}

void EnvirBase::pausePoint()
{
    app->pausePoint();
}


}  // namespace envir
}  // namespace omnetpp

