//==========================================================================
//  DEBUGGERSUPPORT.CC - part of
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

#include <cstdio>
#include <csignal>
#include <fstream>
#include <sstream>
#include <set>
#include <algorithm>
#include "omnetpp/platdep/platmisc.h"  // getpid
#include "debuggersupport.h"
#include "common/stringutil.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"

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

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

#ifdef _WIN32
#define DEFAULT_DEBUGGER_COMMAND    "bash -c \"opp_ide omnetpp://cdt/debugger/attach?pid=%u\""
#else
#define DEFAULT_DEBUGGER_COMMAND    "opp_ide omnetpp://cdt/debugger/attach?pid=%u"
#endif

Register_GlobalConfigOption(CFGID_DEBUGGER_ATTACH_COMMAND, "debugger-attach-command", CFG_STRING, nullptr, "The command line to launch the debugger. It must contain exactly one percent sign, as `%u`, which will be replaced by the PID of this process. The command must not block (i.e. it should end in `&` on Unix-like systems). It will be executed by the default system shell (on Windows, usually cmd.exe). Default on this platform: `" DEFAULT_DEBUGGER_COMMAND "`. This default can be overridden with the `OMNETPP_DEBUGGER_COMMAND` environment variable.");
Register_GlobalConfigOptionU(CFGID_DEBUGGER_ATTACH_WAIT_TIME, "debugger-attach-wait-time", "s", "20s", "An interval to wait after launching the external debugger, to give the debugger time to start up and attach to the simulation process.");

void DebuggerSupport::configure(cConfiguration *cfg)
{
    const char *cmdDefault = opp_emptytodefault(getenv("OMNETPP_DEBUGGER_COMMAND"), DEFAULT_DEBUGGER_COMMAND);
    std::string cmd = cfg->getAsString(CFGID_DEBUGGER_ATTACH_COMMAND, cmdDefault);
    setDebuggerCommand(cmd);

    int waitTime = (int) (ceil(cfg->getAsDouble(CFGID_DEBUGGER_ATTACH_WAIT_TIME)));
    setAttachWaitTime(waitTime);
}

std::string DebuggerSupport::substitutePid(const std::string& cmd)
{
    return opp_replacesubstring(cmd, "%u", std::to_string(getpid()), true);
}

void DebuggerSupport::attachDebugger()
{
    // launch debugger
    std::string cmd = getDebuggerCommand();
    if (cmd == "")
        throw opp_runtime_error("No suitable debugger command");

    std::cout << "Starting debugger: " << cmd << endl;
    int returncode = system(cmd.c_str());

    if (returncode != 0)
        throw opp_runtime_error("Debugger command '%s' returned error", cmd.c_str());

    std::cout << "Waiting for the debugger to start up and attach to us; note that "
                 "for the latter to work, some systems (e.g. Ubuntu) require debugging "
                 "of non-child processes to be explicitly enabled." << endl;

    // hold for a while to allow debugger to start up and attach to us
    int waitTime = getAttachWaitTime();
    time_t startTime = time(nullptr);
    while (time(nullptr)-startTime < waitTime && detectDebugger() != DebuggerPresence::PRESENT)
        for (int i=0; i<100000000; i++); // DEBUGGER ATTACHED -- PLEASE CONTINUE EXECUTION TO REACH THE BREAKPOINT

    if (detectDebugger() == DebuggerPresence::NOT_PRESENT)
        throw opp_runtime_error("Debugger did not attach in time");
}

DebuggerPresence DebuggerSupport::detectDebugger()
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

DebuggerAttachmentPermission DebuggerSupport::debuggerAttachmentPermitted()
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


}  // namespace envir
}  // namespace omnetpp

