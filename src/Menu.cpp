#include "Menu.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>

#include "resource.h"
#include "GlobalVars.h"

#include <dwmapi.h>

#pragma comment(lib, "d3d9.lib")

// Data
static HWND                     gWindow      = nullptr;
static LPDIRECT3D9              g_pD3D       = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static D3DPRESENT_PARAMETERS    g_d3dpp      = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

constexpr auto* windowName      = _T("Menu");
constexpr auto* windowClassName = _T("unknown");

namespace gFonts
{
    namespace segoeui
    {
        #include "Fonts/segoeui.hpp"
    }

    namespace framdit
    {
        #include "Fonts/framdit.hpp"
    }

    namespace ntailu
    {
        #include "Fonts/ntailu.hpp"
    }
}

bool Menu::init(HINSTANCE instance)
{
    int          preference;
    HICON        icon2;
    WNDCLASSEX   windowClassEx;
    ImFontConfig fontConfig;

    icon2 = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
    windowClassEx.cbSize        = sizeof(WNDCLASSEX);
    windowClassEx.style         = CS_CLASSDC;
    windowClassEx.lpfnWndProc   = WndProc;
    windowClassEx.cbClsExtra    = 0;
    windowClassEx.cbWndExtra    = 0;
    windowClassEx.hInstance     = instance;
    windowClassEx.hIcon         = icon2;
    windowClassEx.hCursor       = nullptr;
    windowClassEx.hbrBackground = nullptr;
    windowClassEx.lpszMenuName  = nullptr;
    windowClassEx.lpszClassName  = windowClassName;
    windowClassEx.hIconSm       = nullptr;
    RegisterClassEx(&windowClassEx);

    gWindow = CreateWindow(
        windowClassName,
        windowName,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        100, 
        100,
        800,
        600,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (!CreateDeviceD3D(gWindow)) {
        CleanupDeviceD3D();
        UnregisterClass(windowClassEx.lpszClassName, windowClassEx.hInstance);
        return false;
    }

    ShowWindow(gWindow, SW_SHOWDEFAULT);
    UpdateWindow(gWindow);

    /* remove rounded coreners of my window in Win 11 */
    preference = DWMWCP_DONOTROUND;
    DwmSetWindowAttribute(gWindow, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    fontConfig.FontDataOwnedByAtlas = false;
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF(gFonts::ntailu::rawData, sizeof(gFonts::ntailu::rawData), 20.f, &fontConfig);

    ImGui_ImplWin32_Init(gWindow);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    return true;
}

void Menu::runLoop()
{
    MSG                msg;
    bool               shouldBreak;
    ImVec2             zeroImVec2{ 0.f, 0.f };
    HRESULT            result;
    constexpr D3DCOLOR directXClearColor = 4278190080UL;
    constexpr int      windowFlags = ImGuiWindowFlags_NoBackground 
        | ImGuiWindowFlags_NoResize 
        | ImGuiWindowFlags_NoTitleBar 
        | ImGuiWindowFlags_NoMove 
        | ImGuiWindowFlags_NoCollapse;

    shouldBreak = false;
    while (shouldBreak == false) {
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                shouldBreak = true;
        }

        if (shouldBreak)
            break;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(zeroImVec2, ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
        if (ImGui::Begin("", nullptr, windowFlags)) {
            ImGui::Checkbox("prediction", &gGlobalVars->features.prediction);
            if (gGlobalVars->features.prediction) {
                ImGui::Checkbox("draw ball path", &gGlobalVars->features.esp.predicionPath);
                ImGui::Checkbox("draw shot state", &gGlobalVars->features.esp.drawShotState);
            }

            ImGui::PushItemWidth(150.f);
            ImGui::Combo("9 Ball Game rules", &gGlobalVars->features.nineBallGameRule, "pot any ball\0ball num 9\0");
            ImGui::Combo("automatic action", &gGlobalVars->features.automatic, "none\0aim\0play\0");
            if (gGlobalVars->features.automatic == 2) {
                ImGui::Combo("power mode", &gGlobalVars->features.powerMode, "Random\0Custom\0");
                switch (gGlobalVars->features.powerMode)
                {
                case 0:
                    ImGui::SliderInt2("bounds", &gGlobalVars->features.randPowerBounds[0], 10, 100, "%i", ImGuiSliderFlags_AlwaysClamp);
                    break;
                default:
                    ImGui::SliderFloat("max power", &gGlobalVars->features.autoPlayMaxPower, 10.f, 100.f, "%.f%%", ImGuiSliderFlags_AlwaysClamp);
                    break;
                }
            }

            ImGui::PopItemWidth();
            ImGui::Separator();
            ImGui::Checkbox("adBlock", &gGlobalVars->features.adBlock);
            ImGui::Separator();
            if (ImGui::Button("unload"))
                shouldBreak = true;

            ImGui::End();
        }

        ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(
            zeroImVec2,
            ImGui::GetIO().DisplaySize,
            ImColor(100, 10, 255, 100),
            ImColor(255, 0, 255, 255),
            ImColor(255, 255, 0, 255),
            ImColor(0, 255, 255, 255));

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, directXClearColor, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }
}

void Menu::end(HINSTANCE instance)
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(gWindow);
    UnregisterClass(windowClassName, instance);
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
