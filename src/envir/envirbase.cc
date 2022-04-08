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
#include "omnetpp/crng.h"
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

#ifdef __APPLE__
// these are needed for debugger detection
#include <stdbool.h>

// these are for the alternative method - using ptrace
// #include <sys/types.h>
// #include <unistd.h>
// #include <sys/sysctl.h>

#include <mach/task.h>
#include <mach/mach_init.h>
#endif

#ifdef WITH_PARSIM
#include "omnetpp/cparsimcomm.h"
#include "sim/parsim/cparsimpartition.h"
#include "sim/parsim/cparsimsynchr.h"
#include "sim/parsim/creceivedexception.h"
#endif

#ifdef USE_PORTABLE_COROUTINES  /* coroutine stacks reside in main stack area */

# define TOTAL_STACK_SIZE    (2*1024*1024)
# define MAIN_STACK_SIZE     (128*1024)

#else /* nonportable coroutines, stacks are allocated on heap */

# define TOTAL_STACK_SIZE    0  // dummy value
# define MAIN_STACK_SIZE     0  // dummy value

#endif

#ifdef _WIN32
#define DEFAULT_DEBUGGER_COMMAND    "bash -c \"opp_ide omnetpp://cdt/debugger/attach?pid=%u\""
#else
#define DEFAULT_DEBUGGER_COMMAND    "opp_ide omnetpp://cdt/debugger/attach?pid=%u"
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
Register_GlobalConfigOption(CFGID_PARSIM_NUM_PARTITIONS, "parsim-num-partitions", CFG_INT, nullptr, "If `parallel-simulation=true`, it tells the number of parallel processes to use. This value must be in agreement with the number of simulator instances launched, e.g. with the `-n` or `-np` command-line option specified to the `mpirun` program.");
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
Register_GlobalConfigOption(CFGID_DEBUGGER_ATTACH_COMMAND, "debugger-attach-command", CFG_STRING, nullptr, "The command line to launch the debugger. It must contain exactly one percent sign, as `%u`, which will be replaced by the PID of this process. The command must not block (i.e. it should end in `&` on Unix-like systems). It will be executed by the default system shell (on Windows, usually cmd.exe). Default on this platform: `" DEFAULT_DEBUGGER_COMMAND "`. This default can be overridden with the `OMNETPP_DEBUGGER_COMMAND` environment variable.");
Register_GlobalConfigOptionU(CFGID_DEBUGGER_ATTACH_WAIT_TIME, "debugger-attach-wait-time", "s", "20s", "An interval to wait after launching the external debugger, to give the debugger time to start up and attach to the simulation process.");

Register_PerRunConfigOption(CFGID_NETWORK, "network", CFG_STRING, nullptr, "The name of the network to be simulated.  The package name can be omitted if the ini file is in the same directory as the NED file that contains the network.");
Register_PerRunConfigOption(CFGID_WARNINGS, "warnings", CFG_BOOL, "true", "Enables warnings.");
Register_PerRunConfigOptionU(CFGID_SIM_TIME_LIMIT, "sim-time-limit", "s", nullptr, "Stops the simulation when simulation time reaches the given limit. The default is no limit.");
Register_PerRunConfigOptionU(CFGID_CPU_TIME_LIMIT, "cpu-time-limit", "s", nullptr, "Stops the simulation when CPU usage has reached the given limit. The default is no limit. Note: To reduce per-event overhead, this time limit is only checked every N events (by default, N=1024).");
Register_PerRunConfigOptionU(CFGID_REAL_TIME_LIMIT, "real-time-limit", "s", nullptr, "Stops the simulation after the specified amount of time has elapsed. The default is no limit. Note: To reduce per-event overhead, this time limit is only checked every N events (by default, N=1024).");
Register_PerRunConfigOptionU(CFGID_WARMUP_PERIOD, "warmup-period", "s", nullptr, "Length of the initial warm-up period. When set, results belonging to the first x seconds of the simulation will not be recorded into output vectors, and will not be counted into output scalars (see option `**.result-recording-modes`). This option is useful for steady-state simulations. The default is 0s (no warmup period). Note that models that compute and record scalar results manually (via `recordScalar()`) will not automatically obey this setting.");
Register_PerRunConfigOption(CFGID_FINGERPRINT, "fingerprint", CFG_STRING, nullptr, "The expected fingerprints of the simulation. If you need multiple fingerprints, separate them with commas. When provided, the fingerprints will be calculated from the specified properties of simulation events, messages, and statistics during execution, and checked against the provided values. Fingerprints are suitable for crude regression tests. As fingerprints occasionally differ across platforms, more than one value can be specified for a single fingerprint, separated by spaces, and a match with any of them will be accepted. To obtain a fingerprint, enter a dummy value (such as `0000`), and run the simulation.");
Register_PerRunConfigOption(CFGID_FINGERPRINTER_CLASS, "fingerprintcalculator-class", CFG_STRING, "omnetpp::cSingleFingerprintCalculator", "Part of the Envir plugin mechanism: selects the fingerprint calculator class to be used to calculate the simulation fingerprint. The class has to implement the `cFingerprintCalculator` interface.");
Register_PerRunConfigOption(CFGID_NUM_RNGS, "num-rngs", CFG_INT, "1", "The number of random number generators.");
Register_PerRunConfigOption(CFGID_RNG_CLASS, "rng-class", CFG_STRING, "omnetpp::cMersenneTwister", "The random number generator class to be used. It can be `cMersenneTwister`, `cLCG32`, `cAkaroaRNG`, or you can use your own RNG class (it must be subclassed from `cRNG`).");
Register_PerRunConfigOption(CFGID_SEED_SET, "seed-set", CFG_INT, "${runnumber}", "Selects the kth set of automatic random number seeds for the simulation. Meaningful values include `${repetition}` which is the repeat loop counter (see `repeat` option), and `${runnumber}`.");
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


template<class T>
T *createByClassName(const char *className, const char *what) {
    cObject *obj = createOneIfClassIsKnown(className);
    if (obj == nullptr)
        throw cRuntimeError("Cannot create %s: class \"%s\" not found", what, className);
    T *result = dynamic_cast<T *>(obj);
    if (result == nullptr)
        throw cRuntimeError("Cannot create %s: class \"%s\" is not subclassed from %s", what, className, opp_typename(typeid(T)));
    return result;
}


#define STRINGIZE0(x)    #x
#define STRINGIZE(x)     STRINGIZE0(x)

