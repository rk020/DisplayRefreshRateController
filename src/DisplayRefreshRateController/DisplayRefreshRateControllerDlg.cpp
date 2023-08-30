#include <string>
#include "framework.h"
#include "DisplayRefreshRateController.h"
#include "DisplayRefreshRateControllerDlg.h"

#include <iomanip>

#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDisplayRefreshRateControllerDlg::CDisplayRefreshRateControllerDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DISPLAYREFRESHRATECONTROLLER_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDisplayRefreshRateControllerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDisplayRefreshRateControllerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_CBN_SELCHANGE(IDC_DISPLAY_LIST, &CDisplayRefreshRateControllerDlg::OnCbnSelchangeDisplayList)
    ON_CBN_SELCHANGE(IDC_ADAPTER_LIST, &CDisplayRefreshRateControllerDlg::OnCbnSelchangeAdapterList)
    ON_CBN_SELCHANGE(IDC_RR_PRESET_LIST, &CDisplayRefreshRateControllerDlg::OnCbnSelchangeRrPresetList)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, &CDisplayRefreshRateControllerDlg::OnBnClickedButtonApply)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MIN_RR, &CDisplayRefreshRateControllerDlg::OnNMCustomdrawSliderMinRr)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MAX_RR, &CDisplayRefreshRateControllerDlg::OnNMCustomdrawSliderMaxRr)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RR_INC_LIMIT, &CDisplayRefreshRateControllerDlg::OnNMCustomdrawSliderRrIncLimit)
    ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_RR_DEC_LIMIT, &CDisplayRefreshRateControllerDlg::OnNMCustomdrawSliderRrDecLimit)
END_MESSAGE_MAP()


void CDisplayRefreshRateControllerDlg::OnClose()
{
    CDialogEx::OnCancel();
}

void CDisplayRefreshRateControllerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    CDialogEx::OnSysCommand(nID, lParam);
}

void CDisplayRefreshRateControllerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CDisplayRefreshRateControllerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CDisplayRefreshRateControllerDlg::OnCbnSelchangeDisplayList()
{
    CComboBox* pPanelList = static_cast<CComboBox*>(GetDlgItem(IDC_DISPLAY_LIST));
    SetActivePanel(pActiveController, &PanelList[pPanelList->GetCurSel()]);
}


void CDisplayRefreshRateControllerDlg::OnCbnSelchangeAdapterList()
{
    CComboBox* pAdapterList = static_cast<CComboBox*>(GetDlgItem(IDC_ADAPTER_LIST));
    SetActiveAdapter(pActiveController, &AdapterList[pAdapterList->GetCurSel()]);
}

void CDisplayRefreshRateControllerDlg::OnCbnSelchangeRrPresetList()
{
    CSliderCtrl* pSlider;
    CComboBox* pPresetList = static_cast<CComboBox*>(GetDlgItem(IDC_RR_PRESET_LIST));
    pSelectedPreset = &RrPresetList[pPresetList->GetCurSel()];
    const std::vector<int> Sliders = {
        IDC_SLIDER_MIN_RR, IDC_SLIDER_MAX_RR, IDC_SLIDER_RR_INC_LIMIT, IDC_SLIDER_RR_DEC_LIMIT
    };

    for (const auto id : Sliders)
    {
        pSlider = static_cast<CSliderCtrl*>(GetDlgItem(id));
        pSlider->EnableWindow((pSelectedPreset->Name == L"CUSTOM"));
    }
}

void CDisplayRefreshRateControllerDlg::OnNMCustomdrawSliderMinRr(NMHDR* pNMHDR, LRESULT* pResult)
{
    int currentMin;
    CStatic* pLabelMin;
    CSliderCtrl *pSliderMin, *pSliderMax;
    std::wstringstream stream;

    pSliderMin = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_MIN_RR));
    pSliderMax = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_MAX_RR));

    pLabelMin = static_cast<CStatic*>(GetDlgItem(IDC_MIN_RR_LABEL));

    currentMin = pSliderMin->GetPos();

    pSliderMax->SetRange(currentMin, pSliderMax->GetRangeMax(), 1);

    stream << std::fixed << std::setprecision(2) << pSliderMin->GetPos() << L" Hz";
    pLabelMin->SetWindowText(stream.str().c_str());

    *pResult = 0;
}


