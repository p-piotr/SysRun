#include "RemoteMemoryOperations.h"
#include <Windows.h>
#include "Error.h"

LPVOID AllocRemoteMemory(HANDLE hProcess, SIZE_T size, BOOL executable)
{
	LPVOID buffer;
	DWORD protect;

	protect = executable ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
	buffer = VirtualAllocEx(hProcess, NULL, size, MEM_RESERVE | MEM_COMMIT, protect);
	if (buffer == NULL)
		DisplayLastErrorAndExit(TEXT("VirtualAllocEx"));
	return buffer;
}

SIZE_T WriteRemoteMemory(HANDLE hProcess, LPVOID srcBuffer, LPVOID dstBuffer, SIZE_T size)
{
	BOOL res;
	SIZE_T bytesWritten;

	res = WriteProcessMemory(hProcess, dstBuffer, srcBuffer, size, &bytesWritten);
	if (res == FALSE)
		DisplayLastErrorAndExit(TEXT("WriteProcessMemory"));
	return bytesWritten;
}

LPVOID AllocAndWriteRemoteMemory(HANDLE hProcess, LPVOID srcBuffer, SIZE_T size, BOOL executable, PSIZE_T dwBytesWritten)
{
	LPVOID remoteBuffer;
	SIZE_T bytesWritten;

	remoteBuffer = AllocRemoteMemory(hProcess, size, executable);
	bytesWritten = WriteRemoteMemory(hProcess, srcBuffer, remoteBuffer, size);

	if (dwBytesWritten != NULL)
		*dwBytesWritten = bytesWritten;

	return remoteBuffer;
}