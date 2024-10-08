#include <chrono>
#include <string>
#include <tchar.h>
#include <vector>
#include <Windows.h>

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_internal.h"

#include "D3DManager.h"
#include "DisplayRefreshRateController.h"

static D3DManager g_D3DManager = {};
static DisplayRrController RrController = {};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_D3DManager.pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            g_D3DManager.WaitForLastSubmittedFrame();
            g_D3DManager.CleanupRenderTarget();
            HRESULT result = g_D3DManager.pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam),
                                                                    DXGI_FORMAT_UNKNOWN,
                                                                    DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            assert(SUCCEEDED(result) && "Failed to resize swapchain.");
            g_D3DManager.CreateRenderTarget();
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
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

void delay(int delayInUs)
{
    const auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    while (true)
    {
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        if (duration.count() >= delayInUs)
            return;
    }
}

// Main code
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    RrController = DisplayRrController();
    const bool status = RrController.Init();
    if (!status)
        return -1;

    ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = {
        sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
        L"ImGui Example", nullptr
    };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Display Refresh Rate Controller",
                                WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, 200, 200,
                                800, 500, nullptr, nullptr, wc.hInstance, nullptr);

    if (!g_D3DManager.CreateDeviceD3D(hwnd))
    {
        g_D3DManager.CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Game-pad Controls

    ImGui::StyleColorsDark();

    // Setup Platform/Renderer back-ends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_D3DManager.pd3dDevice, NUM_FRAMES_IN_FLIGHT,
                        DXGI_FORMAT_R8G8B8A8_UNORM, g_D3DManager.pd3dSrvDescHeap,
                        g_D3DManager.pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                        g_D3DManager.pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    // Load Fonts
    io.FontDefault = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\Calibri.ttf)", 13.0f);

    // Our state
    constexpr ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static int currentAdapter = 0;
    static int currentPanel = 0;
    static int currentPreset = 0;
    static int firstFps = 120;

    for (int i = 0; i < RrController.RrPresetList.size(); i++)
        if (RrController.RrPresetList[i].id == RrController.pActivePreset->id)
            currentPreset = i;

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(800, 500));
            ImGui::Begin("Refresh Rate State", nullptr, ImGuiWindowFlags_NoDecoration);

            ImGui::Text("Select Adapter");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(250);
            if (ImGui::Combo("##combo1", &currentAdapter, [](void* vec, int idx, const char** out_text)
            {
                const std::vector<DisplayAdapter>& adapter = *static_cast<std::vector<DisplayAdapter>*>(vec);
                if (idx >= 0 && idx < static_cast<int>(adapter.size()))
                {
                    *out_text = adapter[idx].Name.c_str();
                    return true;
                }
                return false;
            }, &RrController.AdapterList, static_cast<int>(RrController.AdapterList.size())))
            {
                RrController.SetActiveAdapter(RrController.pActiveController,
                                              &RrController.AdapterList[currentAdapter]);
            }

            ImGui::SameLine();
            ImGui::Text("Select Panel");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            if (ImGui::Combo("##combo2", &currentPanel, [](void* vec, int idx, const char** out_text)
            {
                const std::vector<DisplayPanel>& panel = *static_cast<std::vector<DisplayPanel>*>(vec);
                if (idx >= 0 && idx < static_cast<int>(panel.size()))
                {
                    *out_text = panel[idx].Name.c_str();
                    return true;
                }
                return false;
            }, &RrController.PanelList, static_cast<int>(RrController.PanelList.size())))
            {
                RrController.SetActivePanel(RrController.pActiveController, &RrController.PanelList[currentPanel]);
            }

            ImGui::Dummy(ImVec2(0, 20));

            if (RrController.ActiveRrCaps.IsDisplayRrPresetsSupported)
            {
                ImGui::Text("Refresh Rate Preset");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(250);
                if (ImGui::Combo("##combo3", &currentPreset, [](void* vec, int idx, const char** out_text)
                {
                    const std::vector<RrPreset>& preset = *static_cast<std::vector<RrPreset>*>(vec);
                    if (idx >= 0 && idx < static_cast<int>(preset.size()))
                    {
                        *out_text = preset[idx].Name.c_str();
                        return true;
                    }
                    return false;
                }, &RrController.RrPresetList, static_cast<int>(RrController.RrPresetList.size())))
                {
                    RrController.pSelectedPreset = &RrController.RrPresetList[currentPreset];
                }

                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                if (ImGui::Button("Apply"))
                {
                    RrController.Apply(currentPreset);
                    for (int i = 0; i < RrController.RrPresetList.size(); i++)
                        if (RrController.RrPresetList[i].id == RrController.pActivePreset->id)
                            currentPreset = i;
                }

                ImGui::SameLine();
                ImGui::SetNextItemWidth(100);
                if (ImGui::Button("Refresh"))
                {
                    RrController.SetActivePanel(RrController.pActiveController, RrController.pActivePanel);
                    for (int i = 0; i < RrController.RrPresetList.size(); i++)
                        if (RrController.RrPresetList[i].id == RrController.pActivePreset->id)
                            currentPreset = i;
                }
            }

            ImGui::Dummy(ImVec2(0, 20));
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Adjust Refresh Rate Range (Gaming)");

            if (RrController.ActiveRrCaps.IsMinRrChangeSupported)
            {
                ImGui::SetNextItemWidth(150);
                ImGui::Text("Minimum RR (Hz)");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(250);
                if ("CUSTOM" == RrController.pSelectedPreset->Name)
                {
                    ImGui::SliderFloat("##slider1", &RrController.ActiveRrState.MinRr,
                                       RrController.ActiveRrCaps.SupportedMinRr,
                                       RrController.ActiveRrCaps.SupportedMaxRr);
                }
                else
                {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    ImGui::SliderFloat("##slider1", &RrController.ActiveRrState.MinRr,
                                       RrController.ActiveRrCaps.SupportedMinRr,
                                       RrController.ActiveRrCaps.SupportedMaxRr);
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar();
                }
            }

            if (RrController.ActiveRrCaps.IsMaxRrChangeSupported)
            {
                ImGui::SetNextItemWidth(150);
                ImGui::Text("Maximum RR (Hz)");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(250);
                if ("CUSTOM" == RrController.pSelectedPreset->Name)
                {
                    RrController.ActiveRrState.MaxRr = max(RrController.ActiveRrState.MinRr,
                                                           RrController.ActiveRrState.MaxRr);
                    ImGui::SliderFloat("##slider2", &RrController.ActiveRrState.MaxRr,
                                       RrController.ActiveRrState.MinRr,
                                       RrController.ActiveRrCaps.SupportedMaxRr);
                }
                else
                {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    ImGui::SliderFloat("##slider2", &RrController.ActiveRrState.MaxRr,
                                       RrController.ActiveRrState.MinRr,
                                       RrController.ActiveRrCaps.SupportedMaxRr);
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar();
                }
            }

            ImGui::Dummy(ImVec2(0, 20));
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f),
                               "Adjust how fast the RR can change. A lower value can reduce flicker. (0 indicates no limit)");

            int sliderMax = 65000; // 65ms
            if ((0 != static_cast<int>(RrController.ActiveRrCaps.SupportedMinRr)) && (0 != static_cast<int>(RrController
                .ActiveRrCaps.SupportedMaxRr)) && (RrController.ActiveRrCaps.SupportedMaxRr > RrController.ActiveRrCaps.
                SupportedMinRr))
                sliderMax = static_cast<int>(1000000 / RrController.ActiveRrCaps.SupportedMinRr - 1000000 / RrController
                    .ActiveRrCaps.SupportedMaxRr);

            if (RrController.ActiveRrCaps.IsFrameDurationIncreaseToleranceSupported)
            {
                ImGui::SetNextItemWidth(150);
                ImGui::Text("Frame Duration Increase Tolerance (us)");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(250);
                if ("CUSTOM" == RrController.pSelectedPreset->Name)
                {
                    ImGui::SliderInt("##slider3", &RrController.ActiveRrState.FrameDurationIncreaseTolerance, 0,
                                     sliderMax);
                }
                else
                {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    ImGui::SliderInt("##slider3", &RrController.ActiveRrState.FrameDurationIncreaseTolerance, 0,
                                     sliderMax);
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar();
                }
            }

            if (RrController.ActiveRrCaps.IsFrameDurationDecreaseToleranceSupported)
            {
                ImGui::SetNextItemWidth(150);
                ImGui::Text("Frame Duration Decrease Tolerance (us)");
                ImGui::SameLine();
                ImGui::SetNextItemWidth(250);
                if ("CUSTOM" == RrController.pSelectedPreset->Name)
                {
                    ImGui::SliderInt("##slider4", &RrController.ActiveRrState.FrameDurationDecreaseTolerance, 0,
                                     sliderMax);
                }
                else
                {
                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                    ImGui::SliderInt("##slider4", &RrController.ActiveRrState.FrameDurationDecreaseTolerance, 0,
                                     sliderMax);
                    ImGui::PopItemFlag();
                    ImGui::PopStyleVar();
                }
            }

            ImGui::Dummy(ImVec2(0, 20));
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Control the FPS");
            ImGui::SliderInt("##slider5", &firstFps, 20, 300);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            delay(1000000 / firstFps);
            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        FrameContext* frameCtx = g_D3DManager.WaitForNextFrameResources();
        const UINT backBufferIdx = g_D3DManager.pSwapChain->GetCurrentBackBufferIndex();
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = g_D3DManager.mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        g_D3DManager.pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
        g_D3DManager.pd3dCommandList->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        constexpr float clear_color_with_alpha[4] = {
            clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w
        };
        g_D3DManager.pd3dCommandList->ClearRenderTargetView(g_D3DManager.mainRenderTargetDescriptor[backBufferIdx],
                                                            clear_color_with_alpha, 0,
                                                            nullptr);
        g_D3DManager.pd3dCommandList->OMSetRenderTargets(1, &g_D3DManager.mainRenderTargetDescriptor[backBufferIdx],
                                                         FALSE, nullptr);
        g_D3DManager.pd3dCommandList->SetDescriptorHeaps(1, &g_D3DManager.pd3dSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_D3DManager.pd3dCommandList);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        g_D3DManager.pd3dCommandList->ResourceBarrier(1, &barrier);
        g_D3DManager.pd3dCommandList->Close();

        g_D3DManager.pd3dCommandQueue->ExecuteCommandLists(
            1, reinterpret_cast<ID3D12CommandList* const*>(&g_D3DManager.pd3dCommandList));

        // g_D3DManager.pSwapChain->Present(1, 0); // Present with vsync
        g_D3DManager.pSwapChain->Present(0, 0); // Present without vsync

        const UINT64 fenceValue = g_D3DManager.fenceLastSignaledValue + 1;
        g_D3DManager.pd3dCommandQueue->Signal(g_D3DManager.fence, fenceValue);
        g_D3DManager.fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;
    }

    g_D3DManager.WaitForLastSubmittedFrame();

    // Cleanup
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    g_D3DManager.CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}
