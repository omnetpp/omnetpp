//=========================================================================
//  SHAREDMEMORY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2019 Andras Varga
  Copyright (C) 2006-2019 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iostream>

#if defined(__linux__)
  #include <malloc.h>
  #include <sys/sysinfo.h>
#elif defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #define NOGDI
  #include <windows.h>
#elif defined(__APPLE__)
  #include <sys/types.h>
  #include <sys/sysctl.h>
  #include <mach/mach.h>
#endif

#if !defined(_WIN32)
  #include <unistd.h>
  #include <errno.h>
  #include <sys/mman.h>
  #include <sys/stat.h>        /* For mode constants */
  #include <fcntl.h>           /* For O_* constants */
#endif

#include <jni.h>

extern "C" {

JNIEXPORT void JNICALL Java_org_omnetpp_scave_engine_ScaveEngineJNI_createSharedMemory(JNIEnv* env, jobject clazz, jstring name, jlong size)
{
    const char *nameStr = env->GetStringUTFChars(name, nullptr);

#if defined(_WIN32)

    HANDLE hMapFile = CreateFileMapping(
      INVALID_HANDLE_VALUE,    // use paging file
      NULL,                    // default security
      PAGE_READWRITE,          // read/write access
      0,                       // maximum object size (high-order DWORD)
      size,                    // maximum object size (low-order DWORD)
      nameStr);                // name of mapping object

#else // POSIX (linux, mac)

    int fd = shm_open(nameStr, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (fd == -1)
        std::cout << "error opening shm fd" << std::endl;

    if (ftruncate(fd, size) == -1)
        std::cout << "error truncating shm file descriptor" << std::endl;

    if (close(fd) == -1)
        std::cout << "error closing shm fd" << std::endl;

#endif

    env->ReleaseStringUTFChars(name, nameStr);
}

JNIEXPORT jobject JNICALL Java_org_omnetpp_scave_engine_ScaveEngineJNI_mapSharedMemory(JNIEnv* env, jobject clazz, jstring name, jlong size)
{
    const char *nameStr = env->GetStringUTFChars(name, nullptr);

    // must be set by the platform specific fragments
    void *buffer = nullptr;

#if defined(_WIN32)

    // this did NOT work: HANDLE opened = OpenFileMapping(PAGE_READWRITE, FALSE, nameStr);

    HANDLE hMapFile = CreateFileMapping(
      INVALID_HANDLE_VALUE,    // use paging file
      NULL,                    // default security
      PAGE_READWRITE,          // read/write access
      0,                       // maximum object size (high-order DWORD)
      size,                       // maximum object size (low-order DWORD) - ignored, but must not be 0
      nameStr);                // name of mapping object

    if (GetLastError() != ERROR_ALREADY_EXISTS)
        std::cout << "trying to map a nonexistent mapping, so it was created instead... expect zeroes" << std::endl;

    if (hMapFile == NULL) {
        std::cout << "error opening file mapping" << std::endl;

        // char err[2048];
        // FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
        //               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
        // printf("%s\n", err);//just for the safe case
        // puts(err);
    }


    buffer = MapViewOfFile(
      hMapFile,   // handle to map object
      FILE_MAP_ALL_ACCESS, // read/write permission
      0, // no offset (high)
      0, // no offset (low)
      0); // no size, map the whole object

    if (buffer == NULL)
        std::cout << "error mapping view of file" << std::endl;

#else // POSIX (linux, mac)

    int fd = shm_open(nameStr, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
        std::cout << "error opening shm fd" << std::endl;


    buffer = mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED)
        std::cout << "error mmapping shm fd" << std::endl;

    if (close(fd) == -1)
        std::cout << "error closing shm fd" << std::endl;

#endif

    env->ReleaseStringUTFChars(name, nameStr);

    jobject directBuffer = env->NewDirectByteBuffer(buffer, size);
    return directBuffer;
}

JNIEXPORT void JNICALL Java_org_omnetpp_scave_engine_ScaveEngineJNI_unmapSharedMemory(JNIEnv* env, jobject clazz, jobject directBuffer)
{
    void *buffer = env->GetDirectBufferAddress(directBuffer);
    jlong capacity = env->GetDirectBufferCapacity(directBuffer);

#if defined(_WIN32)
    if (UnmapViewOfFile(buffer) == 0)
        std::cout << "error unmapping file view" << std::endl;
#else // POSIX (linux, mac)
    if (munmap(buffer, capacity) == -1)
        std::cout << "error unmapping buf" << std::endl;
#endif
}

JNIEXPORT void JNICALL Java_org_omnetpp_scave_engine_ScaveEngineJNI_removeSharedMemory(JNIEnv* env, jobject clazz, jstring name)
{
    const char *nameStr = env->GetStringUTFChars(name, nullptr);

#if defined(_WIN32)

    HANDLE hMapFile = CreateFileMapping(
      INVALID_HANDLE_VALUE,    // use paging file
      NULL,                    // default security
      PAGE_READWRITE,          // read/write access
      0,                       // maximum object size (high-order DWORD)
      1,                       // maximum object size (low-order DWORD) - ignored, but must not be 0
      nameStr);                // name of mapping object

    if (GetLastError() != ERROR_ALREADY_EXISTS)
        std::cout << "trying to remove a nonexistent mapping" << std::endl;

    if (CloseHandle(hMapFile) == 0)
        std::cout << "error closing file mapping handle" << std::endl;

#else // POSIX (linux, mac)
    if (shm_unlink(nameStr) == -1)
        std::cout << "error unlinking shm" << std::endl;
#endif

    env->ReleaseStringUTFChars(name, nameStr);
}

} // extern "C"
