#pragma once

#include "../pch/Includes.h"
#include "../pch/globals.h"


extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Windows {

	HWND overlay_hwnd;
	HWND hwnd;
}

namespace Overlays {

	void Theme();

	void Init();
	void Render();
}