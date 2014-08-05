//==========================================================================
//  OPP_RUN.CC - part of
//
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Authors: Andras Varga, Rudolf Hornig
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2011 Andras Varga
  Copyright (C) 2006-2011 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

/*
 * 'opp_run' allows starting simulation models that are linked as shared
 * libraries. It provides the main() entry point for the simulation and
 * ensures that all models are compiled in either debug- or release-mode.
 *
 * How it works:
 *
 * In straightforward cases, 'opp_run' simply calls evMain() (in oppenvir) and
 * passes all command line arguments. Unfortunately 'opp_run' cannot execute a
 * simulation directly if the simulation was compiled in release-mode, because
 * 'opp_run' is linked with debug-mode OMNeT++ libraries. Loading a release-mode
 * simulation using a debug-mode 'opp_run' would load both debug and release
 * simulation kenels resulting in duplicate registration lists and other data
 * structures. The simulation kernel detects and reports double loading by
 * setting the __OPPSIM_LOADED__ envoronment variable on the first run and
 * testing it on subsequent library loads.
 *
 * To avoid this situation, we can use a separate 'opp_run_release' starter
 * (linked with release libraries) to start release-mode models
 * ('opp_run_release' just passes all command line options to evMain() directly).
 * To be able to start both debug and release models with the same 'opp_run'
 * command, we first try to detect whether the models provided on the command
 * line are linked with the release or debug simulation library. This is done
 * by dynamically loading the models and then checking for the
 * __is_release_oppsim__ symbol (which is present only in release-mode
 * simkernel). If we find that all the models were linked in debug mode, we
 * pass all command line options to evMain() and let OMNeT++ load the
 * libraries. If we find that the models are linked with release-mode simulation
 * kernel, we rather spawn 'opp_run_release' (which is linked in release-mode)
 * with the command line options. On Unix this is done by exec-ing the
 * 'opp_run_release' binary which will be loaded into the same address space.
 * Unfortunately on Windows it is not possible to load the executable into the
 * same process. Windows can only create a new process which inherits the
 * input/output of 'opp_run' and the parent process blocks until the child
 * finishes. The only drawback is that killing 'opp_run' would not kill
 * automatically the 'opp_run_release' child process (this means process
 * management from the Eclipse debug view would not be able to stop these
 * processes). To avoid this problem, the IDE detects also in what mode the
 * simulation is compiled (detects the active CDT configuration) and starts
 * the model directly using either 'opp_run' or 'opp_run_release' (= the IDE
 * will never invoke 'opp_run' if a model is compiled in release-mode).
 *
 * Limitations:
 *
 * - This method can test only the libraries specified on command line. If a
 *   release-mode shared model library is specified in the .ini file, it will
 *   not be correctly detected, and both debug and release kernel will be
 *   loaded, resulting in an error message. In this case the user have to use
 *   the command 'opp_run_release' to start the simulation.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <errno.h>
#include "platdep/platdefs.h"

#ifdef NDEBUG
#error "opp_run.cc must be compiled in 'debug' mode!"
#endif

typedef int (*intfunc_t)();

NAMESPACE_BEGIN
extern "C" int evMain(int argc, char *argv[]);
NAMESPACE_END

std::string lastLoadLibError;  // contains the error message after calling oppLoadLibrary()

static void splitFileName(const char *pathname, std::string& dir, std::string& fnameonly)
{
    if (!pathname || !*pathname)
    {
         dir = ".";
         fnameonly = "";
         return;
    }

    // find last "/" or "\"
    const char *s = pathname + strlen(pathname) - 1;
    s--; // ignore potential trailing "/"
    while (s>pathname && *s!='\\' && *s!='/') s--;
    const char *sep = s<=pathname ? NULL : s;

    // split along that
    if (!sep)
    {
        // no slash or colon
        if (strchr(pathname,':') || strcmp(pathname,".")==0 || strcmp(pathname,"..")==0)
        {
            fnameonly = "";
            dir = pathname;
        }
        else
        {
            fnameonly = pathname;
            dir = ".";
        }
    }
    else
    {
        fnameonly = s+1;
        dir = std::string(pathname, s-pathname+1);
    }
}

static std::string makeLibFileName(const char *libname, const char *prefix, const char *suffix)
{
     bool hasDir = strchr(libname, '/')!=NULL || strchr(libname, '\\')!=NULL;
     std::string dir, fileNameOnly;
     splitFileName(libname, dir, fileNameOnly);
     bool hasExt = strchr(fileNameOnly.c_str(), '.')!=NULL;

     std::string libFileName;
     if (hasExt)
         libFileName = libname;  // when an exact file name is given, leave it unchanged
     else if (hasDir)
         libFileName = dir + "/" + prefix + fileNameOnly + suffix;
     else
         libFileName = std::string(prefix) + fileNameOnly + suffix;
     return libFileName;
}

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <process.h>
#include "platdep/platmisc.h"

typedef HMODULE libhandle_t;

static libhandle_t oppLoadLibrary(const char *libname)
{
# ifdef __GNUC__
     std::string libFileName = makeLibFileName(libname, "lib", ".dll"); // MinGW
# else
     std::string libFileName = makeLibFileName(libname, "", ".dll"); // Visual C++
# endif
    HMODULE handle = LoadLibrary((char *)libFileName.c_str());
    lastLoadLibError = "";
    if (handle == NULL)
        lastLoadLibError = opp_getWindowsError(GetLastError());
    return handle;
}

static void *getSymbol(libhandle_t handle, const char *symbol)
{
    // note: GetProcAddress(), unlike dlsym(), does not look into modules
    // loaded indirectly; we have to emulate that with additional code.
    void *result = (void *)GetProcAddress(handle, symbol);
    if (result != NULL)
        return result;

    // look up among all loaded modules
    HMODULE handles[1024];
    DWORD bytesNeeded = 0;
    if (!EnumProcessModules(GetCurrentProcess(), handles, sizeof(handles), &bytesNeeded))
    {
        fprintf(stderr, "<!> Error: opp_run: Could not enumerate currently loaded modules.\n");
        exit(1);
    }
    int numModules = bytesNeeded / sizeof(HMODULE);

    if (numModules > 1024)
    {
        fprintf(stderr, "<!> Warning: opp_run: Too many modules loaded, some of them will be ignored. "
                        "opp_run may not detect correctly whether it requires release or debug libraries.\n\n"); // double "\n" to separate message from subsequent OMNeT++ startup banner
        numModules = 1024;
    }

    for (int i = 0; i < numModules; i++)
    {
        if ((result = (void *)GetProcAddress(handles[i], symbol)) != NULL)
            return result;
    }

    return result;
}

static int oppExec(const char *cmd, char *const argv[])
{
    int result = _spawnvp(_P_WAIT, cmd, argv);
    if (result == -1)
        fprintf(stderr, "<!> Error: opp_run: Cannot exec opp_run_release: %s\n", strerror(errno));

    return result;
}

#else

#include <unistd.h>
#include <dlfcn.h>

typedef void *libhandle_t;

static libhandle_t oppLoadLibrary(const char *libname)
{
    std::string libFileName = makeLibFileName(libname, "lib", SHARED_LIB_SUFFIX);
    libhandle_t handle = dlopen(libFileName.c_str(), RTLD_NOW|RTLD_GLOBAL);
    lastLoadLibError = "";
    if (handle == NULL)
        lastLoadLibError = dlerror();
    return handle;
}

static void *getSymbol(libhandle_t handle, const char *symbol)
{
    return dlsym(handle, symbol);
}

static int oppExec(const char *cmd, char *const argv[])
{
    execvp(cmd, argv);
    fprintf(stderr, "<!> Error: opp_run: Cannot exec opp_run_release: %s\n", strerror(errno));
    return 1;
}

#endif

/*
 * Loads all shared libraries specified on the command line and returns
 * whether they are using the debug version of oppsim. If the simkernel
 * is in release mode, it clears the __OPPSIM_LOADED__ environment variable
 * so the simulation can be restarted later with opp_run_release command
 * without the process inheriting the environment variable and throwing an error.
 *
 * Exits with error code 1 if there was an error while loading the libs.
 *
 * NOTE: modifies the __OPPSIM_LOADED__ environment variable (used in onstartup.cc)
 */
