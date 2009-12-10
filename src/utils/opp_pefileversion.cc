//
// Utility to detect MS Visual C++ version, and return it in the exit code
//

#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#include <stdio.h>
#include <tchar.h>
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2)
    {
        printf("error: missing argument\n");
        return 0;
    }

    DWORD dwHandle = 0;
    DWORD dwSize = GetFileVersionInfoSize(argv[1], /*out*/ &dwHandle);
    if (dwSize == 0)
    {
        printf("error: file not found or no version info in file\n");
        return 0;
    }
    void* pData = malloc(dwSize);
    BOOL result = GetFileVersionInfo(argv[1], dwHandle, dwSize, pData);
    if (!result)
    {
        printf("error: cannot get version info from file\n");
        free(pData);
        return 0;
    }
    VS_FIXEDFILEINFO* pVerInfo = NULL;
    UINT nLen = 0;
    result = VerQueryValue(pData, _T("\\"), /*out*/ (void**)&pVerInfo, /*out*/ &nLen);
    if (!result)
    {
        printf("error: cannot query version info\n");
        free(pData);
        return 0;
    }
    WORD version = HIWORD(pVerInfo->dwFileVersionMS);
    free(pData);
    printf("%d\n", version);
    return version;
}
