#include "Overlays.h"

struct DX11Objects {
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* device_context = nullptr;
    IDXGISwapChain* swap_chain = nullptr;
    ID3D11RenderTargetView* main_render_target_view = nullptr;
};

ID3D11DepthStencilState* DepthStencilState_FALSE = nullptr;
ID3D11BlendState* BlendState_Alpha = nullptr;

void CreateRenderTarget(DX11Objects* dx) {
    ID3D11Texture2D* pBackBuffer = nullptr;
    dx->swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    dx->device->CreateRenderTargetView(pBackBuffer, NULL, &dx->main_render_target_view);
    pBackBuffer->Release();
}

void CleanupRenderTarget(DX11Objects* dx) {
    if (dx->main_render_target_view) {
        dx->main_render_target_view->Release();
        dx->main_render_target_view = nullptr;
    }
}

struct EnumData {
    std::string exeName;
    HWND resultHwnd;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    EnumData* data = reinterpret_cast<EnumData*>(lParam);

    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess) {
        char processPath[MAX_PATH];
        if (GetModuleFileNameExA(hProcess, nullptr, processPath, MAX_PATH)) {
            std::string processName = processPath;

            // Convert to lowercase
            std::transform(processName.begin(), processName.end(), processName.begin(),
                [](unsigned char c) { return std::tolower(c); });

            std::string targetLower = data->exeName;
            std::transform(targetLower.begin(), targetLower.end(), targetLower.begin(),
                [](unsigned char c) { return std::tolower(c); });

            if (processName.find(targetLower) != std::string::npos) {
                data->resultHwnd = hwnd;
                CloseHandle(hProcess);
                return FALSE; // stop enumeration
            }
        }
        CloseHandle(hProcess);
    }
    return TRUE; // continue enumeration
}

HWND FindTarget(std::string exeName) {
    EnumData data;
    data.exeName = exeName;
    data.resultHwnd = nullptr;

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return data.resultHwnd;
}

void GetWindowInfo(HWND target, RECT& rect, int& width, int& height) {
    GetClientRect(target, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    // Convert client coordinates to screen coordinates
    POINT pt = { rect.left, rect.top };
    ClientToScreen(target, &pt);
    rect.left = pt.x;
    rect.top = pt.y;
    pt = { rect.right, rect.bottom };
    ClientToScreen(target, &pt);
    rect.right = pt.x;
    rect.bottom = pt.y;
}

void UpdateOverlayPosition(HWND overlay, HWND target) {
    RECT target_rect;
    int width, height;
    GetWindowInfo(target, target_rect, width, height);

    SetWindowPos(overlay, HWND_TOPMOST, target_rect.left, target_rect.top,
        width, height, SWP_NOACTIVATE);
}

void SetupDepthStencilFalse(ID3D11Device* pDevice)
{
    D3D11_DEPTH_STENCIL_DESC desc = {};
    desc.DepthEnable = FALSE;              // Disable Z-testing
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    desc.StencilEnable = FALSE;

    pDevice->CreateDepthStencilState(&desc, &DepthStencilState_FALSE);
}

void Overlays::Theme( ) {

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Set a minimal Japanese-inspired color palette
    colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.94f, 1.00f); // Almost white
    colors[ImGuiCol_Border] = ImVec4(0.60f, 0.60f, 0.60f, 0.50f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.80f, 0.10f, 0.10f, 0.65f); // red hover
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.10f, 0.10f, 1.00f); // red active
    colors[ImGuiCol_TitleBg] = ImVec4(0.90f, 0.10f, 0.10f, 0.90f); // strong red
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.90f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.90f, 0.10f, 0.10f, 0.85f); // buttons red
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.10f, 0.10f, 0.85f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.60f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.90f, 0.10f, 0.10f, 0.45f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.90f, 0.10f, 0.10f, 0.65f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.90f, 0.10f, 0.10f, 0.85f);
    colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f); // dark text
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.90f, 0.10f, 0.10f, 0.80f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.90f, 0.10f, 0.10f, 1.00f);

    // Rounded corners and padding
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 4.0f;
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 6);
}

//Initialize ImGui Menu
void Overlays::Init( ) {

    if (Menu::showMenu)
    {
        ImGui::Begin("Sybao DX11 External");

        ImGui::Text("This is a minimal DirectX11 + Win32 ImGui example.");
        if (ImGui::Button("Exit"))
            PostQuitMessage(0);

        if (ImGui::Checkbox("Stream Proof Overlay", &Menu::StreamProof))

        ImGui::End();
    }
}

//Render ImGui
void Overlays::Render( ) {

    Windows::hwnd = FindTarget(Target::processNameRender);
    if (!Windows::hwnd) {
		cerr << "Target window not found!" << endl;
        return;
	}

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L,
        GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
        (L"ImGuiDx11"), NULL };
    RegisterClassEx(&wc);

    RECT target_rect;
    GetClientRect(Windows::hwnd, &target_rect);
    int width = target_rect.right - target_rect.left;
    int height = target_rect.bottom - target_rect.top;

    Windows::overlay_hwnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"Overlay",
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, wc.hInstance, NULL
    );

    UpdateOverlayPosition(Windows::overlay_hwnd, Windows::hwnd);

    SetLayeredWindowAttributes(Windows::overlay_hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);

    D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = Windows::overlay_hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    DX11Objects* dx = new DX11Objects();
    D3D_FEATURE_LEVEL createdFeatureLevel;
    D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &dx->swap_chain, &dx->device, &createdFeatureLevel, &dx->device_context);
    CreateRenderTarget(dx);

    SetWindowLongPtr(Windows::hwnd, GWLP_USERDATA, (LONG_PTR)dx);

    ShowWindow(Windows::overlay_hwnd, SW_SHOW);
    UpdateWindow(Windows::overlay_hwnd);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable imgui.ini
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    Theme();

    ImGui_ImplWin32_Init(Windows::overlay_hwnd);
    ImGui_ImplDX11_Init(dx->device, dx->device_context);

    io.Fonts->Build();

    SetupDepthStencilFalse(dx->device);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT)
    {

        if (!IsWindow(Windows::hwnd)) {
            MessageBoxA(NULL, "CS2 has been closed!", "Info", MB_ICONINFORMATION);
            break;
        }

        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        UpdateOverlayPosition(Windows::overlay_hwnd, Windows::hwnd);

        if (GetAsyncKeyState(VK_INSERT) & 1) {
            Menu::showMenu = !Menu::showMenu;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        Init();

        ImGui::Render();
        const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        dx->device_context->OMSetRenderTargets(1, &dx->main_render_target_view, NULL);
        dx->device_context->ClearRenderTargetView(dx->main_render_target_view, clear_color);

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        dx->swap_chain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupRenderTarget(dx);
    if (dx->swap_chain) dx->swap_chain->Release();
    if (dx->device_context) dx->device_context->Release();
    if (dx->device) dx->device->Release();
    delete dx;

    DestroyWindow(Windows::overlay_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    if (msg == WM_SIZE) {
        if (wParam != SIZE_MINIMIZED) {
            DX11Objects* dx = (DX11Objects*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
            if (dx && dx->swap_chain) {
                CleanupRenderTarget(dx);
                dx->swap_chain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget(dx);
            }
        }
        return 0;
    }
    if (msg == WM_SYSCOMMAND) {
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
    }
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}