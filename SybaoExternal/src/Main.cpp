#include "../pch/Includes.h"
#include "../driver/driver.h"
#include "../Window/Overlays.h"


//present the imgui window and games
int main() {

	DWORD procId = Driver::GetProcessIdByName(L"cs2.exe");
	if (procId == NULL) {

		cout << "ProcId Not found!" << endl;
		system("pause");
	}

	uintptr_t modBase = Driver::GetModuleBaseAddress(procId, L"cs2.exe");

	cout << "ProcId: " << procId << endl;
	cout << "ModuleBase: 0x" << std::hex << modBase << endl;

	Overlays::Render();

	system("pause");
	return 0;
}