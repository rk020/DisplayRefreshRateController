#pragma once

#include <Windows.h>
#include <fstream>
#include "IntelDisplayController.h"

ctl_result_t IntelDisplayController::InitControlLibrary()
{
    ctl_result_t Result;
    ctl_init_args_t CtlInitArgs;

    if (IsInitialized)
        return CTL_RESULT_SUCCESS;

    ZeroMemory(&CtlInitArgs, sizeof(ctl_init_args_t));
    CtlInitArgs.AppVersion = CTL_MAKE_VERSION(CTL_IMPL_MAJOR_VERSION, CTL_IMPL_MINOR_VERSION);
    CtlInitArgs.flags = 0;
    CtlInitArgs.Size = sizeof(CtlInitArgs);
    CtlInitArgs.Version = 0;
    FillAppId(CtlInitArgs.ApplicationUID);

    Result = ctlInit(&CtlInitArgs, &ApiHandle);
    if (CTL_RESULT_SUCCESS != Result)
        return Result;

    IsInitialized = true;

    return CTL_RESULT_SUCCESS;
}

void IntelDisplayController::DestroyControlLibrary() const
{
    ctlClose(ApiHandle);
    if (pDevices != nullptr)
        free(pDevices);
}

void IntelDisplayController::FillPresetMapping()
{
    PresetMapping.insert(std::pair<uint32_t, RrPreset>(CTL_INTEL_ARC_SYNC_PROFILE_INVALID,
                                                       RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_INVALID, "INVALID"}));
    PresetMapping.insert(std::pair<uint32_t, RrPreset>(CTL_INTEL_ARC_SYNC_PROFILE_RECOMMENDED,
                                                       RrPreset{
                                                           CTL_INTEL_ARC_SYNC_PROFILE_RECOMMENDED, "RECOMMENDED"
                                                       }));
    PresetMapping.insert(std::pair<uint32_t, RrPreset>(CTL_INTEL_ARC_SYNC_PROFILE_EXCELLENT,
                                                       RrPreset{
                                                           CTL_INTEL_ARC_SYNC_PROFILE_EXCELLENT, "EXCELLENT"
                                                       }));
    PresetMapping.insert(std::pair<uint32_t, RrPreset>(CTL_INTEL_ARC_SYNC_PROFILE_GOOD,
                                                       RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_GOOD, "GOOD"}));
    PresetMapping.insert(std::pair<uint32_t, RrPreset>(CTL_INTEL_ARC_SYNC_PROFILE_COMPATIBLE,
                                                       RrPreset{
                                                           CTL_INTEL_ARC_SYNC_PROFILE_COMPATIBLE, "COMPATIBLE"
                                                       }));
    PresetMapping.insert(std::pair<uint32_t, RrPreset>(CTL_INTEL_ARC_SYNC_PROFILE_OFF,
                                                       RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_OFF, "OFF"}));
    PresetMapping.insert(std::pair<uint32_t, RrPreset>(CTL_INTEL_ARC_SYNC_PROFILE_VESA,
                                                       RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_VESA, "VESA"}));
    PresetMapping.insert(std::pair<uint32_t, RrPreset>(CTL_INTEL_ARC_SYNC_PROFILE_CUSTOM,
                                                       RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_CUSTOM, "CUSTOM"}));
}

void IntelDisplayController::FillAppId(ctl_application_id_t& ApplicationUID) const
{
    std::ifstream file("C:\\ControlLibraryAppId.txt");
    if (file.is_open())
    {
        std::string str;
        std::getline(file, str);
        ApplicationUID.Data1 = std::stoi(str);
        std::getline(file, str);
        ApplicationUID.Data2 = static_cast<uint16_t>(std::stoi(str));
        std::getline(file, str);
        ApplicationUID.Data3 = static_cast<uint16_t>(std::stoi(str));
        for (unsigned char& i : ApplicationUID.Data4)
        {
            std::getline(file, str);
            i = static_cast<uint8_t>(std::stoi(str));
        }
    }
}