bool needsDebugSimkernel(int argc, char *argv[])
{
    // At this point the debug version of simulation lib is already loaded.
    // We clear  the environment variable so it will not report an error if
    // the release version of oppsim is loaded during the test
    putenv((char *)"__OPPSIM_LOADED__=no");

    for (int i=1; i<argc; i++)
    {
        const char *arg = argv[i];
        if (arg[0]=='-' && arg[1]=='l')
        {
            const char *libname;
            if (arg[2]!=0)        // accept both -llibname
                libname =  arg+2;
            else if (i+1<argc)    // and -l libname format
                libname = argv[i+1];
            else
            {
                fprintf(stderr, "<!> Error: opp_run: Missing library name at the end of command line after the -l command line option.\n");
                exit(1);
            }

            libhandle_t handle = oppLoadLibrary(libname);
            if (!handle && lastLoadLibError!="")
                fprintf(stderr, "<!> Warning: opp_run: Cannot check library %s: %s\n\n", libname, lastLoadLibError.c_str()); // double "\n" to separate message from subsequent OMNeT++ startup banner

            // Detect whether oppsim is release. If it was not loaded correctly, assume it is debug.
            if (handle && getSymbol(handle, "__is_release_oppsim__"))
            {
                // Release kernel. oppsim must be reloaded later by opp_run_release
                putenv((char *)"__OPPSIM_LOADED__=no");
                fflush(stderr);
                return false;
            }
        }
    }

    // debug kernel. oppsim already loaded and the loaded debug version will be used
    putenv((char *)"__OPPSIM_LOADED__=yes");
    fflush(stderr);
    return true;
}

int main(int argc, char *argv[])
{
    if (needsDebugSimkernel(argc, argv))
        return OPP::evMain(argc, argv);
    else
    {
        // The libs specified on the command line were compiled with release-mode simulation libraries
        // but opp_run is by definition a debug executable; we must delegate to opp_run_release with the
        // current command line arguments.
        return oppExec("opp_run_release", argv);
    }
}
