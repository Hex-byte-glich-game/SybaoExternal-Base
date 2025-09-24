#include "driver.h"

DWORD Driver::GetProcessIdByName(const std::wstring& processName) {
	DWORD processId = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe)) {
			do {
				if (processName == pe.szExeFile) {
					processId = pe.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &pe));
		}
		CloseHandle(hSnapshot);
	}
	return processId;
}

uintptr_t Driver::GetModuleBaseAddress(DWORD procId, const std::wstring& modName) {
	DWORD modBaseAddr = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 me;
		me.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &me)) {
			do {
				if (modName == me.szModule) {
					modBaseAddr = (DWORD)me.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &me));
		}
		CloseHandle(hSnapshot);
	}
	return modBaseAddr;
}

bool Driver::Read(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size) {
	SIZE_T bytesRead;
	return ReadProcessMemory(hProcess, (LPCVOID)address, buffer, size, &bytesRead) && bytesRead == size;
}

bool Driver::Write(HANDLE hProcess, uintptr_t address, void* buffer, SIZE_T size) {
	SIZE_T bytesWritten;
	return WriteProcessMemory(hProcess, (LPVOID)address, buffer, size, &bytesWritten) && bytesWritten == size;
}