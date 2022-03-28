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
#include "common/enumstr.h"
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

#ifdef WITH_PARSIM
#include "omnetpp/cparsimcomm.h"
#include "sim/parsim/cparsimpartition.h"
#include "sim/parsim/cparsimsynchr.h"
#endif

#ifdef NDEBUG
#define CHECKSIGNALS_DEFAULT        "false"
#else
#define CHECKSIGNALS_DEFAULT        "true"
#endif

#ifdef PREFER_SQLITE_RESULT_FILES
#define DEFAULT_OUTPUTVECTORMANAGER_CLASS "omnetpp::envir::SqliteOutputVectorManager"
#define DEFAULT_OUTPUTSCALARMANAGER_CLASS "omnetpp::envir::SqliteOutputScalarManager"
#else
#define DEFAULT_OUTPUTVECTORMANAGER_CLASS "omnetpp::envir::OmnetppOutputVectorManager"
#define DEFAULT_OUTPUTSCALARMANAGER_CLASS "omnetpp::envir::OmnetppOutputScalarManager"
#endif

#define DEFAULT_OUTPUT_SCALAR_PRECISION    "14"
#define DEFAULT_OUTPUT_VECTOR_PRECISION    "14"
#define DEFAULT_OUTPUT_VECTOR_MEMORY_LIMIT "16MiB"
#define DEFAULT_VECTOR_BUFFER              "1MiB"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace envir {

using std::ostream;


