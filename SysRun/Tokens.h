#ifndef _TOKENS_H
#define _TOKENS_H

#include <Windows.h>

#define PRIVILEGE_COUNT 35
#define SR_ADVAPI32DLL_NAME TEXT("Advapi32.dll")
#define SR_OPENPROCESSTOKEN_NAME "OpenProcessToken"
#define SR_ADJUSTTOKENPRIVILEGES_NAME "AdjustTokenPrivileges"
#define SR_DUPLICATETOKENEX_NAME "DuplicateTokenEx"

typedef BOOL(*POPEN_PROCESS_TOKEN)(
	_In_ HANDLE ProcessHandle,
	_In_ DWORD DesiredAccess,
	_Out_ PHANDLE TokenHandle
	);
typedef POPEN_PROCESS_TOKEN LPOPEN_PROCESS_TOKEN;

typedef BOOL(*PADJUST_TOKEN_PRIVILEGES)(
	_In_ HANDLE TokenHandle,
	_In_ BOOL DisableAllPrivileges,
	_In_opt_ PTOKEN_PRIVILEGES NewState,
	_In_ DWORD BufferLength,
	_Out_opt_ PTOKEN_PRIVILEGES PreviousState,
	_Out_opt_ PDWORD ReturnLength
	);
typedef PADJUST_TOKEN_PRIVILEGES LPADJUST_TOKEN_PRIVILEGES;

typedef BOOL(*PDUPLICATE_TOKEN_EX)(
	_In_ HANDLE hExistingToken,
	_In_ DWORD dwDesiredAccess,
	_In_opt_ LPSECURITY_ATTRIBUTES lpTokenAttributes,
	_In_ SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
	_In_ TOKEN_TYPE TokenType,
	_Out_ PHANDLE phNewToken
	);
typedef PDUPLICATE_TOKEN_EX LPDUPLICATE_TOKEN_EX;

typedef BOOL(*PLOOKUP_PRIVILEGE_VALUE_W)(
	_In_opt_ LPCWSTR lpSystemName,
	_In_ LPCWSTR lpName,
	_Out_ PLUID lpLuid
	);
typedef PLOOKUP_PRIVILEGE_VALUE_W LPLOOKUP_PRIVILEGE_VALUE_W;

typedef struct _TOKEN_THREAD_DATA
{
	LPWSTR lpAdvapi32DllName;
	LPWSTR lpAssignPrimaryTokenPrivilege;
	LPSTR lpOpenProcessToken;
	LPSTR lpAdjustTokenPrivileges;
	LPSTR lpDuplicateTokenEx;
	LPSTR lpLookupPrivilegeValueW;
	LPSTR CreateProcessAsUserW;
	LPOPEN_PROCESS_TOKEN OpenProcessToken; // for internal use only
	LPADJUST_TOKEN_PRIVILEGES AdjustTokenPrivileges; // for internal use only
	LPDUPLICATE_TOKEN_EX DuplicateTokenEx; // for internal use only
	LPLOOKUP_PRIVILEGE_VALUE_W LookupPrivilegeValueW; // for internal use only
	TOKEN_PRIVILEGES* pTp;
} TOKEN_THREAD_DATA, * PTOKEN_THREAD_DATA;

const wchar_t* privilegeNames[PRIVILEGE_COUNT];

BOOL SetPrivilege(HANDLE hToken, LPWSTR privilegeName, BOOL enablePrivilege);
BOOL CurrentProcessSetPrivilege(LPWSTR privilegeName, BOOL enablePrivilege);

#endif // _TOKENS_H