//
// Check PE file version (Windows); used for detecting cl.exe version
//

%{
#ifdef _WIN32
# ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0501
# endif
# include <windows.h>
#endif

/**
 * Returns the version of a windows (PE) executable;
 * returns -1 if the version info cannot be detected or not Windows platform
 */
int getPEVersion(const char *fileName)
{
#ifdef _WIN32
    DWORD dwHandle = 0;
    DWORD dwSize = GetFileVersionInfoSize(fileName, /*out*/ &dwHandle);
    if (dwSize == 0) // file not found or no version info in file
        return -1;

    void* pData = malloc(dwSize);
    BOOL result = GetFileVersionInfo(fileName, dwHandle, dwSize, pData);
    if (!result) // cannot get version info from file
    {
        free(pData);
        return -1;
    }
    VS_FIXEDFILEINFO* pVerInfo = NULL;
    UINT nLen = 0;
    result = VerQueryValue(pData, "\\", /*out*/ (void**)&pVerInfo, /*out*/ &nLen);
    if (!result) // cannot query version info
    {
        free(pData);
        return -1;
    }
    WORD version = HIWORD(pVerInfo->dwFileVersionMS);
    free(pData);
    return version;
#else
    return -1;
#endif
}
%}

int getPEVersion(const char *fileName);