std::string IntelDisplayController::GetPanelNameByType(ctl_display_output_types_t Type)
{
    switch (Type)
    {
    case CTL_DISPLAY_OUTPUT_TYPES_DISPLAYPORT: return "DP Intel Display";
    case CTL_DISPLAY_OUTPUT_TYPES_HDMI: return "HDMI Intel Display";
    case CTL_DISPLAY_OUTPUT_TYPES_DVI: return "DVI Intel Display";
    case CTL_DISPLAY_OUTPUT_TYPES_MIPI: return "MIPI Intel Display";
    case CTL_DISPLAY_OUTPUT_TYPES_CRT: return "CRT Intel Display";
    case CTL_DISPLAY_OUTPUT_TYPES_INVALID:
    case CTL_DISPLAY_OUTPUT_TYPES_MAX:
    default:
        return "Invalid Intel Display";
    }
}

std::vector<DisplayAdapter> IntelDisplayController::GetDisplayAdapters()
{
    ctl_result_t Result;
    ctl_device_adapter_properties_t StDeviceAdapterProperties = {0};
    uint32_t AdapterCount = 0;
    std::vector<DisplayAdapter> adapters;

    Result = ctlEnumerateDevices(ApiHandle, &AdapterCount, pDevices);
    if (CTL_RESULT_SUCCESS != Result)
        return adapters;

    pDevices = static_cast<ctl_device_adapter_handle_t*>(
        malloc(sizeof(ctl_device_adapter_handle_t) * AdapterCount));
    if (nullptr == pDevices)
        return adapters;

    Result = ctlEnumerateDevices(ApiHandle, &AdapterCount, pDevices);
    if (CTL_RESULT_SUCCESS != Result)
        return adapters;

    StDeviceAdapterProperties.Size = sizeof(ctl_device_adapter_properties_t);
    StDeviceAdapterProperties.pDeviceID = malloc(sizeof(LUID));
    StDeviceAdapterProperties.device_id_size = sizeof(_LUID);
    StDeviceAdapterProperties.Version = 2;

    for (uint32_t i = 0; i < AdapterCount; i++)
    {
        DisplayAdapter adapter;

        adapter.Name = "Intel DisplayPanel Adapter";

        Result = ctlGetDeviceProperties(pDevices[i], &StDeviceAdapterProperties);
        if (CTL_RESULT_ERROR_UNSUPPORTED_VERSION == Result) // reduce version if required & recheck
        {
            StDeviceAdapterProperties.Version = 0;
            Result = ctlGetDeviceProperties(pDevices[i], &StDeviceAdapterProperties);
        }

        if ((Result == CTL_RESULT_SUCCESS) && (CTL_DEVICE_TYPE_GRAPHICS == StDeviceAdapterProperties.device_type) &&
            (0x8086 == StDeviceAdapterProperties.pci_vendor_id))
        {
            adapter.Name = StDeviceAdapterProperties.name;
        }

        adapter.id = i;
        adapters.push_back(adapter);
    }

    if (nullptr != StDeviceAdapterProperties.pDeviceID)
    {
        free(StDeviceAdapterProperties.pDeviceID);
        StDeviceAdapterProperties.pDeviceID = nullptr;
    }

    return adapters;
}

