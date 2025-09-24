#pragma once

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <chrono>
#include <algorithm>
#include <locale>
#include <Psapi.h>

#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"

using namespace std;