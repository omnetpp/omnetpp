/*
* Utility to create a hard link on Windows.
*
* Author: Andras Varga
*/

#define _WIN32_WINNT  0x0500  // Windows 2000 or later

#include <stdio.h>
#include <windows.h>

void main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "ln: creates a Windows hard link.\nUsage: ln <existingFile> <targetFile>\n");
        exit(argc==0 ? 0 : 1);
    }

    if (!CreateHardLink(argv[2], argv[1], NULL)) {
        LPVOID lpMsgBuf;
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                       FORMAT_MESSAGE_FROM_SYSTEM |
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       GetLastError(),
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR) &lpMsgBuf,
                       0,
                       NULL);
        fprintf(stderr, "ERROR: Cannot create hard link: %s", lpMsgBuf);
        LocalFree(lpMsgBuf);
        exit(1);
    }
    exit(0);
}