std::vector<DisplayPanel> IntelDisplayController::GetDisplayPanels(DisplayAdapter* pAdapter)
{
    ctl_result_t Result;
    uint32_t DisplayCount = 0;
    std::vector<DisplayPanel> displays;

    if (CTL_RESULT_SUCCESS != InitControlLibrary())
        return displays;

    if (nullptr == pDevices)
        return displays;

    ctlEnumerateDisplayOutputs(pDevices[pAdapter->id], &DisplayCount, pDisplayHandles[pAdapter->id]);
    if (0 == DisplayCount)
        return displays;

    pDisplayHandles[pAdapter->id] = static_cast<ctl_display_output_handle_t*>(malloc(
        sizeof(ctl_display_output_handle_t) * DisplayCount));
    if (nullptr == pDisplayHandles[pAdapter->id])
        return displays;

    Result = ctlEnumerateDisplayOutputs(pDevices[pAdapter->id], &DisplayCount, pDisplayHandles[pAdapter->id]);
    if (CTL_RESULT_SUCCESS != Result)
        return displays;

    for (uint32_t i = 0; i < DisplayCount; i++)
    {
        ctl_display_properties_t stDisplayProperties = {};
        stDisplayProperties.Size = sizeof(ctl_display_properties_t);

        Result = ctlGetDisplayProperties(pDisplayHandles[pAdapter->id][i], &stDisplayProperties);
        if (CTL_RESULT_SUCCESS != Result)
            continue;

        const bool active = stDisplayProperties.DisplayConfigFlags & CTL_DISPLAY_CONFIG_FLAG_DISPLAY_ACTIVE;
        const bool attached = stDisplayProperties.DisplayConfigFlags & CTL_DISPLAY_CONFIG_FLAG_DISPLAY_ATTACHED;
        if ((false == active) || (false == attached))
            continue;

        DisplayPanel display;
        display.Name = GetPanelNameByType(stDisplayProperties.Type); // TODO: Get the correct name
        display.pParentAdapter = pAdapter;
        display.id = i;
        displays.push_back(display);
    }

    return displays;
}

DisplayRrCaps IntelDisplayController::GetDisplayRrCaps(DisplayPanel* pDisplay)
{
    DisplayRrCaps caps = {};
    ctl_intel_arc_sync_monitor_params_t StIntelArcSyncMonitorParams;

    StIntelArcSyncMonitorParams.Size = sizeof(ctl_intel_arc_sync_monitor_params_t);
    StIntelArcSyncMonitorParams.Version = 0;
    const ctl_result_t Result = ctlGetIntelArcSyncInfoForMonitor(
        pDisplayHandles[pDisplay->pParentAdapter->id][pDisplay->id], &StIntelArcSyncMonitorParams);
    if (CTL_RESULT_SUCCESS != Result)
        return caps;

    caps.IsMinRrChangeSupported = StIntelArcSyncMonitorParams.IsIntelArcSyncSupported;
    caps.IsMaxRrChangeSupported = StIntelArcSyncMonitorParams.IsIntelArcSyncSupported;
    caps.IsFrameDurationIncreaseToleranceSupported = StIntelArcSyncMonitorParams.IsIntelArcSyncSupported;
    caps.IsFrameDurationDecreaseToleranceSupported = StIntelArcSyncMonitorParams.IsIntelArcSyncSupported;
    caps.IsDisplayRrPresetsSupported = StIntelArcSyncMonitorParams.IsIntelArcSyncSupported;

    if (StIntelArcSyncMonitorParams.IsIntelArcSyncSupported)
    {
        caps.SupportedMinRr = StIntelArcSyncMonitorParams.MinimumRefreshRateInHz;
        caps.SupportedMaxRr = StIntelArcSyncMonitorParams.MaximumRefreshRateInHz;
        caps.SupportedFrameDurationDecreaseTolerance = StIntelArcSyncMonitorParams.MaxFrameTimeDecreaseInUs;
        caps.SupportedFrameDurationIncreaseTolerance = StIntelArcSyncMonitorParams.MaxFrameTimeIncreaseInUs;
    }

    return caps;
}

std::vector<RrPreset> IntelDisplayController::GetSupportedPresets(DisplayPanel* pDisplay)
{
    std::vector<RrPreset> presets;

    presets.push_back(RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_RECOMMENDED, "RECOMMENDED"});
    presets.push_back(RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_EXCELLENT, "EXCELLENT"});
    presets.push_back(RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_GOOD, "GOOD"});
    presets.push_back(RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_COMPATIBLE, "COMPATIBLE"});
    presets.push_back(RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_OFF, "OFF"});
    presets.push_back(RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_VESA, "VESA"});
    presets.push_back(RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_CUSTOM, "CUSTOM"});

    return presets;
}