void CDisplayRefreshRateControllerDlg::OnNMCustomdrawSliderMaxRr(NMHDR* pNMHDR, LRESULT* pResult)
{
    CStatic* pLabelMax;
    CSliderCtrl* pSliderMax;
    std::wstringstream stream;

    pSliderMax = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_MAX_RR));
    pLabelMax = static_cast<CStatic*>(GetDlgItem(IDC_MAX_RR_LABEL));

    stream << std::fixed << std::setprecision(2) << pSliderMax->GetPos() << L" Hz";
    pLabelMax->SetWindowText(stream.str().c_str());

    *pResult = 0;
}


void CDisplayRefreshRateControllerDlg::OnNMCustomdrawSliderRrIncLimit(NMHDR* pNMHDR, LRESULT* pResult)
{
    CStatic* pLabel;
    CSliderCtrl* pSlider;
    std::wstringstream stream;

    pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_RR_INC_LIMIT));
    pLabel = static_cast<CStatic*>(GetDlgItem(IDC_INCREASE_LIMIT_LABEL));

    stream << std::fixed << std::setprecision(2) << pSlider->GetPos() << L" us";
    pLabel->SetWindowText(stream.str().c_str());

    *pResult = 0;
}


void CDisplayRefreshRateControllerDlg::OnNMCustomdrawSliderRrDecLimit(NMHDR* pNMHDR, LRESULT* pResult)
{
    CStatic* pLabel;
    CSliderCtrl* pSlider;
    std::wstringstream stream;

    pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_RR_DEC_LIMIT));
    pLabel = static_cast<CStatic*>(GetDlgItem(IDC_DECREASE_LIMIT_LABEL));

    stream << std::fixed << std::setprecision(2) << pSlider->GetPos() << L" us";
    pLabel->SetWindowText(stream.str().c_str());

    *pResult = 0;
}

void CDisplayRefreshRateControllerDlg::OnBnClickedButtonApply()
{
    bool status;
    CSliderCtrl* pSlider;
    CComboBox* pPresetList = static_cast<CComboBox*>(GetDlgItem(IDC_RR_PRESET_LIST));

    if (pSelectedPreset->Name == L"CUSTOM")
    {
        DisplayRrState NewRrState{};

        pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_MIN_RR));
        NewRrState.MinRr = static_cast<float>(pSlider->GetPos());
        pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_MAX_RR));
        NewRrState.MaxRr = static_cast<float>(pSlider->GetPos());
        pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_RR_INC_LIMIT));
        NewRrState.FrameDurationIncreaseTolerance = pSlider->GetPos();
        pSlider = static_cast<CSliderCtrl*>(GetDlgItem(IDC_SLIDER_RR_DEC_LIMIT));
        NewRrState.FrameDurationDecreaseTolerance = pSlider->GetPos();

        status = pActiveController->SetRrState(pActivePanel, &NewRrState);
    }
    else
    {
        status = pActiveController->SetRrPreset(pActivePanel, &RrPresetList[pPresetList->GetCurSel()]);
    }

    if (!status)
    {
        MessageBoxW(L"Failed to apply RR preset", 0, 0);
        return;
    }

    MessageBoxW(L"Updated the settings successfully", 0, 0);

    pActivePreset = &RrPresetList[pPresetList->GetCurSel()];
    SetActivePanel(pActiveController, pActivePanel);
}

