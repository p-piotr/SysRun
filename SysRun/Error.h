#ifndef _ERROR_H
#define _ERROR_H

#include <Windows.h>

void DisplayErrorAndExit(LPWSTR procName, DWORD errorCode);
void DisplayLastErrorAndExit(LPWSTR procName);

#endif // _ERROR_H