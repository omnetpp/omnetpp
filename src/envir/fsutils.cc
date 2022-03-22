//==========================================================================
//  FSUTILS.CC - part of
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

#include <cstring>
#include "common/fileutil.h"
#include "omnetpp/onstartup.h"
#include "omnetpp/cexception.h"
#include "omnetpp/platdep/platmisc.h"
#include "fsutils.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

static std::string makeLibFileName(const char *libName, const char *namePrefix, const char *nameSuffix, const char *extension)
{
    bool hasDir = strchr(libName, '/') != nullptr || strchr(libName, '\\') != nullptr;
    std::string dir, fileName;
    splitFileName(libName, dir, fileName);
    bool hasExt = strchr(fileName.c_str(), '.') != nullptr;

    std::string libFileName;
    if (hasExt)
        libFileName = libName;  // when an exact file name is given, leave it unchanged (this is mostly for loading 3rd party libs)
    else if (hasDir)
        libFileName = dir + "/" + namePrefix + fileName + nameSuffix + extension;
    else
        libFileName = std::string(namePrefix) + fileName + nameSuffix + extension;
    return libFileName;
}

static bool opp_loadlibrary(const char *libName)
{
    const char *nameSuffix = LIBSUFFIX;
#if HAVE_DLOPEN
    std::string libFileName = makeLibFileName(libName, "lib", nameSuffix, SHARED_LIB_SUFFIX);
    if (!dlopen(libFileName.c_str(), RTLD_NOW|RTLD_GLOBAL))
        throw std::runtime_error(std::string("Cannot load library '") + libFileName + "': " + dlerror());
    return true;

#elif defined(_WIN32)
# ifdef __GNUC__
    std::string libFileName = makeLibFileName(libName, "lib", nameSuffix, ".dll");  // MinGW
# else
    std::string libFileName = makeLibFileName(libName, "", nameSuffix, ".dll");  // Visual C++
# endif
    if (!LoadLibrary((char *)libFileName.c_str()))
        throw std::runtime_error(std::string("Cannot load library '") + libFileName + "': " + opp_getWindowsError(GetLastError()));
    return true;

#else
    throw std::runtime_error(std::string("Cannot load library '") + libName + "': dlopen() syscall not available");
#endif
}

void loadExtensionLibrary(const char *lib)
{
    try {
        opp_loadlibrary(lib);
        CodeFragments::executeAll(CodeFragments::STARTUP);
    }
    catch (std::runtime_error& e) {
        throw cRuntimeError("%s", e.what());
    }
}

}  // namespace envir
}  // namespace omnetpp

