#include "../pch/Includes.h"
#include "../driver/driver.h"
#include "../Window/Overlays.h"
#include "../pch/globals.h"

int Name( ) {

	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	// Optional but helps with C++ locale
	std::ios_base::sync_with_stdio(false);
	std::locale::global(std::locale("en_US.UTF-8"));
	std::wcout.imbue(std::locale());

	// Print box art
	std::wcout << LR"(
                                              ╭━━━╮╱╱╱╭╮
                                              ┃╭━╮┃╱╱╱┃┃
                                              ┃╰━━┳╮╱╭┫╰━┳━━┳━━╮
                                              ╰━━╮┃┃╱┃┃╭╮┃╭╮┃╭╮┃
                                              ┃╰━╯┃╰━╯┃╰╯┃╭╮┃╰╯┃
                                              ╰━━━┻━╮╭┻━━┻╯╰┻━━╯
                                              ╱╱╱╱╭━╯┃
                                              ╱╱╱╱╰━━╯
)" << std::endl;
	return 0;
}

//running the CS2 target to find the ProcId and ModuleBase
bool CS2( ) {

	DWORD procId = Driver::GetProcessIdByName(Target::processName);
	if (procId == NULL) {

		cerr << "ProcId Not found!" << endl;

		return true;
	}
	cout << "[*] ProcId: " << procId << endl;

	uintptr_t modBase = Driver::GetModuleBaseAddress(procId, Target::processName);
	if (modBase == NULL) {
		cerr << "ModuleBase Not found!" << endl;
		return true;
	}
	cout << "[*] ModuleBase: 0x" << std::hex << modBase << endl;

	return false;
}

//Set the opicity of the console
void Opicity(int n) {

	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_EXSTYLE);
	SetWindowLong(consoleWindow, GWL_EXSTYLE, style | WS_EX_LAYERED);

	//this set the opicity of the console already is n
	SetLayeredWindowAttributes(consoleWindow, 0, n, LWA_ALPHA);
}

//generate a random string of given length
std::string generateRandomString(size_t length) {
	const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	std::string result;
	for (size_t i = 0; i < length; ++i) {
		result += characters[rand() % characters.size()];
	}
	return result;
}

//convert string to wstring
std::wstring stringToWString(const std::string& str) {
	return std::wstring(str.begin(), str.end());
}

//thread function to change the title
DWORD WINAPI TitleThread(LPVOID lpParam) {
	while (true) {
		std::string randomTitle = generateRandomString(12);
		std::wstring wTitle = stringToWString(randomTitle);
		SetConsoleTitleW(wTitle.c_str());

		Sleep(50);
	}

	return 0;
}

//set The Title of the console to a random string every 50ms
void SetTheTitle() {

	srand(static_cast<unsigned int>(time(0)));

	CreateThread(NULL, 0, TitleThread, NULL, 0, NULL);

}

//present the imgui window and games
int main( ) {

	SetTheTitle();

	Opicity(180);

	Name();

	std::thread Running(CS2);
	Running.detach();

	std::thread OverlaysThread(Overlays::Render);
	OverlaysThread.detach();

	system( "pause" );
	return 0;
}