Register_GlobalConfigOptionU(CFGID_TOTAL_STACK, "total-stack", "B", nullptr, "Specifies the maximum memory for `activity()` simple module stacks. You need to increase this value if you get a \"Cannot allocate coroutine stack\" error.");
Register_GlobalConfigOption(CFGID_PARALLEL_SIMULATION, "parallel-simulation", CFG_BOOL, "false", "Enables parallel distributed simulation.");
Register_GlobalConfigOption(CFGID_PARSIM_NUM_PARTITIONS, "parsim-num-partitions", CFG_INT, nullptr, "If `parallel-simulation=true`, it specifies the number of parallel processes being used. This value must be in agreement with the number of simulator instances launched, e.g. with the `-n` or `-np` command-line option specified to the `mpirun` program when using MPI.");
Register_GlobalConfigOption(CFGID_PARSIM_PROCID, "parsim-procid", CFG_INT, nullptr, "If `parallel-simulation=true`, it specifies the ordinal of the current simulation process within the list parallel processes. The value must be in the range 0...n-1, where n is the number of partitions. This option is not required when using MPI communications, because MPI has its own way of conveying this information.");
Register_PerRunConfigOption(CFGID_SCHEDULER_CLASS, "scheduler-class", CFG_STRING, "omnetpp::cSequentialScheduler", "Part of the Envir plugin mechanism: selects the scheduler class. This plugin interface allows for implementing real-time, hardware-in-the-loop, distributed and distributed parallel simulation. The class has to implement the `cScheduler` interface.");
Register_GlobalConfigOption(CFGID_PARSIM_COMMUNICATIONS_CLASS, "parsim-communications-class", CFG_STRING, "omnetpp::cFileCommunications", "If `parallel-simulation=true`, it selects the class that implements communication between partitions. The class must implement the `cParsimCommunications` interface.");
Register_GlobalConfigOption(CFGID_PARSIM_SYNCHRONIZATION_CLASS, "parsim-synchronization-class", CFG_STRING, "omnetpp::cNullMessageProtocol", "If `parallel-simulation=true`, it selects the parallel simulation algorithm. The class must implement the `cParsimSynchronizer` interface.");
Register_PerRunConfigOption(CFGID_EVENTLOGMANAGER_CLASS, "eventlogmanager-class", CFG_STRING, "omnetpp::envir::EventlogFileManager", "Part of the Envir plugin mechanism: selects the eventlog manager class to be used to record data. The class has to implement the `cIEventlogManager` interface.");
Register_PerRunConfigOption(CFGID_OUTPUTVECTORMANAGER_CLASS, "outputvectormanager-class", CFG_STRING, DEFAULT_OUTPUTVECTORMANAGER_CLASS, "Part of the Envir plugin mechanism: selects the output vector manager class to be used to record data from output vectors. The class has to implement the `cIOutputVectorManager` interface.");
Register_PerRunConfigOption(CFGID_OUTPUTSCALARMANAGER_CLASS, "outputscalarmanager-class", CFG_STRING, DEFAULT_OUTPUTSCALARMANAGER_CLASS, "Part of the Envir plugin mechanism: selects the output scalar manager class to be used to record data passed to recordScalar(). The class has to implement the `cIOutputScalarManager` interface.");
Register_PerRunConfigOption(CFGID_SNAPSHOTMANAGER_CLASS, "snapshotmanager-class", CFG_STRING, "omnetpp::envir::FileSnapshotManager", "Part of the Envir plugin mechanism: selects the class to handle streams to which snapshot() writes its output. The class has to implement the `cISnapshotManager` interface.");
Register_PerRunConfigOption(CFGID_FUTUREEVENTSET_CLASS, "futureeventset-class", CFG_STRING, "omnetpp::cEventHeap", "Part of the Envir plugin mechanism: selects the class for storing the future events in the simulation. The class has to implement the `cFutureEventSet` interface.");
Register_GlobalConfigOption(CFGID_IMAGE_PATH, "image-path", CFG_PATH, "./images", "A semicolon-separated list of directories that contain module icons and other resources. This list will be concatenated with the contents of the `OMNETPP_IMAGE_PATH` environment variable or with a compile-time, hardcoded image path if the environment variable is empty.");
Register_GlobalConfigOption(CFGID_FNAME_APPEND_HOST, "fname-append-host", CFG_BOOL, nullptr, "Turning it on will cause the host name and process Id to be appended to the names of output files (e.g. omnetpp.vec, omnetpp.sca). This is especially useful with distributed simulation. The default value is true if parallel simulation is enabled, false otherwise.");
Register_PerRunConfigOption(CFGID_DEBUG_ON_ERRORS, "debug-on-errors", CFG_BOOL, "false", "When set to true, runtime errors will cause the simulation program to break into the C++ debugger (if the simulation is running under one, or just-in-time debugging is activated). Once in the debugger, you can view the stack trace or examine variables.");
Register_PerRunConfigOption(CFGID_PRINT_UNDISPOSED, "print-undisposed", CFG_BOOL, "true", "Whether to report objects left (that is, not deallocated by simple module destructors) after network cleanup.");
Register_PerRunConfigOption(CFGID_PARAMETER_MUTABILITY_CHECK, "parameter-mutability-check", CFG_BOOL, "true", "Setting to false will disable errors raised when trying to change the values of module/channel parameters not marked as @mutable. This is primarily a compatibility setting intended to facilitate running simulation models that were not yet annotated with @mutable.");
Register_GlobalConfigOption(CFGID_SIMTIME_SCALE, "simtime-scale", CFG_INT, "-12", "DEPRECATED in favor of simtime-resolution. Sets the scale exponent, and thus the resolution of time for the 64-bit fixed-point simulation time representation. Accepted values are -18..0; for example, -6 selects microsecond resolution. -12 means picosecond resolution, with a maximum simtime of ~110 days.");
Register_GlobalConfigOption(CFGID_SIMTIME_RESOLUTION, "simtime-resolution", CFG_CUSTOM, "ps", "Sets the resolution for the 64-bit fixed-point simulation time representation. Accepted values are: second-or-smaller time units (`s`, `ms`, `us`, `ns`, `ps`, `fs` or as), power-of-ten multiples of such units (e.g. 100ms), and base-10 scale exponents in the -18..0 range. The maximum representable simulation time depends on the resolution. The default is picosecond resolution, which offers a range of ~110 days.");
Register_GlobalConfigOption(CFGID_NED_PATH, "ned-path", CFG_PATH, "", "A semicolon-separated list of directories. The directories will be regarded as roots of the NED package hierarchy, and all NED files will be loaded from their subdirectory trees. This option is normally left empty, as the OMNeT++ IDE sets the NED path automatically, and for simulations started outside the IDE it is more convenient to specify it via command-line option (-n) or via environment variable (OMNETPP_NED_PATH, NEDPATH).");
Register_GlobalConfigOption(CFGID_NED_PACKAGE_EXCLUSIONS, "ned-package-exclusions", CFG_CUSTOM, "", "A semicolon-separated list of NED packages to be excluded when loading NED files. Sub-packages of excluded ones are also excluded. Additional items may be specified via the `-x` command-line option and the `OMNETPP_NED_PACKAGE_EXCLUSIONS` environment variable.");
Register_GlobalConfigOption(CFGID_DEBUGGER_ATTACH_ON_STARTUP, "debugger-attach-on-startup", CFG_BOOL, "false", "When set to true, the simulation program will launch an external debugger attached to it (if not already present), allowing you to set breakpoints before proceeding. The debugger command is configurable. Note that debugging (i.e. attaching to) a non-child process needs to be explicitly enabled on some systems, e.g. Ubuntu.");
Register_GlobalConfigOption(CFGID_DEBUGGER_ATTACH_ON_ERROR, "debugger-attach-on-error", CFG_BOOL, "false", "When set to true, runtime errors and crashes will trigger an external debugger to be launched (if not already present), allowing you to perform just-in-time debugging on the simulation process. The debugger command is configurable. Note that debugging (i.e. attaching to) a non-child process needs to be explicitly enabled on some systems, e.g. Ubuntu.");

