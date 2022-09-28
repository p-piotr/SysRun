#include "ConPTY.h"
#include <Windows.h>
#include "Handles.h"
#include "Error.h"
#include "SysRunThread.h"

DWORD ConsoleInputThread(LPVOID parameter)
{
	char inputBuffer[INPUT_BUFFER_SIZE];
	HANDLE hInputRead, hInputWrite;
	DWORD bytesRead;

	hInputRead = GetStdHandle(STD_INPUT_HANDLE);
	hInputWrite = (HANDLE)parameter;

	while (1)
	{
		Sleep(50);
		ZeroMemory(inputBuffer, INPUT_BUFFER_SIZE);
		ReadFile(hInputRead, inputBuffer, INPUT_BUFFER_SIZE, &bytesRead, NULL);
		WriteFile(hInputWrite, inputBuffer, bytesRead, NULL, NULL);
	}
}

DWORD ConsoleOutputThread(LPVOID parameter)
{
	char outputBuffer[OUTPUT_BUFFER_SIZE];
	HANDLE hOutputRead, hOutputWrite;
	DWORD bytesRead;

	hOutputRead = (HANDLE)parameter;
	hOutputWrite = GetStdHandle(STD_OUTPUT_HANDLE);

	while (1)
	{
		Sleep(10);
		ZeroMemory(outputBuffer, OUTPUT_BUFFER_SIZE);
		ReadFile(hOutputRead, outputBuffer, OUTPUT_BUFFER_SIZE, &bytesRead, NULL);
		if (bytesRead > 0)
			WriteFile(hOutputWrite, outputBuffer, bytesRead, NULL, NULL);
	}
}