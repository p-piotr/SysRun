#ifndef _REMOTE_THREAD_ROUT_H
#define _REMOTE_THREAD_ROUT_H

#include "Connection.h"
#include <Windows.h>
#include "Handles.h"
#include "Tokens.h"

typedef BOOL(WINAPI* PCREATE_PROCESS_W)(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	);
typedef PCREATE_PROCESS_W LPCREATE_PROCESS_W;

typedef BOOL(WINAPI* PCREATE_PROCESS_AS_USER_W)(
	_In_opt_ HANDLE hToken,
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	);
typedef PCREATE_PROCESS_AS_USER_W LPCREATE_PROCESS_AS_USER_W;

typedef HANDLE(*PCREATE_THREAD)(
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ SIZE_T dwStackSize,
	_In_ LPTHREAD_START_ROUTINE lpStartAddress,
	_In_opt_ LPVOID lpParameter,
	_In_ DWORD dwCreationFlags,
	_Out_opt_ LPDWORD lpThreadId
	);
typedef PCREATE_THREAD LPCREATE_THREAD;

typedef BOOL(*PTERMINATE_THREAD)(
	_Inout_ HANDLE hThread,
	_In_ DWORD dwExitCode
	);
typedef PTERMINATE_THREAD LPTERMINATE_THREAD;

typedef BOOL(*PDUPLICATE_HANDLE)(
	_In_ HANDLE hSourceProcessHandle,
	_In_ HANDLE hSourceHandle,
	_In_ HANDLE hTargetProcessHandle,
	_Out_ LPHANDLE lpTargetHandle,
	_In_ DWORD dwDesiredAccess,
	_In_ BOOL bInheritHandle,
	_In_ DWORD dwOptions
	);
typedef PDUPLICATE_HANDLE LPDUPLICATE_HANDLE;

typedef BOOL(*PSET_HANDLE_INFORMATION)(
	_In_ HANDLE hObject,
	_In_ DWORD dwMask,
	_In_ DWORD dwFlags
	);
typedef PSET_HANDLE_INFORMATION LPSET_HANDLE_INFORMATION;

typedef HANDLE(*PGET_CURRENT_PROCESS)();
typedef PGET_CURRENT_PROCESS LPGET_CURRENT_PROCESS;

typedef HMODULE(*PLOAD_LIBRARY_W)(
	_In_ LPCWSTR lpLibFileName
	);
typedef PLOAD_LIBRARY_W LPLOAD_LIBRARY_W;

typedef FARPROC(*PGET_PROC_ADDRESS)(
	_In_ HMODULE hModule,
	_In_ LPCSTR lpProcName
	);
typedef PGET_PROC_ADDRESS LPGET_PROC_ADDRESS;

typedef HANDLE(*PCREATE_EVENT_W)(
	_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
	_In_ BOOL bManualReset,
	_In_ BOOL bInitialState,
	_In_opt_ LPCWSTR lpName
	);
typedef PCREATE_EVENT_W LPCREATE_EVENT_W;

typedef BOOL(*PSET_EVENT)(
	_In_ HANDLE hEvent
	);
typedef PSET_EVENT LPSET_EVENT;

typedef BOOL(*PCLOSE_HANDLE)(
	_In_ HANDLE hObject
	);
typedef PCLOSE_HANDLE LPCLOSE_HANDLE;

typedef HRESULT(WINAPI* PCREATE_PSEUDO_CONSOLE)(
	_In_ COORD size,
	_In_ HANDLE hInput,
	_In_ HANDLE hOutput,
	_In_ DWORD dwFlags,
	_Out_ HPCON* phPC
	);
typedef PCREATE_PSEUDO_CONSOLE LPCREATE_PSEUDO_CONSOLE;

typedef BOOL(*PINITIALIZE_PROC_THREAD_ATTRIBUTE_LIST)(
	_Out_opt_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
	_In_ DWORD dwAttributeCount,
	DWORD dwFlags,
	_Inout_ PSIZE_T lpSize
	);
typedef PINITIALIZE_PROC_THREAD_ATTRIBUTE_LIST LPINITIALIZE_PROC_THREAD_ATTRIBUTE_LIST;

typedef BOOL(*PUPDATE_PROC_THREAD_ATTRIBUTE)(
	_Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
	_In_ DWORD dwFlags,
	_In_ DWORD_PTR Attribute,
	_In_ PVOID lpValue,
	_In_ SIZE_T cbSize,
	_Out_opt_ PVOID lpPreviousValue,
	_In_opt_ PSIZE_T lpReturnSize
	);
typedef PUPDATE_PROC_THREAD_ATTRIBUTE LPUPDATE_PROC_THREAD_ATTRIBUTE;

typedef HANDLE(*PGET_PROCESS_HEAP)(void);
typedef PGET_PROCESS_HEAP LPGET_PROCESS_HEAP;

typedef LPVOID(*PHEAP_ALLOC)(
	_In_ HANDLE hHeap,
	_In_ DWORD dwFlags,
	_In_ SIZE_T dwBytes
	);
typedef PHEAP_ALLOC LPHEAP_ALLOC;