static const char *compilerInfo =
    #if defined __clang__
    "CLANG " __clang_version__
    #elif defined __GNUC__
    "GCC " __VERSION__
    #elif defined _MSC_VER
    "Microsoft Visual C++ version " STRINGIZE(_MSC_VER)
    #elif defined __INTEL_COMPILER
    "Intel Compiler version " STRINGIZE(__INTEL_COMPILER)
    #else
    "unknown-compiler"
    #endif
    ;

static const char *buildInfoFormat =
    "%d-bit"

    #ifdef __x86_64__
    " ARCH_X86_64"
    #elif __i386__
    " ARCH_I386"
    #elif __aarch64__
    " ARCH_AARCH64"
    #else
    " ARCH_UNKNOWN"
    #endif

    #ifdef NDEBUG
    " RELEASE"
    #else
    " DEBUG"
    #endif
    ;

static const char *buildOptions = ""
    #ifdef WITH_PARSIM
    " WITH_PARSIM"
    #endif

    #ifdef WITH_LIBXML
    " WITH_LIBXML"
    #endif

    #ifdef WITH_MPI
    " WITH_MPI"
    #endif

    #ifdef WITH_NETBUILDER
    " WITH_NETBUILDER"
    #endif

    #ifdef WITH_QTENV
    " WITH_QTENV"
    #endif

    #ifdef WITH_OSG
    " WITH_OSG"
    #endif

    #ifdef WITH_OSGEARTH
    " WITH_OSGEARTH"
    #endif

    #ifdef WITH_AKAROA
    " WITH_AKAROA"
    #endif
    ;


EnvirOptions::EnvirOptions()
{
    // note: these values will be overwritten in setup()/readOptions() before taking effect
    totalStack = 0;
    parsim = false;
    numRNGs = 1;
    seedset = 0;
    debugStatisticsRecording = false;
    checkSignals = false;
    fnameAppendHost = false;
    warnings = true;
    verbose = true;
    useStderr = true;
    printUndisposed = true;
    realTimeLimit = 0;
    cpuTimeLimit = 0;
}

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

    numRNGs = 0;
    rngs = nullptr;

#ifdef WITH_PARSIM
    parsimComm = nullptr;
    parsimPartition = nullptr;
#endif

    exitCode = 0;
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

    for (int i = 0; i < numRNGs; i++)
        delete rngs[i];
    delete[] rngs;

#ifdef WITH_PARSIM
    delete parsimComm;
    delete parsimPartition;
#endif
}

int EnvirBase::run(int argc, char *argv[], cConfiguration *configobject)
{
    opt = createOptions();
    args = new ArgList();
    args->parse(argc, argv, ARGSPEC);
    opt->useStderr = !args->optionGiven('m');
    opt->verbose = !args->optionGiven('s');
    cfg = dynamic_cast<cConfigurationEx *>(configobject);
    if (!cfg)
        throw cRuntimeError("Cannot cast configuration object %s to cConfigurationEx", configobject->getClassName());
    if (cfg->getAsBool(CFGID_DEBUGGER_ATTACH_ON_STARTUP) && detectDebugger() != DebuggerPresence::PRESENT)
        attachDebugger();

    if (simulationRequired()) {
        if (setup())
            run();  // must not throw, because we want shutdown() always to be called
        shutdown();
    }
    return exitCode;
}

bool EnvirBase::simulationRequired()
{
    // handle -h and -v command-line options
    if (args->optionGiven('h')) {
        const char *category = args->optionValue('h', 0);
        if (!category)
            printHelp();
        else
            EnvirUtils::dumpComponentList(out, category, opt->verbose);
        return false;
    }

    if (args->optionGiven('v')) {
        out << "Build: " OMNETPP_RELEASE " " OMNETPP_BUILDID << "" << endl;
        out << "Compiler: " << compilerInfo << "" << endl;
        out << "Options: " << opp_stringf(buildInfoFormat, 8*sizeof(void *));
        out << buildOptions << endl;
        return false;
    }

    cConfigurationEx *cfg = getConfigEx();

    // -a option: print all config names, and number of runs in them
    if (args->optionGiven('a')) {
        std::vector<std::string> configNames = cfg->getConfigNames();
        for (auto & configName : configNames)
            out << "Config " << configName << ": " << cfg->getNumRunsInConfig(configName.c_str()) << "" << endl;
        return false;
    }

    // try processing -q option
    const char *configName = args->optionValue('c');
    const char *runFilter = args->optionValue('r');
    const char *query = args->optionValue('q');
    const char *configOptionToEcho = args->optionValue('e');

    // process -q
    if (query) {
        if (!configName)
            configName = "General";
        printRunInfo(configName, runFilter, query);
        return false;
    }

    // process -e
    if (configOptionToEcho) {
        if (!configName)
            configName = "General";
        printConfigValue(configName, runFilter, configOptionToEcho);
        return false;
    }

    return true;
}

void EnvirBase::printRunInfo(const char *configName, const char *runFilter, const char *query)
{
    //
    // IMPORTANT: the simulation launcher will parse the output of this
    // option, so it should be modified with care and the two kept in sync
    // (see OmnetppLaunchUtils.getSimulationRunInfo()).
    //
    // Rules:
    // (1) the number of runs should appear on the rest of the line
    //     after the "Number of runs:" text
    // (2) per-run information lines should span from the "Number of runs:"
    //     line until the next blank line ("\n\n").
    //

    std::string q = opp_strlower(query); // make match case-insensitive

    if (q.find("run") != q.npos) {
        std::vector<int> runNumbers = resolveRunFilter(configName, runFilter);

        if (opt->verbose) {
            out <<"Config: " << configName << endl;
            out <<"Number of runs: " << cfg->getNumRunsInConfig(configName) << endl;
            if (!opp_isblank(runFilter))
                out << "Number of runs selected: " << runNumbers.size() << endl;
        }

        std::vector<cConfiguration::RunInfo> runInfos = cfg->unrollConfig(configName);
        if (q == "numruns") {
            if (!opt->verbose) // otherwise it was already printed
                out << runNumbers.size() << endl;
        }
        else if (q == "runnumbers") {
            if (opt->verbose) {
                out << endl;
                out << "Run numbers:";
            }
            for (int runNumber : runNumbers)
                out << " " << runNumber;
            out << endl;
        }
        else if (q == "runs") {
            if (opt->verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const cConfiguration::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
            }
        }
        else if (q == "rundetails") {
            if (opt->verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const cConfiguration::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
                out << opp_indentlines(runInfo.configBrief, "\t");
                if (runNumber != runNumbers.back())
                    out << endl;
            }
        }
        else if (q == "runconfig") {
            if (opt->verbose)
                out << endl;
            for (int runNumber : runNumbers) {
                const cConfiguration::RunInfo& runInfo = runInfos[runNumber];
                out << "Run " << runNumber << ": " << runInfo.info << endl;
                cfg->activateConfig(configName, runNumber);
                std::vector<const char *> keysValues = cfg->getKeyValuePairs(cConfigurationEx::FILT_ALL);
                for (int i = 0; i < (int)keysValues.size(); i += 2) {
                    const char *key = keysValues[i];
                    const char *value = keysValues[i+1];
                    out << "\t" << key << " = " << value << endl;
                }
                if (runNumber != runNumbers.back())
                    out << endl;
            }
        }
        else {
            err() << "Unrecognized -q argument '" << q << "'" << endl;
            exitCode = 1;
        }
    }
    else if (q == "sectioninheritance") {
        if (opt->verbose)
            out << endl;
        std::vector<std::string> configNames = cfg->getConfigChain(configName);
        for (auto configName : configNames) {
            if (configName != "General")
                configName = "Config " + configName;
            out << configName << endl;
        }
    }
    else {
        err() << "Unrecognized -q argument '" << q << "'" << endl;
        exitCode = 1;
    }
}

