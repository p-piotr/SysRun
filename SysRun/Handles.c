#include "Handles.h"
#include <Windows.h>
#include "Error.h"

PINTERPROCESS_PIPE_HANDLES GetInterprocessPipeHandles(HANDLE hRemoteProcess)
{
	HANDLE hInputReadPipe, hInputWritePipe, hOutputReadPipe, hOutputWritePipe, hRemoteInputPipe, hRemoteOutputPipe, hCurrentProcess, hProcessHeap;
	PINTERPROCESS_PIPE_HANDLES pPipeHandles;
	SECURITY_ATTRIBUTES sa;
	BOOL res;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	hCurrentProcess = GetCurrentProcess();
	hProcessHeap = GetProcessHeap();
	if (hProcessHeap == NULL)
		DisplayLastErrorAndExit(TEXT("GetProcessHeap"));

	// input pipe			local -> remote
	res = CreatePipe(&hInputReadPipe, &hInputWritePipe, &sa, 0);
	if (res == FALSE)
		goto lCreatePipeFailed;

	res = SetHandleInformation(hInputWritePipe, HANDLE_FLAG_INHERIT, 0);
	if (res == FALSE)
		goto lSetHandleInformationFailed;

	// output, error pipe	remote -> local
	res = CreatePipe(&hOutputReadPipe, &hOutputWritePipe, &sa, 0);
	if (res == FALSE)
		goto lCreatePipeFailed;

	res = SetHandleInformation(hOutputReadPipe, HANDLE_FLAG_INHERIT, 0);
	if (res == FALSE)
		goto lCreatePipeFailed;

	res = DuplicateHandle(
		hCurrentProcess,
		hInputReadPipe,
		hRemoteProcess,
		&hRemoteInputPipe,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS
	);
	if (res == FALSE)
		goto lDuplicateHandleFailed;

	res = DuplicateHandle(
		hCurrentProcess,
		hOutputWritePipe,
		hRemoteProcess,
		&hRemoteOutputPipe,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS
	);
	if (res == FALSE)
		goto lDuplicateHandleFailed;

	pPipeHandles = (PINTERPROCESS_PIPE_HANDLES)HeapAlloc(hProcessHeap, HEAP_ZERO_MEMORY, sizeof(INTERPROCESS_PIPE_HANDLES));
	if (pPipeHandles == NULL)
		goto lHeapAllocFailed;

	pPipeHandles->Local.hInputReadPipe = hInputReadPipe;
	pPipeHandles->Local.hInputWritePipe = hInputWritePipe;
	pPipeHandles->Local.hOutputReadPipe = hOutputReadPipe;
	pPipeHandles->Local.hOutputWritePipe = hOutputWritePipe;

	pPipeHandles->Remote.hInputPipe = hRemoteInputPipe;
	pPipeHandles->Remote.hOutputPipe = hRemoteOutputPipe;

	return pPipeHandles;

lCreatePipeFailed:
	DisplayLastErrorAndExit(TEXT("CreatePipe"));
lSetHandleInformationFailed:
	DisplayLastErrorAndExit(TEXT("SetHandleInformation"));
lDuplicateHandleFailed:
	DisplayLastErrorAndExit(TEXT("DuplicateHandle"));
lHeapAllocFailed:
	DisplayLastErrorAndExit(TEXT("HeapAlloc"));
}