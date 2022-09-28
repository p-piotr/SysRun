#include "Error.h"
#include <Windows.h>
#include <stdio.h>

void DisplayErrorAndExit(LPWSTR procName, DWORD errorCode)
{
	LPWSTR buffer;
	DWORD result;

	result = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errorCode,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		&buffer,
		0,
		NULL
	);

	if (result == 0)
		wprintf(TEXT("[!] %s failed with code %u - Cannot provide formatted message.\n"), procName, errorCode);
	else
		wprintf(TEXT("[!] %s failed with code %u - %s\n"), procName, errorCode, buffer);

	ExitProcess(-1);
}

void DisplayLastErrorAndExit(LPWSTR procName)
{
	DisplayErrorAndExit(procName, GetLastError());
}