typedef PVOID(*PRTL_ALLOCATE_HEAP)(
	_In_ PVOID HeapHandle,
	_In_opt_ ULONG Flags,
	_In_ SIZE_T Size
	);
typedef PRTL_ALLOCATE_HEAP LPRTL_ALLOCATE_HEAP;

typedef LPVOID(*PVIRTUAL_ALLOC)(
	_In_opt_ LPVOID lpAddress,
	_In_ SIZE_T dwSize,
	_In_ DWORD flAllocationType,
	_In_ DWORD flProtect
	);
typedef PVIRTUAL_ALLOC LPVIRTUAL_ALLOC;

typedef BOOL(*PVIRTUAL_FREE)(
	_In_ LPVOID lpAddress,
	_In_ SIZE_T dwSize,
	_In_ DWORD dwFreeType
	);
typedef PVIRTUAL_FREE LPVIRTUAL_FREE;

typedef BOOL(*PHEAP_FREE)(
	_In_ HANDLE hHeap,
	_In_ DWORD dwFlags,
	_In_ LPVOID lpMem
	);
typedef PHEAP_FREE LPHEAP_FREE;

typedef BOOL(*PALLOC_CONSOLE)(void);
typedef PALLOC_CONSOLE LPALLOC_CONSOLE;

typedef BOOL(*PFREE_CONSOLE)(void);
typedef PFREE_CONSOLE LPFREE_CONSOLE;

typedef BOOL(*PCREATE_PIPE)(
	_Out_ PHANDLE pReadPipe,
	_Out_ PHANDLE pWritePipe,
	_In_opt_ LPSECURITY_ATTRIBUTES lpPipeAttributes,
	_In_ DWORD nSize
	);
typedef PCREATE_PIPE LPCREATE_PIPE;

typedef BOOL(*PREAD_FILE)(
	_In_ HANDLE hFile,
	_Out_ LPVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToRead,
	_Out_opt_ LPDWORD lpNumberOfBytesRead,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
	);
typedef PREAD_FILE LPREAD_FILE;

typedef BOOL(*PWRITE_FILE)(
	_In_ HANDLE hFile,
	_In_ LPVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToWrite,
	_Out_opt_ LPDWORD lpNumberOfBytesWritten,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
	);
typedef PWRITE_FILE LPWRITE_FILE;

typedef BOOL(WINAPI* PCANCEL_IO_EX)(
	_In_ HANDLE hFile,
	_In_opt_ LPOVERLAPPED lpOverlapped
	);
typedef PCANCEL_IO_EX LPCANCEL_IO_EX;

typedef BOOL(WINAPI* PCANCEL_SYNCHRONOUS_IO)(
	_In_ HANDLE hFile
	);
typedef PCANCEL_SYNCHRONOUS_IO LPCANCEL_SYNCHRONOUS_IO;

typedef BOOL(*PSET_COMM_TIMEOUTS)(
	_In_ HANDLE hFile,
	_In_ LPCOMMTIMEOUTS lpCommTimeouts
	);
typedef PSET_COMM_TIMEOUTS LPSET_COMM_TIMEOUTS;

typedef BOOL(WINAPI* PSET_STD_HANDLE)(
	_In_ DWORD nStdHandle,
	_In_ HANDLE hHandle
	);
typedef PSET_STD_HANDLE LPSET_STD_HANDLE;

typedef HANDLE(WINAPI* PGET_STD_HANDLE)(
	_In_ DWORD nStdHandle
	);
typedef PGET_STD_HANDLE LPGET_STD_HANDLE;

typedef HANDLE(WINAPI* PGET_STD_HANDLE)(
	_In_ DWORD nStdHandle
	);
typedef PGET_STD_HANDLE LPGET_STD_HANDLE;

typedef HANDLE(WINAPI *PCREATE_CONSOLE_SCREEN_BUFFER)(
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ const SECURITY_ATTRIBUTES* lpSecurityAttributes,
	_In_ DWORD dwFlags,
	_Reserved_ LPVOID lpScreenBufferData
);
typedef PCREATE_CONSOLE_SCREEN_BUFFER LPCREATE_CONSOLE_SCREEN_BUFFER;

typedef BOOL(WINAPI* PSET_CONSOLE_ACTIVE_SCREEN_BUFFER)(
	_In_ HANDLE hConsoleOutput
	);
typedef PSET_CONSOLE_ACTIVE_SCREEN_BUFFER LPSET_CONSOLE_ACTIVE_SCREEN_BUFFER;

typedef BOOL(WINAPI* PWRITE_CONSOLE_A)(
	_In_ HANDLE hConsoleOutput,
	_In_ const VOID* lpBuffer,
	_In_ DWORD nNumberOfCharsToWrite,
	_Out_opt_ LPDWORD lpNumberOfCharsWritten,
	_Reserved_ LPVOID lpReserved
	);
typedef PWRITE_CONSOLE_A LPWRITE_CONSOLE_A;

typedef BOOL(*PENUM_WINDOWS)(
	_In_ WNDENUMPROC lpEnumProc,
	_In_ LPARAM lParam
	);
typedef PENUM_WINDOWS LPENUM_WINDOWS;