void CDisplayRefreshRateControllerDlg::ResetPanelUiElements()
{
    const std::vector<int> ComboBoxes = {IDC_RR_PRESET_LIST};
    for (const auto id : ComboBoxes)
    {
        CComboBox* pComboBox = static_cast<CComboBox*>(GetDlgItem(id));
        pComboBox->ResetContent();
        pComboBox->ShowWindow(SW_HIDE);
    }

    const std::vector<int> Sliders = {
        IDC_SLIDER_MIN_RR, IDC_SLIDER_MAX_RR, IDC_SLIDER_RR_INC_LIMIT, IDC_SLIDER_RR_DEC_LIMIT
    };
    for (const auto id : Sliders)
    {
        CSliderCtrl* pSlider = static_cast<CSliderCtrl*>(GetDlgItem(id));
        pSlider->ShowWindow(SW_HIDE);
    }

    const std::vector<int> Labels = {
        IDC_STATIC_PRESET, IDC_STATIC_MIN_RR, IDC_STATIC_MAX_RR, IDC_STATIC_INC_LIMIT, IDC_STATIC_DEC_LIMIT,
        IDC_MIN_RR_LABEL, IDC_MAX_RR_LABEL, IDC_INCREASE_LIMIT_LABEL, IDC_DECREASE_LIMIT_LABEL
    };
    for (const auto id : Labels)
    {
        CStatic* pLabel = static_cast<CStatic*>(GetDlgItem(id));
        pLabel->ShowWindow(SW_HIDE);
    }

    CButton* pButton = static_cast<CButton*>(GetDlgItem(IDC_BUTTON_APPLY));
    pButton->ShowWindow(SW_HIDE);
}

void CDisplayRefreshRateControllerDlg::ResetAdapterUiElements()
{
    const std::vector<int> ComboBoxes = {IDC_DISPLAY_LIST};
    for (const auto id : ComboBoxes)
    {
        CComboBox* pComboBox = static_cast<CComboBox*>(GetDlgItem(id));
        pComboBox->ResetContent();
        pComboBox->ShowWindow(SW_HIDE);
    }

    const std::vector<int> Labels = {};

    ResetPanelUiElements();
}

void CDisplayRefreshRateControllerDlg::SetRrControl(int staticId, int labelId, int sliderId, float labelValue,
                                                    const std::wstring labelUnit, float sliderMin, float sliderMax)
{
    CStatic* pLabel;
    CSliderCtrl* pSlider;
    std::wstringstream stream;

    pLabel = static_cast<CStatic*>(GetDlgItem(staticId));
    pLabel->ShowWindow(SW_SHOW);
    pLabel = static_cast<CStatic*>(GetDlgItem(labelId));
    stream << std::fixed << std::setprecision(2) << labelValue << L" " << labelUnit;
    pLabel->SetWindowText(stream.str().c_str());
    pLabel->ShowWindow(SW_SHOW);

    pSlider = static_cast<CSliderCtrl*>(GetDlgItem(sliderId));
    pSlider->SetRange(static_cast<int>(sliderMin), static_cast<int>(sliderMax), 0);
    pSlider->SetPos(static_cast<int>(labelValue));
    pSlider->ShowWindow(SW_SHOW);
    pSlider->EnableWindow((pActivePreset->Name == L"CUSTOM"));
}