RrPreset IntelDisplayController::GetRrPreset(DisplayPanel* pDisplay)
{
    ctl_intel_arc_sync_profile_params_t StIntelArcSyncProfileParams;

    StIntelArcSyncProfileParams.Size = sizeof(ctl_intel_arc_sync_profile_params_t);
    StIntelArcSyncProfileParams.Version = 0;
    const ctl_result_t Result = ctlGetIntelArcSyncProfile(
        pDisplayHandles[pDisplay->pParentAdapter->id][pDisplay->id],
        &StIntelArcSyncProfileParams);
    if (CTL_RESULT_SUCCESS != Result)
        return RrPreset{CTL_INTEL_ARC_SYNC_PROFILE_INVALID, "INVALID"};

    return PresetMapping.at(StIntelArcSyncProfileParams.IntelArcSyncProfile);
}

bool IntelDisplayController::SetRrPreset(DisplayPanel* pDisplay, RrPreset* pPreset)
{
    ctl_intel_arc_sync_profile_params_t StIntelArcSyncProfileParams = {};

    StIntelArcSyncProfileParams.Size = sizeof(ctl_intel_arc_sync_profile_params_t);
    StIntelArcSyncProfileParams.Version = 0;
    StIntelArcSyncProfileParams.IntelArcSyncProfile = static_cast<ctl_intel_arc_sync_profile_t>(pPreset->id);
    const ctl_result_t Result = ctlSetIntelArcSyncProfile(
        pDisplayHandles[pDisplay->pParentAdapter->id][pDisplay->id],
        &StIntelArcSyncProfileParams);
    return (CTL_RESULT_SUCCESS == Result);
}

DisplayRrState IntelDisplayController::GetRrState(DisplayPanel* pDisplay)
{
    DisplayRrState RrState = {};
    ctl_intel_arc_sync_profile_params_t StIntelArcSyncProfileParams;

    StIntelArcSyncProfileParams.Size = sizeof(ctl_intel_arc_sync_profile_params_t);
    StIntelArcSyncProfileParams.Version = 0;
    const ctl_result_t Result = ctlGetIntelArcSyncProfile(
        pDisplayHandles[pDisplay->pParentAdapter->id][pDisplay->id],
        &StIntelArcSyncProfileParams);
    if (CTL_RESULT_SUCCESS != Result)
        return RrState;

    RrState.MinRr = StIntelArcSyncProfileParams.MinRefreshRateInHz;
    RrState.MaxRr = StIntelArcSyncProfileParams.MaxRefreshRateInHz;
    RrState.FrameDurationIncreaseTolerance = static_cast<int>(StIntelArcSyncProfileParams.MaxFrameTimeIncreaseInUs);
    RrState.FrameDurationDecreaseTolerance = static_cast<int>(StIntelArcSyncProfileParams.MaxFrameTimeDecreaseInUs);

    return RrState;
}


bool IntelDisplayController::SetRrState(DisplayPanel* pDisplay, DisplayRrState* pRrState)
{
    ctl_intel_arc_sync_profile_params_t StIntelArcSyncProfileParams;
    ctl_result_t Result;

    StIntelArcSyncProfileParams.Size = sizeof(ctl_intel_arc_sync_profile_params_t);
    StIntelArcSyncProfileParams.Version = 0;
    StIntelArcSyncProfileParams.IntelArcSyncProfile = CTL_INTEL_ARC_SYNC_PROFILE_CUSTOM;

    // Min/Max RR must be in panel supported range
    StIntelArcSyncProfileParams.MinRefreshRateInHz = pRrState->MinRr;
    StIntelArcSyncProfileParams.MaxRefreshRateInHz = pRrState->MaxRr;
    StIntelArcSyncProfileParams.MaxFrameTimeIncreaseInUs = pRrState->FrameDurationIncreaseTolerance;
    StIntelArcSyncProfileParams.MaxFrameTimeDecreaseInUs = pRrState->FrameDurationDecreaseTolerance;

    Result = ctlSetIntelArcSyncProfile(pDisplayHandles[pDisplay->pParentAdapter->id][pDisplay->id],
                                       &StIntelArcSyncProfileParams);
    return (CTL_RESULT_SUCCESS == Result);
}