void EnvirBase::printConfigValue(const char *configName, const char *runFilter, const char *optionName)
{
    // activate the selected configuration
    std::vector<int> runNumbers = resolveRunFilter(configName, runFilter);
    if (runNumbers.size() == 0)
        throw cRuntimeError("Run filter does not match any run");
    if (runNumbers.size() > 1)
        throw cRuntimeError("Run filter matches more than one run (%d)", (int)runNumbers.size());
    int runNumber = runNumbers[0];
    cfg->activateConfig(configName, runNumber);

    // query option
    cConfigOption *option = cConfigOption::get(optionName);
    const char *value = cfg->getConfigValue(option, "");
    out << value << endl;
}

bool EnvirBase::setup()
{
    try {
        // ensure correct numeric format in output files
        setPosixLocale();

        // set opt->* variables from ini file(s)
        readOptions();

        if (attachDebuggerOnErrors) {
            signal(SIGSEGV, crashHandler);
            signal(SIGILL, crashHandler);
#ifndef _WIN32
            signal(SIGBUS, crashHandler);
            signal(SIGUSR1, crashHandler);
#endif
        }

        // initialize coroutine library
        if (TOTAL_STACK_SIZE != 0 && opt->totalStack <= MAIN_STACK_SIZE+4096) {
            if (opt->verbose)
                out << "Total stack size " << opt->totalStack << " increased to " << MAIN_STACK_SIZE << endl;
            opt->totalStack = MAIN_STACK_SIZE+4096;
        }
        cCoroutine::init(opt->totalStack, MAIN_STACK_SIZE);

        // install XML document cache
        xmlCache = new XMLDocCache();

        // parallel simulation
        if (opt->parsim) {
#ifdef WITH_PARSIM
            // parsim: create components
            parsimComm = createByClassName<cParsimCommunications>(opt->parsimcommClass.c_str(), "parallel simulation communications layer");
            parsimPartition = new cParsimPartition();
            cParsimSynchronizer *parsimSynchronizer = createByClassName<cParsimSynchronizer>(opt->parsimsynchClass.c_str(), "parallel simulation synchronization layer");
            addLifecycleListener(parsimPartition);

            // wire them together (note: 'parsimSynchronizer' is also the scheduler for 'simulation')
            parsimPartition->setContext(getSimulation(), parsimComm, parsimSynchronizer);
            parsimSynchronizer->setContext(getSimulation(), parsimPartition, parsimComm);
            getSimulation()->setScheduler(parsimSynchronizer);

            // initialize them
            parsimComm->init(opt->parsimNumPartitions);
#else
            throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
        }

        // load NED files embedded into the simulation program as string literals
        if (!embeddedNedFiles.empty()) {
            if (opt->verbose)
                out << "Loading embedded NED files: " << embeddedNedFiles.size() << endl;
            for (const auto& file : embeddedNedFiles) {
                std::string nedText = file.nedText;
                if (!file.garblephrase.empty())
                    nedText = opp_ungarble(file.nedText, file.garblephrase);
                getSimulation()->loadNedText(file.fileName.c_str(), nedText.c_str());
            }
        }

        // load NED files from folders on the NED path
        StringTokenizer tokenizer(opt->nedPath.c_str(), PATH_SEPARATOR);
        std::set<std::string> foldersLoaded;
        while (tokenizer.hasMoreTokens()) {
            const char *folder = tokenizer.nextToken();
            if (foldersLoaded.find(folder) == foldersLoaded.end()) {
                if (opt->verbose)
                    out << "Loading NED files from " << folder << ": ";
                int count = getSimulation()->loadNedSourceFolder(folder, opt->nedExcludedPackages.c_str());
                if (opt->verbose)
                    out << " " << count << endl;
                foldersLoaded.insert(folder);
            }
        }
        getSimulation()->doneLoadingNedFiles();

        // notify listeners when global setup is complete
        notifyLifecycleListeners(LF_ON_STARTUP);
    }
    catch (std::exception& e) {
        displayException(e);
        exitCode = 1;
        return false;  // don't run the app
    }
    return true;
}

