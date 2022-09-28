#ifndef _CONPTY_H
#define _CONPTY_H

#include <Windows.h>
#include "Handles.h"

#define INPUT_BUFFER_SIZE 1024
#define OUTPUT_BUFFER_SIZE 4096

DWORD ConsoleInputThread(LPVOID parameter); // parameter = input write handle
DWORD ConsoleOutputThread(LPVOID parameter); // parameter = output read handle

#endif // _CONPTY_H