Register_PerRunConfigOption(CFGID_NETWORK, "network", CFG_STRING, nullptr, "The name of the network to be simulated.  The package name can be omitted if the ini file is in the same directory as the NED file that contains the network.");
Register_PerRunConfigOption(CFGID_WARNINGS, "warnings", CFG_BOOL, "true", "Enables warnings.");
Register_PerRunConfigOptionU(CFGID_SIM_TIME_LIMIT, "sim-time-limit", "s", nullptr, "Stops the simulation when simulation time reaches the given limit. The default is no limit.");
Register_PerRunConfigOptionU(CFGID_CPU_TIME_LIMIT, "cpu-time-limit", "s", nullptr, "Stops the simulation when CPU usage has reached the given limit. The default is no limit. Note: To reduce per-event overhead, this time limit is only checked every N events (by default, N=1024).");
Register_PerRunConfigOptionU(CFGID_REAL_TIME_LIMIT, "real-time-limit", "s", nullptr, "Stops the simulation after the specified amount of time has elapsed. The default is no limit. Note: To reduce per-event overhead, this time limit is only checked every N events (by default, N=1024).");
Register_PerRunConfigOptionU(CFGID_WARMUP_PERIOD, "warmup-period", "s", nullptr, "Length of the initial warm-up period. When set, results belonging to the first x seconds of the simulation will not be recorded into output vectors, and will not be counted into output scalars (see option `**.result-recording-modes`). This option is useful for steady-state simulations. The default is 0s (no warmup period). Note that models that compute and record scalar results manually (via `recordScalar()`) will not automatically obey this setting.");
Register_PerRunConfigOption(CFGID_FINGERPRINT, "fingerprint", CFG_STRING, nullptr, "The expected fingerprints of the simulation. If you need multiple fingerprints, separate them with commas. When provided, the fingerprints will be calculated from the specified properties of simulation events, messages, and statistics during execution, and checked against the provided values. Fingerprints are suitable for crude regression tests. As fingerprints occasionally differ across platforms, more than one value can be specified for a single fingerprint, separated by spaces, and a match with any of them will be accepted. To obtain a fingerprint, enter a dummy value (such as `0000`), and run the simulation.");
Register_PerRunConfigOption(CFGID_FINGERPRINTER_CLASS, "fingerprintcalculator-class", CFG_STRING, "omnetpp::cSingleFingerprintCalculator", "Part of the Envir plugin mechanism: selects the fingerprint calculator class to be used to calculate the simulation fingerprint. The class has to implement the `cFingerprintCalculator` interface.");
Register_PerRunConfigOption(CFGID_RESULT_DIR, "result-dir", CFG_STRING, "results", "Base value for the `${resultdir}` variable, which is used as the default directory for result files (output vector file, output scalar file, eventlog file, etc.). See also the `resultdir-subdivision` config option.");
Register_PerRunConfigOption(CFGID_RECORD_EVENTLOG, "record-eventlog", CFG_BOOL, "false", "Enables recording an eventlog file, which can be later visualized on a sequence chart. See `eventlog-file` option too.");
Register_PerRunConfigOption(CFGID_DEBUG_STATISTICS_RECORDING, "debug-statistics-recording", CFG_BOOL, "false", "Turns on the printing of debugging information related to statistics recording (`@statistic` properties)");
Register_PerRunConfigOption(CFGID_CHECK_SIGNALS, "check-signals", CFG_BOOL, CHECKSIGNALS_DEFAULT, "Controls whether the simulation kernel will validate signals emitted by modules and channels against signal declarations (`@signal` properties) in NED files. The default setting depends on the build type: `true` in DEBUG, and `false` in RELEASE mode.");

Register_PerObjectConfigOption(CFGID_PARTITION_ID, "partition-id", KIND_MODULE, CFG_STRING, nullptr, "With parallel simulation: in which partition the module should be instantiated. Specify numeric partition ID, or a comma-separated list of partition IDs for compound modules that span across multiple partitions. Ranges (`5..9`) and `*` (=all) are accepted too.");
Register_PerObjectConfigOption(CFGID_RNG_K, "rng-%", KIND_COMPONENT, CFG_INT, "", "Maps a module-local RNG to one of the global RNGs. Example: `**.gen.rng-1=3` maps the local RNG 1 of modules matching `**.gen` to the global RNG 3. The value may be an expression, with the `index` and `ancestorIndex()` operators being potentially very useful. The default is one-to-one mapping, i.e. RNG k of all modules refer to the global RNG k (`for k=0..num-rngs-1`).\nUsage: `<module-full-path>.rng-<local-index>=<global-index>`. Examples: `**.mac.rng-0=1; **.source[*].rng-0=index`");

