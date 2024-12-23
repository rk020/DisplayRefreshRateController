#pragma once

#include <map>

#include "../IDisplayController.h"
#include "igcl_api.h"

class IntelDisplayController final : public IDisplayController
{
    bool IsInitialized = false;
    ctl_api_handle_t ApiHandle = {};
    ctl_device_adapter_handle_t* pDevices = nullptr;
    ctl_display_output_handle_t* pDisplayHandles[4] = {}; // to store display handles of up to 4 adapters
    std::map<uint32_t, RrPreset> PresetMapping;

    ctl_result_t InitControlLibrary();
    void DestroyControlLibrary() const;
    void FillAppId(ctl_application_id_t& ApplicationUID) const;
    void FillPresetMapping();
    std::string GetPanelNameByType(ctl_display_output_types_t Type);

public:
    IntelDisplayController()
    {
        FillPresetMapping();
        InitControlLibrary();
    }

    ~IntelDisplayController()
    {
        DestroyControlLibrary();
    }

    std::vector<DisplayAdapter> GetDisplayAdapters() override;

    std::vector<DisplayPanel> GetDisplayPanels(DisplayAdapter* pAdapter) override;

    DisplayRrCaps GetDisplayRrCaps(DisplayPanel* pDisplay) override;

    std::vector<RrPreset> GetSupportedPresets(DisplayPanel* pDisplay) override;
    RrPreset GetRrPreset(DisplayPanel* pDisplay) override;
    bool SetRrPreset(DisplayPanel* pDisplay, RrPreset* pPreset) override;

    DisplayRrState GetRrState(DisplayPanel* pDisplay) override;
    bool SetRrState(DisplayPanel* pDisplay, DisplayRrState* pRrState) override;
};
