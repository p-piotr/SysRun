#include "Tokens.h"
#include <Windows.h>
#include <stdio.h>
#include "Error.h"

const wchar_t* privilegeNames[PRIVILEGE_COUNT] =
{
	SE_CREATE_TOKEN_NAME,
	SE_ASSIGNPRIMARYTOKEN_NAME,
	SE_LOCK_MEMORY_NAME,
	SE_INCREASE_QUOTA_NAME,
	SE_UNSOLICITED_INPUT_NAME,
	SE_MACHINE_ACCOUNT_NAME,
	SE_TCB_NAME,
	SE_SECURITY_NAME,
	SE_TAKE_OWNERSHIP_NAME,
	SE_LOAD_DRIVER_NAME,
	SE_SYSTEM_PROFILE_NAME,
	SE_SYSTEMTIME_NAME,
	SE_PROF_SINGLE_PROCESS_NAME,
	SE_CREATE_PAGEFILE_NAME,
	SE_CREATE_PERMANENT_NAME,
	SE_BACKUP_NAME,
	SE_RESTORE_NAME,
	SE_SHUTDOWN_NAME,
	SE_DEBUG_NAME,
	SE_AUDIT_NAME,
	SE_SYSTEM_ENVIRONMENT_NAME,
	SE_CHANGE_NOTIFY_NAME,
	SE_REMOTE_SHUTDOWN_NAME,
	SE_UNDOCK_NAME,
	SE_SYNC_AGENT_NAME,
	SE_ENABLE_DELEGATION_NAME,
	SE_MANAGE_VOLUME_NAME,
	SE_IMPERSONATE_NAME,
	SE_CREATE_GLOBAL_NAME,
	SE_TRUSTED_CREDMAN_ACCESS_NAME,
	SE_RELABEL_NAME,
	SE_INC_WORKING_SET_NAME,
	SE_TIME_ZONE_NAME,
	SE_CREATE_SYMBOLIC_LINK_NAME,
	SE_DELEGATE_SESSION_USER_IMPERSONATE_NAME
}; // all privileges available in the system

BOOL SetPrivilege(HANDLE hToken, LPWSTR privilegeName, BOOL enablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	BOOL ret = FALSE;

	ret = LookupPrivilegeValueW(NULL, privilegeName, &luid);
	if (ret == FALSE)
		DisplayErrorAndExit(TEXT("LookupPrivilegeValueW"), GetLastError());

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = (enablePrivilege) ? SE_PRIVILEGE_ENABLED : 0;

	ret = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, NULL);
	if (ret == FALSE)
		DisplayErrorAndExit(TEXT("AdjustTokenPrivileges"), GetLastError());

	return ret;
}

BOOL CurrentProcessSetPrivilege(LPWSTR privilegeName, BOOL enablePrivilege)
{
	HANDLE hToken;
	BOOL ret, privilegeRet;

	ret = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	if (ret == FALSE)
		DisplayErrorAndExit(TEXT("OpenProcessToken"), GetLastError());

	privilegeRet = SetPrivilege(hToken, privilegeName, enablePrivilege);

	ret = CloseHandle(hToken);
	if (ret == FALSE)
		DisplayErrorAndExit(TEXT("CloseHandle"), GetLastError());

	return privilegeRet;
}