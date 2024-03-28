#pragma once
#include "Intel/IntelDisplayControllerAdapter.h"


// DisplayRrController dialog
class DisplayRrController
{
public:
    IntelDisplayControllerAdapter IntelController;
    DisplayControllerInterface* pActiveController;

    std::vector<DisplayAdapter> AdapterList;
    DisplayAdapter* pActiveAdapter;

    std::vector<DisplayPanel> PanelList;
    DisplayPanel* pActivePanel;

    std::vector<RrPreset> RrPresetList;
    RrPreset *pActivePreset, *pSelectedPreset;

    DisplayRrCaps ActiveRrCaps;
    DisplayRrState ActiveRrState, SelectedRrState;

    virtual bool Init();
    virtual void SetActiveAdapter(DisplayControllerInterface*, DisplayAdapter*);
    virtual void SetActivePanel(DisplayControllerInterface*, DisplayPanel*);
    virtual bool Apply(int SelectedPreset);
};
