#ifndef _TOKENS_H
#define _TOKENS_H

#include <Windows.h>

#define PRIVILEGE_COUNT 35
#define SR_ADVAPI32DLL_NAME TEXT("Advapi32.dll")
#define SR_OPENPROCESSTOKEN_NAME "OpenProcessToken"
#define SR_ADJUSTTOKENPRIVILEGES_NAME "AdjustTokenPrivileges"

typedef struct _TOKEN_THREAD_DATA
{
	LPWSTR lpAdvapi32DllName;
	LPSTR lpOpenProcessToken;
	LPSTR lpAdjustTokenPrivileges;
	LPOPEN_PROCESS_TOKEN OpenProcessToken; // for internal use only
	LPADJUST_TOKEN_PRIVILEGES AdjustTokenPrivileges; // for internal use only
	TOKEN_PRIVILEGES* pTp;
} TOKEN_THREAD_DATA, * PTOKEN_THREAD_DATA;

const wchar_t* privilegeNames[PRIVILEGE_COUNT];

BOOL SetPrivilege(HANDLE hToken, LPWSTR privilegeName, BOOL enablePrivilege);
BOOL CurrentProcessSetPrivilege(LPWSTR privilegeName, BOOL enablePrivilege);

#endif // _TOKENS_H