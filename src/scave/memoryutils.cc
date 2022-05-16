//=========================================================================
//  MEMORYUTILS.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2019 Andras Varga
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "memoryutils.h"

#if defined(__linux__)
  #include <malloc.h>    // for ::malloc_trim
  #include <sys/sysinfo.h>
#elif defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOGDI
  #include <windows.h>
#elif defined(__APPLE__)
  #include <sys/types.h>
  #include <sys/sysctl.h>
  #include <mach/mach.h>
  #include <unistd.h>
#endif

namespace omnetpp {
namespace scave {

int malloc_trim()
{
#if defined(__linux__)
    return ::malloc_trim(0);
#else
    return 1;
#endif
}

int64_t getAvailableMemoryBytes()
{
#if defined(__linux__)

    struct sysinfo si;

    if (sysinfo(&si) != 0)
        return -1;

    return (int64_t)si.freeram * si.mem_unit;

#elif defined(_WIN32)

    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);

    if (GlobalMemoryStatusEx(&status) == 0)
        return -1;

    return status.ullAvailPhys;

#elif defined(__APPLE__)

    vm_statistics64_data_t vm_stat;
    int count = HOST_VM_INFO64_COUNT;

    if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (integer_t*)&vm_stat, (mach_msg_type_number_t*)&count) != KERN_SUCCESS)
        return -1;

    int pagesize = getpagesize();
    return vm_stat.free_count * pagesize;

#else
    // this Should Not Happen (r)
    return -1;
#endif
}

}  // namespace scave
}  // namespace omnetpp