typedef BOOL(*PSHOW_WINDOW)(
	_In_ HWND hwnd,
	_In_ int nCmdShow
	);
typedef PSHOW_WINDOW LPSHOW_WINDOW;

typedef DWORD(*PWAIT_FOR_SINGLE_OBJECT)(
	_In_ HANDLE hHandle,
	_In_ DWORD dwMilliseconds
	);
typedef PWAIT_FOR_SINGLE_OBJECT LPWAIT_FOR_SINGLE_OBJECT;

typedef void(WINAPI* PCLOSE_PSEUDO_CONSOLE)(
	_In_ HPCON hPC
	);
typedef PCLOSE_PSEUDO_CONSOLE LPCLOSE_PSEUDO_CONSOLE;

typedef BOOL(*PWRITE_PROCESS_MEMORY)(
	_In_ HANDLE hProcess,
	_In_ LPVOID lpBaseAddress,
	_In_ LPVOID lpBuffer,
	_In_ SIZE_T nSize,
	_Out_opt_ SIZE_T* lpNumberOfBytesWritten
	);
typedef PWRITE_PROCESS_MEMORY LPWRITE_PROCESS_MEMORY;

typedef BOOL(WINAPI* PSET_CONSOLE_OUTPUT_CP)(
	_In_ UINT wCodePageID
	);
typedef PSET_CONSOLE_OUTPUT_CP LPSET_CONSOLE_OUTPUT_CP;

typedef BOOL(*PIS_VALID_CODE_PAGE)(
	_In_ UINT CodePage
	);
typedef PIS_VALID_CODE_PAGE LPIS_VALID_CODE_PAGE;

typedef DWORD(*PGET_LAST_ERROR)();
typedef PGET_LAST_ERROR LPGET_LAST_ERROR;

typedef void(*PEXIT_THREAD)(
	_In_ DWORD dwExitCode
	);
typedef PEXIT_THREAD LPEXIT_THREAD;

typedef void(*PSLEEP)(
	_In_ DWORD dwMilliseconds
	);
typedef PSLEEP LPSLEEP;

typedef struct _REMOTE_THREAD_DATA
{
	// used to call inside remote thread
	LPLOAD_LIBRARY_W LoadLibraryW;
	LPGET_PROC_ADDRESS GetProcAddress;
	LPTHREAD_START_ROUTINE RemoteInputThread;
	LPTHREAD_START_ROUTINE RemoteOutputThread;
	LPCREATE_PROCESS_W CreateProcessW;
	LPCREATE_PROCESS_AS_USER_W CreateProcessAsUserW;
	LPCREATE_THREAD CreateThread;
	LPTERMINATE_THREAD TeminateThread;
	LPDUPLICATE_HANDLE DuplicateHandle;
	LPSET_HANDLE_INFORMATION SetHandleInformation;
	LPCLOSE_HANDLE CloseHandle;
	LPCREATE_PSEUDO_CONSOLE CreatePseudoConsole;
	LPCLOSE_PSEUDO_CONSOLE ClosePseudoConsole;
	LPINITIALIZE_PROC_THREAD_ATTRIBUTE_LIST InitializeProcThreadAttributeList;
	LPUPDATE_PROC_THREAD_ATTRIBUTE UpdateProcThreadAttribute;
	LPWRITE_PROCESS_MEMORY WriteProcessMemory;
	LPGET_CURRENT_PROCESS GetCurrentProcess;
	LPCREATE_PIPE CreatePipe;
	LPSET_EVENT SetEvent;
	LPVIRTUAL_ALLOC VirtualAlloc;
	LPVIRTUAL_FREE VirtualFree;
	LPREAD_FILE ReadFile;
	LPWRITE_FILE WriteFile;
	LPWAIT_FOR_SINGLE_OBJECT WaitForSingleObject;
	LPGET_LAST_ERROR GetLastError;
	LPEXIT_THREAD ExitThread;
	LPSLEEP Sleep;
	LPWSTR lpCommandLine;
	LPWSTR lpCreateProcessAsUserW;
	// used to signal execution success
	HANDLE hEventSuccess;
	HANDLE hEventFail;
	// used to manage console lifetime
	LPHANDLE hReturnConsoleProcess;
	// used for connection
	CONNECTION_THREAD_DATA ctd;
	// used for token management
	TOKEN_THREAD_DATA ttd;
	// used for console management
	HANDLE hRemoteProcess;
	// for internal use in remote threads only
	LOCAL_PIPE_HANDLES lpHandles;
	HANDLE hInputThread;
	HANDLE hOutputThread;
} REMOTE_THREAD_DATA, *PREMOTE_THREAD_DATA;

__declspec(safebuffers)
LPTHREAD_START_ROUTINE RemoteThreadRoutine(LPVOID lpParameter);

int Label1();

__declspec (safebuffers)
LPTHREAD_START_ROUTINE RemoteInputThread(LPVOID lpParameter);

int Label2();

__declspec (safebuffers)
LPTHREAD_START_ROUTINE RemoteOutputThread(LPVOID lpParameter);

int Label3();

#endif // _REMOTE_THREAD_ROUT_H