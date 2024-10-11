#pragma once
#include <Windows.h>

#include "DisplayRefreshRateController.h"

struct APP_CONTEXT
{
    HWND hWindow;
    WNDCLASSEXW WindowClass;

    bool CloseApp = false;

    DisplayRrController RrController;
    int currentAdapter = 0;
    int currentPanel = 0;
    int currentPreset = 0;
    int firstFps = 120;
};
