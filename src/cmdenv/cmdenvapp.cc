//==========================================================================
//  CMDENVAPP.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cmdenvapp.h"
#include "cmdenvsimulationrunner.h"
#include "envir/args.h"
#include "envir/appreg.h"
#include "common/stringutil.h"
#include "sim/pythonutil.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {
namespace cmdenv {

//
// Register the Cmdenv user interface
//
Register_OmnetApp("Cmdenv", CmdenvApp, 10, "command-line user interface");

//
// The following function can be used to force linking with Cmdenv; specify
// -u _cmdenv_lib (gcc) or /include:_cmdenv_lib (vc++) in the link command.
//
extern "C" CMDENV_API void cmdenv_lib() {}
// on some compilers (e.g. linux gcc 4.2) the functions are generated without _
extern "C" CMDENV_API void _cmdenv_lib() {}

CmdenvApp::CmdenvApp()
{
    // Note: ctor should only contain trivial initializations, because
    // the class may be instantiated only for the purpose of calling
    // printUISpecificHelp() on it
}

CmdenvApp::~CmdenvApp()
{
}

void CmdenvApp::printUISpecificHelp()
{
    out << "Cmdenv-specific information:\n";
    out << "    Cmdenv executes all runs denoted by the -c and -r options. The number\n";
    out << "    of runs executed and the number of runs that ended with an error are\n";
    out << "    reported at the end.\n";
    out << endl;
}

int CmdenvApp::doRunApp()
{
    CodeFragments::executeAll(CodeFragments::STARTUP); //TODO probably remove this from here

    //TODO cleanup; also into Qtenv
    const char *fname;
    for (int k = 0; (fname = args->argument(k)) != nullptr; k++) {
        if (opp_stringendswith(fname, ".ini")) {
            // ignore -- already processed
        }
        else if (opp_stringendswith(fname, ".py")) {
#ifdef WITH_PYTHON
            ensurePythonInterpreter();
            FILE *f = fopen(fname, "r");
            if (f) {
                PyRun_AnyFile(f, fname);
                fclose(f);
            }
#else
            throw opp_runtime_error("Cannot process command-line argument '%s': No OMNeT++ was compiled with WITH_PYTHON=no", fname);
#endif
        }
        else {
            throw opp_runtime_error("Cannot process command-line argument '%s': Unknown file extension", fname);
        }
    }

    CmdenvSimulationRunner::installSigintHandler();

    CmdenvSimulationRunner cmdenvRunner(out, args);
    cmdenvRunner.setVerbose(verbose);
    cmdenvRunner.setUseStderr(useStderr);
    int exitCode = cmdenvRunner.runCmdenv(ini);

    CmdenvSimulationRunner::deinstallSigintHandler();

    return exitCode;
}

void CmdenvApp::displayException(std::exception& ex)
{
    std::string msg = cException::getFormattedMessage(ex);
    std::ostream& os = (cException::isError(ex) && useStderr) ? std::cerr : out;
    os << "\n<!> " << msg << endl << endl;
}

void CmdenvApp::alert(const char *msg)
{
    std::ostream& err = useStderr ? std::cerr : out;
    err << "\n<!> " << msg << endl << endl;
}

}  // namespace cmdenv
}  // namespace omnetpp

