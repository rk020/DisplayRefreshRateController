#include "DisplayRefreshRateController.h"

bool DisplayRrController::Apply(int SelectedPreset)
{
    bool status;
    if (pSelectedPreset->Name == "CUSTOM")
    {
        status = pActiveController->SetRrState(pActivePanel, &ActiveRrState);
    }
    else
    {
        status = pActiveController->SetRrPreset(pActivePanel, &RrPresetList[SelectedPreset]);
    }

    pActivePreset = &RrPresetList[SelectedPreset];
    SetActivePanel(pActiveController, pActivePanel);
    return status;
}

void DisplayRrController::SetActivePanel(DisplayControllerInterface* pControlAdapter, DisplayPanel* pPanel)
{
    pActivePanel = pPanel;

    ActiveRrCaps = pControlAdapter->GetDisplayRrCaps(pActivePanel);

    if (ActiveRrCaps.IsDisplayRrPresetsSupported)
    {
        RrPresetList = pControlAdapter->GetSupportedPresets(pActivePanel);
        const RrPreset RrPreset = pControlAdapter->GetRrPreset(pActivePanel);

        for (auto& i : RrPresetList)
        {
            if (i.id == RrPreset.id)
            {
                pActivePreset = &i;
                pSelectedPreset = pActivePreset;
            }
        }
    }

    ActiveRrState = pControlAdapter->GetRrState(pActivePanel);
}

void DisplayRrController::SetActiveAdapter(DisplayControllerInterface* pControlAdapter,
                                           DisplayAdapter* pAdapter)
{
    if ((nullptr != pActiveAdapter) && (pActiveAdapter->id == pAdapter->id))
        return;

    pActiveAdapter = pAdapter;

    PanelList = pControlAdapter->GetDisplayPanels(pActiveAdapter);
    if (PanelList.empty())
    {
        return;
    }

    SetActivePanel(pControlAdapter, &PanelList[0]);
}

bool DisplayRrController::Init()
{
    IntelController = IntelDisplayControllerAdapter();
    pActiveController = &IntelController;

    AdapterList = pActiveController->GetDisplayAdapters();
    if (AdapterList.empty())
    {
        return false;
    }

    SetActiveAdapter(pActiveController, &AdapterList[0]);
    return true;
}
