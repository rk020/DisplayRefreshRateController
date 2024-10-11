#pragma once
#include "Intel/IntelDisplayController.h"


// DisplayRrController dialog
class DisplayRrController
{
public:
    IDisplayController* pActiveController = nullptr;

    std::vector<DisplayAdapter> AdapterList;
    DisplayAdapter* pActiveAdapter;

    std::vector<DisplayPanel> PanelList;
    DisplayPanel* pActivePanel;

    std::vector<RrPreset> RrPresetList;
    RrPreset *pActivePreset, *pSelectedPreset;

    DisplayRrCaps ActiveRrCaps;
    DisplayRrState ActiveRrState, SelectedRrState;

    virtual ~DisplayRrController();

    virtual bool Init();
    virtual void SetActiveAdapter(IDisplayController*, DisplayAdapter*);
    virtual void SetActivePanel(IDisplayController*, DisplayPanel*);
    virtual bool Apply(int SelectedPreset);
};
