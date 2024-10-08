#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include "imgui_impl_win32.h"

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif

#define NUM_FRAMES_IN_FLIGHT 3
#define NUM_BACK_BUFFERS 3

struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64 FenceValue;
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Data
class D3DManager
{
public:
    FrameContext frameContext[NUM_FRAMES_IN_FLIGHT] = {};
    UINT frameIndex = 0;
    ID3D12Device* pd3dDevice = nullptr;
    ID3D12DescriptorHeap* pd3dRtvDescHeap = nullptr;
    ID3D12DescriptorHeap* pd3dSrvDescHeap = nullptr;
    ID3D12CommandQueue* pd3dCommandQueue = nullptr;
    ID3D12GraphicsCommandList* pd3dCommandList = nullptr;
    ID3D12Fence* fence = nullptr;
    HANDLE fenceEvent = nullptr;
    UINT64 fenceLastSignaledValue = 0;
    IDXGISwapChain3* pSwapChain = nullptr;
    HANDLE hSwapChainWaitableObject = nullptr;
    ID3D12Resource* mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
    D3D12_CPU_DESCRIPTOR_HANDLE mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

    // Forward declarations of helper functions
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    void WaitForLastSubmittedFrame();
    FrameContext* WaitForNextFrameResources();
};
