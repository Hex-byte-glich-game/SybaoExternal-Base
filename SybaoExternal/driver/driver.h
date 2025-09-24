#pragma once

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <tlhelp32.h>

namespace Driver {

	DWORD GetProcessIdByName(const std::wstring& processName);
	uintptr_t GetModuleBaseAddress(DWORD procId, const std::wstring& modName);
	bool Read(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size);
	bool Write(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size);
}