Register_PerRunConfigOption(CFGID_OUTPUT_SCALAR_FILE, "output-scalar-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.sca", "Name for the output scalar file.");
Register_PerRunConfigOption(CFGID_OUTPUT_SCALAR_FILE_APPEND, "output-scalar-file-append", CFG_BOOL, "false", "What to do when the output scalar file already exists: append to it (OMNeT++ 3.x behavior), or delete it and begin a new file (default).");
Register_PerRunConfigOption(CFGID_OUTPUT_SCALAR_PRECISION, "output-scalar-precision", CFG_INT, DEFAULT_OUTPUT_SCALAR_PRECISION, "The number of significant digits for recording data into the output scalar file. The maximum value is ~15 (IEEE double precision). This has no effect on SQLite recording, as it stores values as 8-byte IEEE floating point numbers.");

Register_PerObjectConfigOption(CFGID_SCALAR_RECORDING, "scalar-recording", KIND_SCALAR, CFG_BOOL, "true", "Whether the matching output scalars and statistic objects should be recorded.\nUsage: `<module-full-path>.<scalar-name>.scalar-recording=true/false`. To enable/disable individual recording modes for a @statistic (those added via the `record=...` key of `@statistic` or the `**.result-recording-modes=...` config option), use `<statistic-name>:<mode>` for `<scalar-name>`, and make sure the `@statistic` as a whole is not disabled with `**.<statistic-name>.statistic-recording=false`.\nExample: `**.ping.roundTripTime:stddev.scalar-recording=false`");
Register_PerObjectConfigOption(CFGID_BIN_RECORDING, "bin-recording", KIND_SCALAR, CFG_BOOL, "true", "Whether the bins of the matching histogram object should be recorded, provided that recording of the histogram object itself is enabled (`**.<scalar-name>.scalar-recording=true`).\nUsage: `<module-full-path>.<scalar-name>.bin-recording=true/false`. To control histogram recording from a `@statistic`, use `<statistic-name>:histogram` for `<scalar-name>`.\nExample: `**.ping.roundTripTime:histogram.bin-recording=false`");
Register_PerObjectConfigOption(CFGID_PARAM_RECORDING, "param-recording", KIND_PARAMETER, CFG_BOOL, "true", "Whether the matching module (and channel) parameters should be recorded.\nUsage: `<module-full-path>.<parameter-name>.param-recording=true/false`.\nExample: `**.app.pkLen.param-recording=true`");

Register_PerRunConfigOption(CFGID_OUTPUT_VECTOR_FILE, "output-vector-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.vec", "Name for the output vector file.");
Register_PerRunConfigOption(CFGID_OUTPUT_VECTOR_FILE_APPEND, "output-vector-file-append", CFG_BOOL, "false", "What to do when the output vector file already exists: append to it, or delete it and begin a new file (default). Note: `cIndexedFileOutputVectorManager` currently does not support appending.");
Register_PerRunConfigOption(CFGID_OUTPUT_VECTOR_PRECISION, "output-vector-precision", CFG_INT, DEFAULT_OUTPUT_VECTOR_PRECISION, "The number of significant digits for recording data into the output vector file. The maximum value is ~15 (IEEE double precision). This setting has no effect on SQLite recording (it stores values as 8-byte IEEE floating point numbers), and for the \"time\" column which is represented as fixed-point numbers and always get recorded precisely.");

Register_PerObjectConfigOption(CFGID_VECTOR_RECORDING, "vector-recording", KIND_VECTOR, CFG_BOOL, "true", "Whether data written into an output vector should be recorded.\nUsage: `<module-full-path>.<vector-name>.vector-recording=true/false`. To control vector recording from a `@statistic`, use `<statistic-name>:vector for <vector-name>`. Example: `**.ping.roundTripTime:vector.vector-recording=false`");
Register_PerObjectConfigOption(CFGID_VECTOR_RECORD_EVENTNUMBERS, "vector-record-eventnumbers", KIND_VECTOR, CFG_BOOL, "true", "Whether to record event numbers for an output vector. (Values and timestamps are always recorded.) Event numbers are needed by the Sequence Chart Tool, for example.\nUsage: `<module-full-path>.<vector-name>.vector-record-eventnumbers=true/false`.\nExample: `**.ping.roundTripTime:vector.vector-record-eventnumbers=false`");
Register_PerObjectConfigOption(CFGID_VECTOR_RECORDING_INTERVALS, "vector-recording-intervals", KIND_VECTOR, CFG_CUSTOM, nullptr, "Allows one to restrict recording of an output vector to one or more simulation time intervals. Usage: `<module-full-path>.<vector-name>.vector-recording-intervals=<intervals>`. The syntax for `<intervals>` is: `[<from>]..[<to>],...` That is, both start and end of an interval are optional, and intervals are separated by comma.\nExample: `**.roundTripTime:vector.vector-recording-intervals=..100, 200..400, 900..`");
Register_PerRunConfigOptionU(CFGID_OUTPUTVECTOR_MEMORY_LIMIT, "output-vectors-memory-limit", "B", DEFAULT_OUTPUT_VECTOR_MEMORY_LIMIT, "Total memory that can be used for buffering output vectors. Larger values produce less fragmented vector files (i.e. cause vector data to be grouped into larger chunks), and therefore allow more efficient processing later. There is also a per-vector limit, see `**.vector-buffer`.");
Register_PerObjectConfigOptionU(CFGID_VECTOR_BUFFER, "vector-buffer", KIND_VECTOR, "B", DEFAULT_VECTOR_BUFFER, "For output vectors: the maximum per-vector buffer space used for storing values before writing them out as a block into the output vector file. There is also a total limit, see `output-vectors-memory-limit`.\nUsage: `<module-full-path>.<vector-name>.vector-buffer=<amount>`.");