void EnvirBase::printHelp()
{
    out << "Command line options:\n";
    out << "  <inifile> or -f <inifile>\n";
    out << "                Use the given ini file instead of omnetpp.ini. More than one\n";
    out << "                ini file can be specified.\n";
    out << "  --<configuration-option>=<value>\n";
    out << "                Configuration options and parameter values can be specified\n";
    out << "                on the command line, and they take precedence over options\n";
    out << "                specified in the ini file(s). Examples:\n";
    out << "                      --debug-on-errors=true\n";
    out << "                      --record-eventlog=true\n";
    out << "                      --sim-time-limit=1000s\n";
    out << "                      --**.serviceTime=100ms\n";
    out << "  -u <ui>       Selects the user interface. Standard choices are Cmdenv\n";
    out << "                and Qtenv. Specify -h userinterfaces to see the list\n";
    out << "                of the user interfaces available in your simulation program.\n";
    out << "  -c <configname>\n";
    out << "                Select a configuration for execution. With inifile-based\n";
    out << "                configuration database, this selects the [Config <configname>]\n";
    out << "                section; the default is the [General] section.\n";
    out << "                See also: -r.\n";
    out << "  -r <runfilter>\n";
    out << "                With -c: select runs from the specified configuration. A\n";
    out << "                missing -r option selects all runs in the given configuration.\n";
    out << "                <runfilter> is either a comma-separated list of run numbers or\n";
    out << "                run number ranges (for example 1,2,5-10), or a match expression.\n";
    out << "                The match expression may contain a wildcard pattern that is\n";
    out << "                matched against the iteration variables string (see -q runs),\n";
    out << "                matchers for individual iteration variables in the\n";
    out << "                name(valuepattern) syntax, or their combination using AND and\n";
    out << "                OR. Parentheses may be used to change evaluation order. Values\n";
    out << "                containing spaces etc need to be enclosed in quotes. Patterns\n";
    out << "                may contain elements matching numeric ranges, in the {a..b}\n";
    out << "                syntax. See also: -q.\n";
    out << "  -n <nedpath>  List of folders to load NED files from. Folders are separated\n";
    out << "                with a semicolon (on non-Windows systems, colon may also be used).\n";
    out << "                Multiple -n options may be present. The effective NED path is\n";
    out << "                produced by concatenating the values of the -n options, the\n";
    out << "                ned-path configuration option, and the OMNETPP_NED_PATH environment\n";
    out << "                variable. If the result is empty, the NED path defaults to '.',\n";
    out << "                that is, NED files will be loaded from the current directory and\n";
    out << "                its subfolders.\n";
    out << "  -x <ned-packages-to-exclude>\n";
    out << "                Semicolon-separated list of NED packages to exclude when loading\n";
    out << "                NED files. Sub-packages of excluded ones are also excluded.\n";
    out << "                Multiple -x options may be present, and additional packages may be\n";
    out << "                specified in the ned-package-exclusions configuration option and the\n";
    out << "                OMNETPP_NED_PACKAGE_EXCLUSIONS environment variable.\n";
    out << "  -i <imgpath>  List of folders to load images from. Folders are separated\n";
    out << "                with a semicolon (on non-Windows systems, colon may also be used).\n";
    out << "                Multiple -i options may be present. The effective image path is\n";
    out << "                produced by concatenating the values of the -i options, the\n";
    out << "                image-path configuration option, and the OMNETPP_IMAGE_PATH\n";
    out << "                environment variable. When the environment variable is not\n";
    out << "                present, a hardcoded folder (determined at compile time) pointing\n";
    out << "                to <omnetpp>/images is used instead.\n";
    out << "  -l <library>  Load the specified shared library (.so or .dll) on startup.\n";
    out << "                The file name should be given without the .so or .dll suffix\n";
    out << "                (it will be appended automatically.) The loaded module may\n";
    out << "                contain simple modules, plugins, etc. Multiple -l options\n";
    out << "                can be present.\n";
#ifdef SIMFRONTEND_SUPPORT
    out << "  -p <port>     Port number for the built-in web server.\n";
    out << "                If the port is not available, the program will exit with an\n";
    out << "                error message unless the number is suffixed with the plus\n";
    out << "                sign. The plus sign causes the program to search for the\n";
    out << "                first available port number above the given one, and not stop\n";
    out << "                with an error even if no available port was found. A plain\n";
    out << "                minus sign will turn off the built-in web server.\n";
    out << "                The default value is \"8000+\".\n";
#endif
    out << "  -s            Silent mode; makes the output less verbose.\n";
    out << "  -v            Print version and build info, and exit.\n";
    out << "  -m            Merge standard error into standard output, i.e. report errors on\n";
    out << "                the standard output instead of the default standard error.\n";
    out << "  -a            Print all config names and number of runs in them, and exit.\n";
    out << "  -q <what>     To be used together with -c and -r. Prints information about\n";
    out << "                the specified configuration and runs, and exits.\n";
    out << "    -q numruns  Prints the number of runs in the given configuration and the\n";
    out << "                number of runs selected by the run filter (-r option).\n";
    out << "    -q runnumbers\n";
    out << "                Prints the run numbers of the runs selected by the run filter\n";
    out << "                (-r option).\n";
    out << "    -q runs     Like -q numruns, but also prints one line of information with the\n";
    out << "                iteration variables about each run that the run filter matches.\n";
    out << "    -q rundetails\n";
    out << "                Like -q numruns, but also prints the values of the iteration\n";
    out << "                variables and a summary of the configuration (the expanded\n";
    out << "                values of configuration entries that contain iteration variables)\n";
    out << "                for each matching run.\n";
    out << "    -q runconfig\n";
    out << "                Like -q numruns, but also prints the values of the iteration\n";
    out << "                variables and the full configuration for each matching run.\n";
    out << "    -q sectioninheritance\n";
    out << "                Print the section fallback chain of the specified configuration.\n";
    out << "  -e <option>   Prints the value of the given configuration option in the\n";
    out << "                simulation run denoted by the -c and -r options. (The run filter\n";
    out << "                should match exactly one simulation run.)\n";
    out << "  -h            Print this help and exit.\n";
    out << "  -h <category> Lists registered components:\n";
    out << "    -h config         Prints the list of available configuration options\n";
    out << "    -h configdetails  Prints the list of available configuration options, with\n";
    out << "                      their documentation\n";
    out << "    -h configvars     Prints the list of variables that can be used in configuration values\n";
    out << "    -h userinterfaces Lists available user interfaces (see -u option)\n";
    out << "    -h classes        Lists registered C++ classes (including module classes)\n";
    out << "    -h classdesc      Lists C++ classes that have associated reflection\n";
    out << "                      information (needed for Qtenv inspectors)\n";
    out << "    -h nedfunctions   Lists registered NED functions\n";
    out << "    -h neddecls       Lists built-in NED component declarations\n";
    out << "    -h units          Lists recognized physical units\n";
    out << "    -h enums          Lists registered enums\n";
    out << "    -h resultfilters  Lists result filters\n";
    out << "    -h resultrecorders Lists result recorders\n";
    out << "    -h figures        Lists available figure types\n";
    out << "    -h all            Union of all the above\n";
    out << endl;

    // print specific help for each user interface
    cRegistrationList *table = omnetapps.getInstance();
    table->sort();
    for (int i = 0; i < table->size(); i++) {
        // Instantiate the user interface, call printUISpecificHelp(), then dispose.
        // Note: their constructors are not supposed to do anything but trivial member initialization.
        cOmnetAppRegistration *appreg = dynamic_cast<cOmnetAppRegistration *>(table->get(i));
        ASSERT(appreg != nullptr);
        cEnvir *app = appreg->createOne();
        if (EnvirBase *envir = dynamic_cast<EnvirBase *>(app))
            envir->printUISpecificHelp();
        delete app;
    }
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

void EnvirBase::run()
{
    try {
        doRun();
    }
    catch (std::exception& e) {
        displayException(e);
    }
}

void EnvirBase::shutdown()
{
    try {
        getSimulation()->deleteNetwork();
        notifyLifecycleListeners(LF_ON_SHUTDOWN);
    }
    catch (std::exception& e) {
        displayException(e);
    }
}

void EnvirBase::setupNetwork(cModuleType *network)
{
    currentEventName = "";
    currentEventClassName = nullptr;
    currentModuleId = -1;

    getSimulation()->setupNetwork(network);
    eventlogManager->flush();

    if (opt->debugStatisticsRecording)
        EnvirUtils::dumpResultRecorders(out, getSimulation()->getSystemModule());
}

void EnvirBase::prepareForRun()
{
    resetClock();
    if (opt->simtimeLimit >= SIMTIME_ZERO)
        getSimulation()->setSimulationTimeLimit(opt->simtimeLimit);
    getSimulation()->callInitialize();
    cLogProxy::flushLastLine();
}

//-------------------------------------------------------------

std::vector<int> EnvirBase::resolveRunFilter(const char *configName, const char *runFilter)
{
    std::vector<int> runNumbers;

    if (opp_isblank(runFilter)) {
        int numRuns = cfg->getNumRunsInConfig(configName);
        for (int i = 0; i < numRuns; i++)
            runNumbers.push_back(i);
        return runNumbers;
    }

    // if filter contains a list of run numbers (e.g. "0..4,9,12"), parse it accordingly
    if (strspn (runFilter, "0123456789,.- ") == strlen(runFilter)) {
        int numRuns = cfg->getNumRunsInConfig(configName);
        EnumStringIterator it(runFilter);
        while (true) {
            int runNumber = it();
            if (runNumber == -1)
                break;
            if (runNumber >= numRuns)
                throw cRuntimeError("Run number %d in run list '%s' is out of range 0..%d", runNumber, runFilter, numRuns-1);
            runNumbers.push_back(runNumber);
            it++;
        }
        if (it.hasError())
            throw cRuntimeError("Syntax error in run list '%s'", runFilter);
    }
    else {
        // evaluate filter as constraint expression
        std::vector<cConfiguration::RunInfo> runDescriptions = cfg->unrollConfig(configName);
        for (int runNumber = 0; runNumber < (int) runDescriptions.size(); runNumber++) {
            try {
                cConfiguration::RunInfo runInfo = runDescriptions[runNumber];
                std::string expandedRunFilter = opp_substitutevariables(runFilter, runInfo.runAttrs);
                ValueIterator::Expr expr(expandedRunFilter.c_str());
                expr.substituteVariables(ValueIterator::VariableMap());
                expr.evaluate();
                if (expr.boolValue())
                    runNumbers.push_back(runNumber);
            }
            catch (std::exception& e) {
                throw cRuntimeError("Cannot evaluate run filter: %s", e.what());
            }
        }
    }
    return runNumbers;
}

void EnvirBase::preconfigure(cComponent *component)
{
    setupRNGMapping(component);
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
    StringTokenizer nedTokenizer(opt->nedPath.c_str(), PATH_SEPARATOR);
    while (nedTokenizer.hasMoreTokens()) {
        const char *dir = nedTokenizer.nextToken();
        std::string path = concatDirAndFile(dir, fileName);
        if (fileExists(path.c_str()))
            return tidyFilename(path.c_str());
    }

    // search the image path
    StringTokenizer imgTokenizer(opt->imagePath.c_str(), PATH_SEPARATOR);
    while (imgTokenizer.hasMoreTokens()) {
        const char *dir = imgTokenizer.nextToken();
        std::string path = concatDirAndFile(dir, fileName);
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

//-------------------------------------------------------------

void EnvirBase::processFileName(std::string& fname)
{
    // insert ".<hostname>.<pid>" if requested before file extension
    // (note: parsimProcId cannot be appended because of initialization order)
    if (opt->fnameAppendHost) {
        std::string extension = "";
        std::string::size_type index = fname.rfind('.');
        if (index != std::string::npos) {
            extension = std::string(fname, index);
            fname.erase(index);
        }

        const char *hostname = opp_gethostname();
        if (!hostname)
            throw cRuntimeError("Cannot append hostname to file name '%s': no host name configured, and no HOST, HOSTNAME "
                                "or COMPUTERNAME (Windows) environment variable set", fname.c_str());
        int pid = getpid();

        // append
        fname += opp_stringf(".%s.%d%s", hostname, pid, extension.c_str());
    }
}

void EnvirBase::startOutputRedirection(const char *fileName)
{
    ASSERT(!isOutputRedirected());

    mkPath(directoryOf(fileName).c_str());

    std::filebuf *fbuf = new std::filebuf;
    fbuf->open(fileName, std::ios_base::out);
    if (!fbuf->is_open())
       throw cRuntimeError("Cannot open output redirection file '%s'", fileName);
    out.rdbuf(fbuf);
    redirectionFilename = fileName;
}

void EnvirBase::stopOutputRedirection()
{
    if (isOutputRedirected()) {
        std::streambuf *fbuf = out.rdbuf();
        fbuf->pubsync();
        out.rdbuf(std::cout.rdbuf());
        delete fbuf;
        redirectionFilename = "";
    }
}

bool EnvirBase::isOutputRedirected()
{
    return out.rdbuf() != std::cout.rdbuf();
}

std::ostream& EnvirBase::err()
{
    std::ostream& err = opt->useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (opt->useStderr ? std::cerr : std::cout) << "<!> Error -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> Error: ";
    return err;
}

std::ostream& EnvirBase::errWithoutPrefix()
{
    std::ostream& err = opt->useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (opt->useStderr ? std::cerr : std::cout) << "<!> Error -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> ";
    return err;
}

std::ostream& EnvirBase::warn()
{
    std::ostream& err = opt->useStderr && !isOutputRedirected() ? std::cerr : out;
    if (isOutputRedirected())
        (opt->useStderr ? std::cerr : std::cout) << "<!> Warning -- see " << redirectionFilename << " for details" << endl;
    err << endl << "<!> Warning: ";
    return err;
}

void EnvirBase::readOptions()
{
    cConfiguration *cfg = getConfig();

    opt->totalStack = (size_t)cfg->getAsDouble(CFGID_TOTAL_STACK, TOTAL_STACK_SIZE);
    opt->parsim = cfg->getAsBool(CFGID_PARALLEL_SIMULATION);
    if (opt->parsim) {
#ifdef WITH_PARSIM
        opt->parsimNumPartitions = cfg->getAsInt(CFGID_PARSIM_NUM_PARTITIONS);
        opt->parsimcommClass = cfg->getAsString(CFGID_PARSIM_COMMUNICATIONS_CLASS);
        opt->parsimsynchClass = cfg->getAsString(CFGID_PARSIM_SYNCHRONIZATION_CLASS);
#else
        throw cRuntimeError("Parallel simulation is turned on in the ini file, but OMNeT++ was compiled without parallel simulation support (WITH_PARSIM=no)");
#endif
    }

    opt->fnameAppendHost = cfg->getAsBool(CFGID_FNAME_APPEND_HOST, opt->parsim);

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

    // note: this is read per run as well, but Qtenv needs its value on startup too
    opt->inifileNetworkDir = cfg->getConfigEntry(CFGID_NETWORK->getName()).getBaseDirectory();

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
    opt->networkName = cfg->getAsString(CFGID_NETWORK);
    opt->inifileNetworkDir = cfg->getConfigEntry(CFGID_NETWORK->getName()).getBaseDirectory();
    opt->warnings = cfg->getAsBool(CFGID_WARNINGS);
    opt->simtimeLimit = cfg->getAsDouble(CFGID_SIM_TIME_LIMIT, -1);
    opt->realTimeLimit = cfg->getAsDouble(CFGID_REAL_TIME_LIMIT, -1);
    opt->cpuTimeLimit = cfg->getAsDouble(CFGID_CPU_TIME_LIMIT, -1);
    opt->warmupPeriod = cfg->getAsDouble(CFGID_WARMUP_PERIOD);
    opt->numRNGs = cfg->getAsInt(CFGID_NUM_RNGS);
    opt->rngClass = cfg->getAsString(CFGID_RNG_CLASS);
    opt->seedset = cfg->getAsInt(CFGID_SEED_SET);
    opt->debugStatisticsRecording = cfg->getAsBool(CFGID_DEBUG_STATISTICS_RECORDING);
    opt->checkSignals = cfg->getAsBool(CFGID_CHECK_SIGNALS);
    opt->schedulerClass = cfg->getAsString(CFGID_SCHEDULER_CLASS);
    opt->futureeventsetClass = cfg->getAsString(CFGID_FUTUREEVENTSET_CLASS);
    opt->eventlogManagerClass = cfg->getAsString(CFGID_EVENTLOGMANAGER_CLASS);
    opt->outputVectorManagerClass = cfg->getAsString(CFGID_OUTPUTVECTORMANAGER_CLASS);
    opt->outputScalarManagerClass = cfg->getAsString(CFGID_OUTPUTSCALARMANAGER_CLASS);
    opt->snapshotmanagerClass = cfg->getAsString(CFGID_SNAPSHOTMANAGER_CLASS);
    debugOnErrors = cfg->getAsBool(CFGID_DEBUG_ON_ERRORS);  // note: handling overridden in Qtenv::readPerRunOptions() due to interference with GUI
    opt->printUndisposed = cfg->getAsBool(CFGID_PRINT_UNDISPOSED);
    getSimulation()->setParameterMutabilityCheck(cfg->getAsBool(CFGID_PARAMETER_MUTABILITY_CHECK));

    // make time limits effective
    stopwatch.setCPUTimeLimit(opt->cpuTimeLimit);
    stopwatch.setRealTimeLimit(opt->realTimeLimit);
    getSimulation()->setWarmupPeriod(opt->warmupPeriod);

    // install eventlog manager
    delete eventlogManager;
    eventlogManager = createByClassName<cIEventlogManager>(opt->eventlogManagerClass.c_str(), "eventlog manager");

    // install output vector manager
    delete outvectorManager;
    outvectorManager = createByClassName<cIOutputVectorManager>(opt->outputVectorManagerClass.c_str(), "output vector manager");
    addLifecycleListener(outvectorManager);

    // install output scalar manager
    delete outScalarManager;
    outScalarManager = createByClassName<cIOutputScalarManager>(opt->outputScalarManagerClass.c_str(), "output scalar manager");
    addLifecycleListener(outScalarManager);

    // install snapshot manager
    delete snapshotManager;
    snapshotManager = createByClassName<cISnapshotManager>(opt->snapshotmanagerClass.c_str(), "snapshot manager");
    addLifecycleListener(snapshotManager);

    // install FES
    cFutureEventSet *fes = createByClassName<cFutureEventSet>(opt->futureeventsetClass.c_str(), "FES");
    getSimulation()->setFES(fes);

    // install scheduler
    if (!opt->parsim) {
        cScheduler *scheduler = createByClassName<cScheduler>(opt->schedulerClass.c_str(), "event scheduler");
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

    // run RNG self-test on RNG class selected for this run
    cRNG *testRng = createByClassName<cRNG>(opt->rngClass.c_str(), "random number generator");
    testRng->selfTest();
    delete testRng;

    // set up RNGs
    for (int i = 0; i < numRNGs; i++)
        delete rngs[i];
    delete[] rngs;

    numRNGs = opt->numRNGs;
    rngs = new cRNG *[numRNGs];
    for (int i = 0; i < numRNGs; i++) {
        rngs[i] = createByClassName<cRNG>(opt->rngClass.c_str(), "random number generator");
        rngs[i]->initialize(opt->seedset, i, numRNGs, getParsimProcId(), getParsimNumPartitions(), getConfig());
    }

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

int EnvirBase::getNumRNGs() const
{
    return numRNGs;
}

cRNG *EnvirBase::getRNG(int k)
{
    if (k < 0 || k >= numRNGs)
        throw cRuntimeError("RNG index %d is out of range (num-rngs=%d, check the configuration)", k, numRNGs);
    return rngs[k];
}

void EnvirBase::setupRNGMapping(cComponent *component)
{
    cConfigurationEx *cfg = getConfigEx();
    std::string componentFullPath = component->getFullPath();
    std::vector<const char *> suffixes = cfg->getMatchingPerObjectConfigKeySuffixes(componentFullPath.c_str(), "rng-*");  // CFGID_RNG_K
    if (suffixes.empty())
        return;

    // extract into tmpmap[]
    int mapsize = 0;
    int tmpmap[100];
    for (auto suffix : suffixes) {
        // contains "rng-1", "rng-4" or whichever has been found in the config for this module/channel
        const cConfiguration::KeyValue& entry = cfg->getPerObjectConfigEntry(componentFullPath.c_str(), suffix);
        const char *value = entry.getValue();
        ASSERT(value != nullptr);
        try {
            char *endptr;
            int modRng = strtol(suffix+strlen("rng-"), &endptr, 10);
            if (*endptr != '\0')
                throw opp_runtime_error("Numeric RNG index expected after 'rng-'");

            int physRng = strtol(value, &endptr, 10);
            if (*endptr != '\0') {
                // not a numeric constant, try parsing it as an expression
                cDynamicExpression expr;
                expr.parse(value);
                cExpression::Context context(component, entry.getBaseDirectory());
                cValue tmp = expr.evaluate(&context);
                if (!tmp.isNumeric())
                    throw opp_runtime_error("Numeric constant or expression expected");
                physRng = tmp;
            }

            if (physRng >= getNumRNGs())
                throw cRuntimeError("RNG index %d out of range (num-rngs=%d)", physRng, getNumRNGs());
            if (modRng >= mapsize) {
                if (modRng >= 100)
                    throw cRuntimeError("Local RNG index %d out of supported range 0..99", modRng);
                while (mapsize <= modRng) {
                    tmpmap[mapsize] = mapsize;
                    mapsize++;
                }
            }
            tmpmap[modRng] = physRng;
        }
        catch (std::exception& e) {
            throw cRuntimeError("%s in configuration entry *.%s = %s for module/channel %s",
                    e.what(), suffix, value, component->getFullPath().c_str());

        }
    }

    // install map into the module
    if (mapsize > 0) {
        int *map = new int[mapsize];
        memcpy(map, tmpmap, mapsize*sizeof(int));
        component->setRNGMap(mapsize, map);
    }
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

std::string EnvirBase::makeDebuggerCommand()
{
    const char *cmdDefault = opp_emptytodefault(getenv("OMNETPP_DEBUGGER_COMMAND"), DEFAULT_DEBUGGER_COMMAND);
    std::string cmd = getConfig()->getAsString(CFGID_DEBUGGER_ATTACH_COMMAND, cmdDefault);

    if (cmd == "") {
        alert("Cannot start debugger: no debugger configured");
        return "";
    }
    size_t pos = cmd.find('%');
    if (pos == std::string::npos || cmd.rfind('%') != pos || cmd[pos+1] != 'u') {
        alert("Cannot start debugger: debugger attach command must contain '%u' and no additional percent sign");
        return "";
    }
    pid_t pid = getpid();
    return opp_stringf(cmd.c_str(), (unsigned int)pid);
}

void EnvirBase::attachDebugger()
{
    // launch debugger
    std::string cmd = makeDebuggerCommand();
    if (cmd == "") {
        alert("No suitable debugger command!");
        return;
    }

    out << "Starting debugger: " << cmd << endl;
    int returncode = system(cmd.c_str());

    if (returncode != 0) {
        std::string errormsg = "Debugger command '" + cmd + "' returned error.";
        alert(errormsg.c_str());
        return;
    }

    out << "Waiting for the debugger to start up and attach to us; note that "
           "for the latter to work, some systems (e.g. Ubuntu) require debugging "
           "of non-child processes to be explicitly enabled." << endl;

    // hold for a while to allow debugger to start up and attach to us
    int secondsToWait = (int)ceil(getConfig()->getAsDouble(CFGID_DEBUGGER_ATTACH_WAIT_TIME));
    time_t startTime = time(nullptr);
    while (time(nullptr)-startTime < secondsToWait && detectDebugger() != DebuggerPresence::PRESENT)
        for (int i=0; i<100000000; i++); // DEBUGGER ATTACHED -- PLEASE CONTINUE EXECUTION TO REACH THE BREAKPOINT

    if (detectDebugger() == DebuggerPresence::NOT_PRESENT)
        alert("Debugger did not attach in time.");
}

bool EnvirBase::ensureDebugger(cRuntimeError *error)
{
    if (error == nullptr || attachDebuggerOnErrors) {
        if (detectDebugger() == DebuggerPresence::NOT_PRESENT)
            attachDebugger();
        return detectDebugger() != DebuggerPresence::NOT_PRESENT;
    }
    else {

#ifdef NDEBUG
       printf("\n[Warning: Program was compiled without debug info, you might not be able to debug.]\n");
#endif

   printf("\n"
          "RUNTIME ERROR. A cRuntimeError exception is about to be thrown, and you\n"
          "requested (by setting debug-on-errors=true in the ini file) that errors\n"
          "abort execution and break into the debugger.\n\n"
#ifdef _MSC_VER
           "If you see a [Debug] button on the Windows crash dialog and you have\n"
           "just-in-time debugging enabled, select it to get into the Visual Studio\n"
           "debugger. Otherwise, you should already have attached to this process from\n"
           "Visual Studio. Once in the debugger, see you can browse to the context of\n"
           "the error in the \"Call stack\" debug view.\n\n"
#else
           "You should now probably be running the simulation under gdb or another\n"
           "debugger. The simulation kernel will now raise a SIGINT signal which will\n"
           "get you into the debugger. If you are not running under a debugger, you can\n"
           "still use the core dump for post-mortem debugging. Once in the debugger,\n"
           "view the call stack (in gdb: \"bt\" command) to see the context of the\n"
           "runtime error.\n\n"
#endif
       );

       printf("<!> %s\n", error->getFormattedMessage().c_str());
       printf("\nTRAPPING on the exception above, due to a debug-on-errors=true configuration option. Is your debugger ready?\n");
       fflush(stdout);

       return true;
    }

    return false;
}

DebuggerPresence EnvirBase::detectDebugger()
{
#ifdef _WIN32
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms680345%28v=vs.85%29.aspx
    return IsDebuggerPresent() ? DebuggerPresence::PRESENT : DebuggerPresence::NOT_PRESENT;
#elif defined(__APPLE__)
    /*
    // This is the "official" method, described here:
    // https://developer.apple.com/library/content/qa/qa1361/_index.html
    int                 junk;
    int                 mib[4];
    struct kinfo_proc   info;
    size_t              size;

    // Initialize the flags so that, if sysctl fails for some bizarre
    // reason, we get a predictable result.

    info.kp_proc.p_flag = 0;

    // Initialize mib, which tells sysctl the info we want, in this case
    // we're looking for information about a specific process ID.

    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = getpid();

    // Call sysctl.

    size = sizeof(info);
    junk = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, nullptr, 0);
    if (junk != 0)
        return DebuggerPresence::CANT_DETECT;

    // We're being debugged if the P_TRACED flag is set.

    return ( (info.kp_proc.p_flag & P_TRACED) != 0 ) ? DebuggerPresence::PRESENT : DebuggerPresence::NOT_PRESENT;
    */

    // This one is supposed to be better.
    // https://zgcoder.net/ramblings/osx-debugger-detection.html

    mach_msg_type_number_t count = 0;
    exception_mask_t masks[EXC_TYPES_COUNT];
    mach_port_t ports[EXC_TYPES_COUNT];
    exception_behavior_t behaviors[EXC_TYPES_COUNT];
    thread_state_flavor_t flavors[EXC_TYPES_COUNT];
    exception_mask_t mask = EXC_MASK_ALL & ~(EXC_MASK_RESOURCE | EXC_MASK_GUARD);

    kern_return_t result = task_get_exception_ports(mach_task_self(), mask, masks, &count, ports, behaviors, flavors);

    if (result != KERN_SUCCESS)
        return DebuggerPresence::CANT_DETECT;

    for (mach_msg_type_number_t portIndex = 0; portIndex < count; portIndex++)
        if (MACH_PORT_VALID(ports[portIndex]))
            return DebuggerPresence::PRESENT;

    return DebuggerPresence::NOT_PRESENT;

#else

    // Assume that we are on GNU/Linux, and have a procfs.
    // If not, in the worst case, the file can't be opened, and we return CANT_DETECT.

    // https://stackoverflow.com/questions/3596781/how-to-detect-if-the-current-process-is-being-run-by-gdb/24969863#24969863

    std::ifstream input("/proc/self/status");

    if (!input.good())
        return DebuggerPresence::CANT_DETECT;

    for (std::string line; getline(input, line); ) {
        std::stringstream ss(line);
        std::string info;
        ss >> info;

        if (info.substr(0, 9) == "TracerPid") {
            int value;
            ss >> value;
            return value == 0 ? DebuggerPresence::NOT_PRESENT : DebuggerPresence::PRESENT;
        }
    }

    return DebuggerPresence::CANT_DETECT;
#endif
}

DebuggerAttachmentPermission EnvirBase::debuggerAttachmentPermitted()
{
#if defined(__APPLE__) || defined(_WIN32)
    return DebuggerAttachmentPermission::CANT_DETECT; // we don't know of anything that would block debugger attachment on these platforms
#else
    // we assume we are on GNU/Linux
    // https://askubuntu.com/questions/41629/after-upgrade-gdb-wont-attach-to-process
    std::ifstream fs("/proc/sys/kernel/yama/ptrace_scope");

    int scope;
    fs >> scope;

    if (!fs.good())
        return DebuggerAttachmentPermission::CANT_DETECT; // can't detect

    return scope == 0 // if it's not zero, non-child debugging is blocked
            ? DebuggerAttachmentPermission::PERMITTED
            : DebuggerAttachmentPermission::DENIED;
#endif
}

void EnvirBase::crashHandler(int)
{
    ((EnvirBase*)cSimulation::getActiveEnvir())->attachDebugger();
}

std::string EnvirBase::getFormattedMessage(std::exception& ex)
{
    if (cException *e = dynamic_cast<cException *>(&ex))
        return e->getFormattedMessage();
    else
        return ex.what();
}

void EnvirBase::displayException(std::exception& ex)
{
    std::string msg = getFormattedMessage(ex);
    if (dynamic_cast<cTerminationException*>(&ex) != nullptr)
        out << endl << "<!> " << msg << endl;
    else if (msg.substr(0,5) == "Error")
        errWithoutPrefix() << msg << endl;
    else
        err() << msg << endl;
}

bool EnvirBase::idle()
{
    return false;
}

int EnvirBase::getArgCount() const
{
    return args->getArgCount();
}

char **EnvirBase::getArgVector() const
{
    return args->getArgVector();
}

//-------------------------------------------------------------

void EnvirBase::resetClock()
{
    stopwatch.resetClock();
}

void EnvirBase::startClock()
{
    stopwatch.startClock();
}

void EnvirBase::stopClock()
{
    stopwatch.stopClock();
    simulatedTime = getSimulation()->getSimTime();
}

double EnvirBase::getElapsedSecs()
{
    return stopwatch.getElapsedSecs();
}

void EnvirBase::checkTimeLimits()
{
    if (!stopwatch.hasTimeLimits())
        return;
    if (isExpressMode() && (getSimulation()->getEventNumber() & 1023) != 0)  // optimize: in Express mode, don't read the clock on every event
        return;
    stopwatch.checkTimeLimits();
}

void EnvirBase::stoppedWithTerminationException(cTerminationException& e)
{
    // if we're running in parallel and this exception is NOT one we received
    // from other partitions, then notify other partitions
#ifdef WITH_PARSIM
    if (opt->parsim && !dynamic_cast<cReceivedTerminationException *>(&e))
        parsimPartition->broadcastTerminationException(e);
#endif
    if (recordEventlog) {
        //FIXME should not be in this function (Andras)
        eventlogManager->endRun(e.isError(), e.getErrorCode(), e.getFormattedMessage().c_str());
    }
}

void EnvirBase::stoppedWithException(std::exception& e)
{
    // if we're running in parallel and this exception is NOT one we received
    // from other partitions, then notify other partitions
#ifdef WITH_PARSIM
    if (opt->parsim && !dynamic_cast<cReceivedException *>(&e))
        parsimPartition->broadcastException(e);
#endif
    if (recordEventlog) {
        // TODO: get error code from the exception?
        eventlogManager->endRun(true, E_CUSTOM, e.what());  //FIXME this should be rather in endRun(), or? (Andras)
    }
}

void EnvirBase::checkFingerprint()
{
    cFingerprintCalculator *fingerprint = getSimulation()->getFingerprintCalculator();
    if (!getSimulation()->getFingerprintCalculator())
        return;

    auto flags = opp_substringafterlast(fingerprint->str(), "/");
    if (fingerprint->checkFingerprint())
        printfmsg("Fingerprint successfully verified: %s", fingerprint->str().c_str());
    else
        printfmsg("Fingerprint mismatch! calculated: %s, expected: %s",
                fingerprint->str().c_str(), cfg->getAsString(CFGID_FINGERPRINT).c_str());
}

cModuleType *EnvirBase::resolveNetwork(const char *networkname)
{
    cModuleType *network = nullptr;
    std::string inifilePackage = getSimulation()->getNedPackageForFolder(opt->inifileNetworkDir.c_str());

    bool hasInifilePackage = !inifilePackage.empty() && strcmp(inifilePackage.c_str(), "-") != 0;
    if (hasInifilePackage)
        network = cModuleType::find((inifilePackage+"."+networkname).c_str());
    if (!network)
        network = cModuleType::find(networkname);
    if (!network) {
        if (hasInifilePackage)
            throw cRuntimeError("Network '%s' or '%s' not found, check .ini and .ned files",
                    networkname, (inifilePackage+"."+networkname).c_str());
        else
            throw cRuntimeError("Network '%s' not found, check .ini and .ned files", networkname);
    }
    if (!network->isNetwork())
        throw cRuntimeError("Module type '%s' is not a network", network->getFullName());
    return network;
}

}  // namespace envir
}  // namespace omnetpp

