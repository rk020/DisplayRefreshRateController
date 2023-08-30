#pragma once
#include "resource.h"		// main symbols


class CDisplayRefreshRateControllerApp : public CWinApp
{
public:
    CDisplayRefreshRateControllerApp();

    // Overrides
public:
    virtual BOOL InitInstance();
    // Implementation

    DECLARE_MESSAGE_MAP()
};

extern CDisplayRefreshRateControllerApp theApp;
