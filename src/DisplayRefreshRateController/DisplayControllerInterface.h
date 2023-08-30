#pragma once
#include <string>
#include <vector>

class DisplayAdapter
{
public:
    std::wstring Name;
    uint32_t id{};
};

class DisplayPanel
{
public:
    DisplayAdapter* pParentAdapter;
    std::wstring Name;
    uint32_t id{};
};

class DisplayRrCaps
{
public:
    bool IsMinRrChangeSupported = false;
    bool IsMaxRrChangeSupported = false;
    bool IsFrameDurationIncreaseToleranceSupported = false;
    bool IsFrameDurationDecreaseToleranceSupported = false;
    bool IsDisplayRrPresetsSupported = false;
    float SupportedMinRr{};
    float SupportedMaxRr{};
    uint32_t SupportedFrameDurationIncreaseTolerance{};
    uint32_t SupportedFrameDurationDecreaseTolerance{};
};

class DisplayRrState
{
public:
    float MinRr;
    float MaxRr;
    uint32_t FrameDurationIncreaseTolerance;
    uint32_t FrameDurationDecreaseTolerance;
};

class RrPreset
{
public:
    uint32_t id;
    std::wstring Name;
};

class DisplayControllerInterface
{
public:
    virtual std::vector<DisplayAdapter> GetDisplayAdapters() = 0;
    virtual std::vector<DisplayPanel> GetDisplayPanels(DisplayAdapter* pAdapter) = 0;

    virtual DisplayRrCaps GetDisplayRrCaps(DisplayPanel* pPanel) = 0;

    virtual std::vector<RrPreset> GetSupportedPresets(DisplayPanel* pPanel) = 0;
    virtual RrPreset GetRrPreset(DisplayPanel* pPanel) = 0;
    virtual bool SetRrPreset(DisplayPanel* pPanel, RrPreset* pPreset) = 0;

    virtual DisplayRrState GetRrState(DisplayPanel* pPanel) = 0;
    virtual bool SetRrState(DisplayPanel* pPanel, DisplayRrState* pRrState) = 0;
};
