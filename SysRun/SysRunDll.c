#include <stdio.h>
#include <Windows.h>
#include "Error.h"
#include "Tokens.h"

wchar_t dllPath[] = TEXT("Z:\\test_dll.dll");

#define DLL_NAME_SIZE (int)(sizeof(dllPath))

int main(int argc, char** argv)
{
	BOOL res;
	DWORD pid, dwRemoteThreadId;
	HANDLE hSysProcess, hRemoteThread; // hSysThread
	LPVOID lpRemoteDllName;
	LPTHREAD_START_ROUTINE lpLoadLibraryAddress;
	HMODULE hKernel32Mod;
	DEBUG_EVENT de;

	if (argc < 2)
	{
		printf("[-] Insufficient arguments.\n");
		ExitProcess(-1);
	}

	pid = atoi(argv[1]);
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

	res = DebugActiveProcess(pid);
	if (res == FALSE)
		DisplayErrorAndExit(TEXT("DebugActiveProcess"), GetLastError());
	printf("[+] Attached debugger successfully (debuggee PID: %u).\n", pid);

	res = DebugSetProcessKillOnExit(FALSE);
	if (res == FALSE)
		DisplayErrorAndExit(TEXT("DebugSetProcessKillOnExit"), GetLastError());

	do
	{
		res = WaitForDebugEvent(&de, INFINITE);
		if (res == FALSE)
			DisplayErrorAndExit(TEXT("WaitForDebugEvent"), GetLastError());
	}
	while (de.dwDebugEventCode != CREATE_PROCESS_DEBUG_EVENT);

	hSysProcess = de.u.CreateProcessInfo.hProcess;

	printf("[+] CREATE_PROCESS_DEBUG_EVENT occured, obtained process all-access handle: 0x%p. Writing remote data to load DLL module...\n", hSysProcess);

	lpRemoteDllName = VirtualAllocEx(hSysProcess, NULL, DLL_NAME_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (lpRemoteDllName == NULL)
		DisplayErrorAndExit(TEXT("VirtualAllocEx"), GetLastError());
	printf("[+] %u-bytes sized memory buffer allocated successfully.\n", DLL_NAME_SIZE);

	res = WriteProcessMemory(hSysProcess, lpRemoteDllName, dllPath, DLL_NAME_SIZE, NULL);
	if (res == FALSE)
		DisplayErrorAndExit(TEXT("WriteProcessMemory"), GetLastError());
	printf("[+] Library path successfully written to the buffer.\n");

	hKernel32Mod = GetModuleHandleW(TEXT("Kernel32.dll"));
	if (hKernel32Mod == NULL)
		DisplayErrorAndExit(TEXT("GetModuleHandleW"), GetLastError());
	lpLoadLibraryAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32Mod, "LoadLibraryW");
	if (lpLoadLibraryAddress == NULL)
		DisplayErrorAndExit(TEXT("GetProcAddress"), GetLastError());

	printf("[+] Successfully obtained LoadLibraryW procedure address - 0x%pn\n", lpLoadLibraryAddress);

	hRemoteThread = CreateRemoteThread(hSysProcess, NULL, 0, lpLoadLibraryAddress, lpRemoteDllName, 0, &dwRemoteThreadId);
	if (hRemoteThread == NULL)
		DisplayErrorAndExit(TEXT("CreateRemoteThread"), GetLastError());
	printf("[+] Remote thread created successfully! Thread ID: %u, thread handle: 0x%p\n", dwRemoteThreadId, hRemoteThread);

	res = ContinueDebugEvent(de.dwProcessId, de.dwThreadId, DBG_CONTINUE);
	if (res == FALSE)
		DisplayErrorAndExit(TEXT("ContinueDebugEvent"), GetLastError());
	DebugActiveProcessStop(pid);
	if (res == FALSE)
		DisplayErrorAndExit(TEXT("DebugActiveProcessStop"), GetLastError());

	printf("[+] Debugger disconnected successfully.\nExiting...\n");
	ExitProcess(0);
}