EnvirBase::EnvirBase() : out(std::cout.rdbuf())
{
    opt = nullptr;
    args = nullptr;
    cfg = nullptr;
    xmlCache = nullptr;

    recordEventlog = false;
    eventlogManager = nullptr;
    outvectorManager = nullptr;
    outScalarManager = nullptr;
    snapshotManager = nullptr;

#ifdef WITH_PARSIM
    parsimComm = nullptr;
    parsimPartition = nullptr;
#endif
}

EnvirBase::~EnvirBase()
{
    while (!listeners.empty()) {
        listeners.back()->listenerRemoved();
        listeners.pop_back();
    }

    delete opt;
    delete args;
    delete cfg;
    delete xmlCache;

    delete eventlogManager;
    delete outvectorManager;
    delete outScalarManager;
    delete snapshotManager;

#ifdef WITH_PARSIM
    delete parsimComm;
    delete parsimPartition;
#endif
}

void EnvirBase::preconfigure(cComponent *component)
{
}

void EnvirBase::configure(cComponent *component)
{
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
    const cConfiguration::KeyValue& entry = getConfigEx()->getParameterEntry(moduleFullPath.c_str(), par->getName(), par->containsValue());
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

bool EnvirBase::isModuleLocal(cModule *parentmod, const char *modname, int index)
{
#ifdef WITH_PARSIM
    if (!opt->parsim)
        return true;

    // toplevel module is local everywhere
    if (!parentmod)
        return true;

    // find out if this module is (or has any submodules that are) on this partition
    char parname[MAX_OBJECTFULLPATH];
    if (index < 0)
        sprintf(parname, "%s.%s", parentmod->getFullPath().c_str(), modname);
    else
        sprintf(parname, "%s.%s[%d]", parentmod->getFullPath().c_str(), modname, index);  // FIXME this is incorrectly chosen for non-vector modules too!
    std::string procIds = getConfig()->getAsString(parname, CFGID_PARTITION_ID, "");
    if (procIds.empty()) {
        // modules inherit the setting from their parents, except when the parent is the system module (the network) itself
        if (!parentmod->getParentModule())
            throw cRuntimeError("Incomplete partitioning: Missing value for '%s'", parname);
        // "true" means "inherit", because an ancestor which answered "false" doesn't get recursed into
        return true;
    }
    else if (strcmp(procIds.c_str(), "*") == 0) {
        // present on all partitions (provided that ancestors have "*" set as well)
        return true;
    }
    else {
        // we expect a partition Id (or partition Ids, separated by commas) where this
        // module needs to be instantiated. So we return true if any of the numbers
        // is the Id of the local partition, otherwise false.
        EnumStringIterator procIdIter(procIds.c_str());
        if (procIdIter.hasError())
            throw cRuntimeError("Wrong partitioning: Syntax error in value '%s' for '%s' "
                                "(allowed syntax: '', '*', '1', '0,3,5-7')",
                    procIds.c_str(), parname);
        int numPartitions = parsimComm->getNumPartitions();
        int myProcId = parsimComm->getProcId();
        for ( ; procIdIter() != -1; procIdIter++) {
            if (procIdIter() >= numPartitions)
                throw cRuntimeError("Wrong partitioning: Value %d too large for '%s' (total partitions=%d)",
                        procIdIter(), parname, numPartitions);
            if (procIdIter() == myProcId)
                return true;
        }
        return false;
    }
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

cConfiguration *EnvirBase::getConfig()
{
    return cfg;
}

cConfigurationEx *EnvirBase::getConfigEx()
{
    return cfg;
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
    for (std::string dir : opp_splitpath(opt->nedPath)) {
        std::string path = concatDirAndFile(dir.c_str(), fileName);
        if (fileExists(path.c_str()))
            return tidyFilename(path.c_str());
    }

    // search the image path
    for (std::string dir : opp_splitpath(opt->imagePath)) {
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
}

void EnvirBase::objectDeleted(cObject *object)
{
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
}

void EnvirBase::componentInitBegin(cComponent *component, int stage)
{
    // To make initialization similar to processed events in this regard.
    // Unfortunately, for modules created (initialized) dynamically,
    // this moduleId will be stuck like this for the rest of the event,
    // processed by a different module.
    currentModuleId = (component && component->isModule()) ? component->getId() : -1;
}

void EnvirBase::beginSend(cMessage *msg, const SendOptions& options)
{
    if (recordEventlog)
        eventlogManager->beginSend(msg, options);
}

void EnvirBase::messageScheduled(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageScheduled(msg);
}

void EnvirBase::messageCancelled(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageCancelled(msg);
}

void EnvirBase::messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result)
{
    if (recordEventlog)
        eventlogManager->messageSendDirect(msg, toGate, result);
}

void EnvirBase::messageSendHop(cMessage *msg, cGate *srcGate)
{
    if (recordEventlog)
        eventlogManager->messageSendHop(msg, srcGate);
}

void EnvirBase::messageSendHop(cMessage *msg, cGate *srcGate, const cChannel::Result& result)
{
    if (recordEventlog)
        eventlogManager->messageSendHop(msg, srcGate, result);
}

void EnvirBase::endSend(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->endSend(msg);
}

void EnvirBase::messageCreated(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageCreated(msg);
}

void EnvirBase::messageCloned(cMessage *msg, cMessage *clone)
{
    if (recordEventlog)
        eventlogManager->messageCloned(msg, clone);
}

void EnvirBase::messageDeleted(cMessage *msg)
{
    if (recordEventlog)
        eventlogManager->messageDeleted(msg);
}

void EnvirBase::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va, bool silent)
{
    if (recordEventlog)
        eventlogManager->componentMethodBegin(from, to, methodFmt, va);
}

void EnvirBase::componentMethodEnd()
{
    if (recordEventlog)
        eventlogManager->componentMethodEnd();
}

void EnvirBase::moduleCreated(cModule *newmodule)
{
    if (recordEventlog)
        eventlogManager->moduleCreated(newmodule);
}

void EnvirBase::moduleDeleted(cModule *module)
{
    if (recordEventlog)
        eventlogManager->moduleDeleted(module);
}

void EnvirBase::moduleReparented(cModule *module, cModule *oldparent, int oldId)
{
    if (recordEventlog)
        eventlogManager->moduleReparented(module, oldparent, oldId);
}

void EnvirBase::gateCreated(cGate *newgate)
{
    if (recordEventlog)
        eventlogManager->gateCreated(newgate);
}

void EnvirBase::gateDeleted(cGate *gate)
{
    if (recordEventlog)
        eventlogManager->gateDeleted(gate);
}

void EnvirBase::connectionCreated(cGate *srcgate)
{
    if (recordEventlog)
        eventlogManager->connectionCreated(srcgate);
}

void EnvirBase::connectionDeleted(cGate *srcgate)
{
    if (recordEventlog)
        eventlogManager->connectionDeleted(srcgate);
}

void EnvirBase::displayStringChanged(cComponent *component)
{
    if (recordEventlog)
        eventlogManager->displayStringChanged(component);
}

void EnvirBase::log(cLogEntry *entry)
{
    if (recordEventlog) {
        std::string prefix = logFormatter.formatPrefix(entry);
        eventlogManager->logLine(prefix.c_str(), entry->text, entry->textLength);
    }
}

void EnvirBase::undisposedObject(cObject *obj)
{
    if (opt->printUndisposed)
        out << "undisposed object: (" << obj->getClassName() << ") " << obj->getFullPath() << " -- check module destructor" << endl;
}

void EnvirBase::readOptions()
{
    cConfiguration *cfg = getConfig();

    opt->parsim = cfg->getAsBool(CFGID_PARALLEL_SIMULATION);

#ifndef WITH_PARSIM
    if (opt->parsim)
        throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif

    attachDebuggerOnErrors = cfg->getAsBool(CFGID_DEBUGGER_ATTACH_ON_ERROR);

    // simtime resolution
    bool hasSimtimeResolution = cfg->getConfigValue(CFGID_SIMTIME_RESOLUTION->getName()) != nullptr;
    bool hasSimtimeScale = cfg->getConfigValue(CFGID_SIMTIME_SCALE->getName()) != nullptr;
    int exp;
    if (hasSimtimeResolution || !hasSimtimeScale)
        exp = parseSimtimeResolution(cfg->getAsCustom(CFGID_SIMTIME_RESOLUTION)); // new
    else
        exp = (int)cfg->getAsInt(CFGID_SIMTIME_SCALE); // legacy
    SimTime::setScaleExp(exp);
    if (hasSimtimeScale)
        printfmsg("Warning: Obsolete config option %s= found, please use the improved %s= instead "
                "(it allows values like \"us\" or \"100ps\" in addition to base-10 scale exponents)",
                CFGID_SIMTIME_SCALE->getName(), CFGID_SIMTIME_RESOLUTION->getName());

    // NED path. It is taken from the "-n" command-line options, the OMNETPP_NED_PATH
    // environment variable, and the "ned-path=" config option. If the result is still
    // empty, we fall back to "." -- this is needed for single-directory models to work.
    std::string nedPath = opp_join(args->optionValues('n'), ";", true);
    nedPath = opp_join(";", nedPath, getConfig()->getAsPath(CFGID_NED_PATH));
    nedPath = opp_join(";", nedPath, opp_nulltoempty(getenv("OMNETPP_NED_PATH")));
    nedPath = opp_join(";", nedPath, opp_nulltoempty(getenv("NEDPATH")));
    if (nedPath.empty())
        nedPath = ".";
    opt->nedPath = nedPath;

    // NED packages to exclude
    std::string nedExcludedPackages = opp_join(args->optionValues('x'), ";", true);
    nedExcludedPackages = opp_join(";", nedExcludedPackages, opp_nulltoempty(getConfig()->getAsCustom(CFGID_NED_PACKAGE_EXCLUSIONS)));
    nedExcludedPackages = opp_join(";", nedExcludedPackages, opp_nulltoempty(getenv("OMNETPP_NED_PACKAGE_EXCLUSIONS")));
    opt->nedExcludedPackages = nedExcludedPackages;

    // Image path similarly to NED path, except that we have compile-time default as well,
    // in the OMNETPP_IMAGE_PATH macro.
    std::string imagePath;
    for (std::string opt : args->optionValues('i'))
        imagePath = opp_join(";", imagePath, opt);
    imagePath = opp_join(";", imagePath, getConfig()->getAsPath(CFGID_IMAGE_PATH));
    std::string builtinImagePath = opp_removestart(OMNETPP_IMAGE_PATH, "/;"); // strip away the /; sequence from the beginning (a workaround for MinGW path conversion). See #785
    imagePath = opp_join(";", imagePath, opp_emptytodefault(getenv("OMNETPP_IMAGE_PATH"), builtinImagePath.c_str()));
    opt->imagePath = imagePath;

    // other options are read on per-run basis
}

void EnvirBase::readPerRunOptions()
{
    cConfiguration *cfg = getConfig();

    // get options from ini file
    opt->checkSignals = cfg->getAsBool(CFGID_CHECK_SIGNALS);
    debugOnErrors = cfg->getAsBool(CFGID_DEBUG_ON_ERRORS);  // note: handling overridden in Qtenv::readPerRunOptions() due to interference with GUI
    opt->printUndisposed = cfg->getAsBool(CFGID_PRINT_UNDISPOSED);
    getSimulation()->setParameterMutabilityCheck(cfg->getAsBool(CFGID_PARAMETER_MUTABILITY_CHECK));

    // install eventlog manager
    delete eventlogManager;
    std::string eventlogManagerClass = cfg->getAsString(CFGID_EVENTLOGMANAGER_CLASS);
    eventlogManager = createByClassName<cIEventlogManager>(eventlogManagerClass.c_str(), "eventlog manager");
    eventlogManager->configure(cfg);
    addLifecycleListener(eventlogManager);

    // install output vector manager
    delete outvectorManager;
    std::string outputVectorManagerClass = cfg->getAsString(CFGID_OUTPUTVECTORMANAGER_CLASS);
    outvectorManager = createByClassName<cIOutputVectorManager>(outputVectorManagerClass.c_str(), "output vector manager");
    outvectorManager->configure(cfg);
    addLifecycleListener(outvectorManager);

    // install output scalar manager
    delete outScalarManager;
    std::string outputScalarManagerClass = cfg->getAsString(CFGID_OUTPUTSCALARMANAGER_CLASS);
    outScalarManager = createByClassName<cIOutputScalarManager>(outputScalarManagerClass.c_str(), "output scalar manager");
    outScalarManager->configure(cfg);
    addLifecycleListener(outScalarManager);

    // install snapshot manager
    delete snapshotManager;
    std::string snapshotmanagerClass = cfg->getAsString(CFGID_SNAPSHOTMANAGER_CLASS);
    snapshotManager = createByClassName<cISnapshotManager>(snapshotmanagerClass.c_str(), "snapshot manager");
    snapshotManager->configure(cfg);
    addLifecycleListener(snapshotManager);

    // install FES
    std::string futureeventsetClass = cfg->getAsString(CFGID_FUTUREEVENTSET_CLASS);
    cFutureEventSet *fes = createByClassName<cFutureEventSet>(futureeventsetClass.c_str(), "FES");
    getSimulation()->setFES(fes);

    // install scheduler
    if (!opt->parsim) {
        std::string schedulerClass = cfg->getAsString(CFGID_SCHEDULER_CLASS);
        cScheduler *scheduler = createByClassName<cScheduler>(schedulerClass.c_str(), "event scheduler");
        getSimulation()->setScheduler(scheduler);
    }

    // install fingerprint calculator object
    cFingerprintCalculator *fingerprint = nullptr;
    std::string expectedFingerprints = cfg->getAsString(CFGID_FINGERPRINT);
    if (!expectedFingerprints.empty()) {
        // create calculator
        std::string fingerprintClass = cfg->getAsString(CFGID_FINGERPRINTER_CLASS);
        fingerprint = createByClassName<cFingerprintCalculator>(fingerprintClass.c_str(), "fingerprint calculator");
        if (expectedFingerprints.find(',') != expectedFingerprints.npos)
            fingerprint = new cMultiFingerprintCalculator(fingerprint);
        fingerprint->initialize(expectedFingerprints.c_str(), cfg);
    }
    getSimulation()->setFingerprintCalculator(fingerprint);

    cComponent::setCheckSignals(opt->checkSignals);

    getSimulation()->getRngManager()->configure(cfg);

    // init nextUniqueNumber -- startRun() is too late because simple module ctors have run by then
    nextUniqueNumber = 0;
#ifdef WITH_PARSIM
    if (opt->parsim)
        nextUniqueNumber = (unsigned)parsimComm->getProcId() * ((~0UL) / (unsigned)parsimComm->getNumPartitions());
#endif

    recordEventlog = cfg->getAsBool(CFGID_RECORD_EVENTLOG);
}

int EnvirBase::parseSimtimeResolution(const char *resolution)
{
    try {
        // Three possibilities: <unit-only>, <number-with-unit>, <number-only>
        const double INVALID_EXP = 1e100;
        double exp = INVALID_EXP;
        if (opp_isalpha(resolution[0])) {
            // try as <unit-only>, e.g. "ms"
            double f = UnitConversion::getConversionFactor(resolution, "s");
            if (f == 0)
                throw std::runtime_error("Wrong unit");
            exp = log10(f);
            ASSERT(exp == floor(exp));
        }
        else {
            // try as <number-only>: this will be an exponent, e.g. -12
            try { exp = opp_atol(resolution); } catch (std::exception& e) {}

            // try as <number-with-unit>, e.g. "100ps"
            if (exp == INVALID_EXP) {
                double f = UnitConversion::parseQuantity(resolution, "s");
                exp = log10(f);
                if (exp != floor(exp))
                    throw std::runtime_error("Not power of 10");
            }
        }
        return (int)exp;
    }
    catch (std::exception& e) {
        throw cRuntimeError(
                "Invalid value \"%s\" for configuration option simtime-resolution: it must be "
                "a valid second-or-smaller time unit (s, ms, us, ns, ps, fs or as), "
                "a power-of-ten multiple of such unit (e.g. 100ms), or a base-10 scale "
                "exponent in the -18..0 range. (Details: %s)", resolution, e.what());
    }
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

//-------------------------------------------------------------

void *EnvirBase::registerOutputVector(const char *modulename, const char *vectorname)
{
    ASSERT(outvectorManager);
    return outvectorManager->registerVector(modulename, vectorname);
}

void EnvirBase::deregisterOutputVector(void *vechandle)
{
    ASSERT(outvectorManager);
    outvectorManager->deregisterVector(vechandle);
}

void EnvirBase::setVectorAttribute(void *vechandle, const char *name, const char *value)
{
    ASSERT(outvectorManager);
    outvectorManager->setVectorAttribute(vechandle, name, value);
}

bool EnvirBase::recordInOutputVector(void *vechandle, simtime_t t, double value)
{
    ASSERT(outvectorManager);
    if (getSimulation()->getFingerprintCalculator())
        // TODO: determine component and result name if possible
        getSimulation()->getFingerprintCalculator()->addVectorResult(nullptr, "", t, value);
    return outvectorManager->record(vechandle, t, value);
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

unsigned long EnvirBase::getUniqueNumber()
{
    unsigned long ret = nextUniqueNumber++;
    if (nextUniqueNumber == 0)
        throw cRuntimeError("getUniqueNumber(): All values have been consumed");
#ifdef WITH_PARSIM
    if (opt->parsim) {
        unsigned long limit = (unsigned)(parsimComm->getProcId()+1) * ((~0UL) / (unsigned)parsimComm->getNumPartitions());
        if (nextUniqueNumber == limit)
            throw cRuntimeError("getUniqueNumber(): All values have been consumed");
    }
#endif
    return ret;
}


bool EnvirBase::idle()
{
    return false;
}

bool EnvirBase::ensureDebugger(cRuntimeError *error)
{
    return false; // note: this is implemented in RunnableEnvir
}

int EnvirBase::getParsimProcId() const
{
#ifdef WITH_PARSIM
    return parsimComm ? parsimComm->getProcId() : 0;
#else
    return 0;
#endif
}

int EnvirBase::getParsimNumPartitions() const
{
#ifdef WITH_PARSIM
    return parsimComm ? parsimComm->getNumPartitions() : 0;
#else
    return 0;
#endif
}



}  // namespace envir
}  // namespace omnetpp

