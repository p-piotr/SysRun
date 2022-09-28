#ifndef _REMOTE_MEMORY_OPERATIONS_H
#define _REMOTE_MEMORY_OPERATIONS_H

#include <Windows.h>

LPVOID AllocRemoteMemory(HANDLE hProcess, SIZE_T size, BOOL executable);
SIZE_T WriteRemoteMemory(HANDLE hProcess, LPVOID srcBuffer, LPVOID dstBuffer, SIZE_T size);
LPVOID AllocAndWriteRemoteMemory(HANDLE hProcess, LPVOID srcBuffer, SIZE_T size, BOOL executable, PSIZE_T dwBytesWritten);

#endif // _REMOTE_MEMORY_OPERATIONS_H