void CDisplayRefreshRateControllerDlg::SetActivePanel(DisplayControllerInterface* pControlAdapter, DisplayPanel* pPanel)
{
    std::wstringstream stream;
    CStatic* pLabel;

    // if ((nullptr != pActivePanel) && (pActivePanel->pParentAdapter->id == pPanel->pParentAdapter->id) && (pActivePanel->
    //     id == pPanel->id))
    //     return;

    ResetPanelUiElements();

    pActivePanel = pPanel;

    DisplayRrCaps RrCaps = pControlAdapter->GetDisplayRrCaps(pActivePanel);

    if (RrCaps.IsDisplayRrPresetsSupported)
    {
        pLabel = static_cast<CStatic*>(GetDlgItem(IDC_STATIC_PRESET));
        pLabel->ShowWindow(SW_SHOW);

        RrPresetList = pControlAdapter->GetSupportedPresets(pActivePanel);
        RrPreset RrPreset = pControlAdapter->GetRrPreset(pActivePanel);

        CComboBox* pPresetList = static_cast<CComboBox*>(GetDlgItem(IDC_RR_PRESET_LIST));
        pPresetList->ResetContent();
        for (int i = 0; i < RrPresetList.size(); i++)
        {
            pPresetList->InsertString(i, RrPresetList[i].Name.c_str());
            if (RrPresetList[i].id == RrPreset.id)
            {
                pActivePreset = &RrPresetList[i];
                pSelectedPreset = pActivePreset;
                pPresetList->SetCurSel(i);
            }
        }
        pPresetList->ShowWindow(SW_SHOW);
    }

    DisplayRrState RrState = pControlAdapter->GetRrState(pActivePanel);

    if (RrCaps.IsMinRrChangeSupported)
        SetRrControl(IDC_STATIC_MIN_RR, IDC_MIN_RR_LABEL, IDC_SLIDER_MIN_RR, RrState.MinRr, L"Hz",
                     RrCaps.SupportedMinRr, RrCaps.SupportedMaxRr);

    if (RrCaps.IsMaxRrChangeSupported)
        SetRrControl(IDC_STATIC_MAX_RR, IDC_MAX_RR_LABEL, IDC_SLIDER_MAX_RR, RrState.MaxRr, L"Hz",
                     RrCaps.SupportedMinRr, RrCaps.SupportedMaxRr);

    int sliderMax = 65000; // 65ms
    if ((0 != static_cast<int>(RrCaps.SupportedMinRr)) && (0 != static_cast<int>(RrCaps.SupportedMaxRr)) && (RrCaps.
        SupportedMaxRr > RrCaps.SupportedMinRr))
        sliderMax = static_cast<int>(1000000 / RrCaps.SupportedMinRr - 1000000 / RrCaps.SupportedMaxRr);

    if (RrCaps.IsFrameDurationIncreaseToleranceSupported)
        SetRrControl(IDC_STATIC_INC_LIMIT, IDC_INCREASE_LIMIT_LABEL, IDC_SLIDER_RR_INC_LIMIT,
                     static_cast<float>(RrState.FrameDurationIncreaseTolerance), L"us", 0,
                     static_cast<float>(sliderMax));

    if (RrCaps.IsFrameDurationDecreaseToleranceSupported)
        SetRrControl(IDC_STATIC_DEC_LIMIT, IDC_DECREASE_LIMIT_LABEL, IDC_SLIDER_RR_DEC_LIMIT,
                     static_cast<float>(RrState.FrameDurationDecreaseTolerance), L"us", 0,
                     static_cast<float>(sliderMax));

    if (RrCaps.IsDisplayRrPresetsSupported)
    {
        CButton* pButton = static_cast<CButton*>(GetDlgItem(IDC_BUTTON_APPLY));
        pButton->ShowWindow(SW_SHOW);
    }
}

void CDisplayRefreshRateControllerDlg::SetActiveAdapter(DisplayControllerInterface* pControlAdapter,
                                                        DisplayAdapter* pAdapter)
{
    if ((nullptr != pActiveAdapter) && (pActiveAdapter->id == pAdapter->id))
        return;

    pActiveAdapter = pAdapter;

    ResetAdapterUiElements();

    PanelList = pControlAdapter->GetDisplayPanels(pActiveAdapter);
    if (PanelList.empty())
    {
        MessageBoxW(L"No active display found", 0, 0);
        return;
    }

    CComboBox* pPanelList = static_cast<CComboBox*>(GetDlgItem(IDC_DISPLAY_LIST));
    pPanelList->ResetContent();
    for (int i = 0; i < PanelList.size(); i++)
        pPanelList->InsertString(i, PanelList[i].Name.c_str());
    pPanelList->SetCurSel(0);
    pPanelList->ShowWindow(SW_SHOW);

    SetActivePanel(pControlAdapter, &PanelList[0]);
}

void CDisplayRefreshRateControllerDlg::InitUiElements(DisplayControllerInterface* pControlAdapter)
{
    AdapterList = pControlAdapter->GetDisplayAdapters();
    if (AdapterList.empty())
    {
        MessageBoxW(L"Application is not supported on this platform", 0, 0);
        OnClose();
        return;
    }

    CComboBox* pAdapterList = static_cast<CComboBox*>(GetDlgItem(IDC_ADAPTER_LIST));
    pAdapterList->ResetContent();
    for (int i = 0; i < AdapterList.size(); i++)
        pAdapterList->InsertString(i, AdapterList[i].Name.c_str());
    pAdapterList->SetCurSel(0);
    pAdapterList->ShowWindow(SW_SHOW);

    SetActiveAdapter(pControlAdapter, &AdapterList[0]);
}

BOOL CDisplayRefreshRateControllerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    IntelController = IntelDisplayControllerAdapter();
    pActiveController = &IntelController;

    InitUiElements(&IntelController);

    return TRUE;
}
