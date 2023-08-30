// DisplayRefreshRateControllerDlg.h : header file
//

#pragma once
#include <iostream>
#include <sstream>
#include "Intel/IntelDisplayControllerAdapter.h"


// CDisplayRefreshRateControllerDlg dialog
class CDisplayRefreshRateControllerDlg : public CDialogEx
{
    IntelDisplayControllerAdapter IntelController;
    DisplayControllerInterface* pActiveController;

    std::vector<DisplayAdapter> AdapterList;
    DisplayAdapter* pActiveAdapter;

    std::vector<DisplayPanel> PanelList;
    DisplayPanel* pActivePanel;

    std::vector<RrPreset> RrPresetList;
    RrPreset *pActivePreset, *pSelectedPreset;

protected:
    HICON m_hIcon;
    virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
    virtual BOOL OnInitDialog();

    void OnOK() override
    {
        CDialogEx::OnOK();
    }

    void OnCancel() override
    {
        CDialogEx::OnCancel();
    }

    afx_msg void OnClose();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

public:
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DISPLAYREFRESHRATECONTROLLER_DIALOG };
#endif

    CDisplayRefreshRateControllerDlg(CWnd* pParent = nullptr); // standard constructor

    virtual void InitUiElements(DisplayControllerInterface*);
    virtual void ResetAdapterUiElements();
    virtual void ResetPanelUiElements();
    virtual void SetActiveAdapter(DisplayControllerInterface*, DisplayAdapter*);
    virtual void SetActivePanel(DisplayControllerInterface*, DisplayPanel*);
    virtual void SetRrControl(int, int, int, float, std::wstring, float, float);

    afx_msg void OnCbnSelchangeDisplayList();
    afx_msg void OnCbnSelchangeAdapterList();
    afx_msg void OnCbnSelchangeRrPresetList();
    afx_msg void OnBnClickedButtonApply();
    afx_msg void OnNMCustomdrawSliderMinRr(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMCustomdrawSliderMaxRr(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMCustomdrawSliderRrIncLimit(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnNMCustomdrawSliderRrDecLimit(NMHDR* pNMHDR, LRESULT* pResult);
};
