#ifndef MSETTINGS_DLG_HPP_
#define MSETTINGS_DLG_HPP_

#include "MWindowBase.hpp"
#include "Settings.hpp"

class MSettingsDlg : public MDialogBase
{
public:
    MSettingsDlg(BLAKER_SETTINGS& settings);
    virtual ~MSettingsDlg();

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        default:
            return DefaultProcDx();
        }
    }

protected:
    BLAKER_SETTINGS& m_settings;

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnOK(HWND hwnd);
    void OnPsh1(HWND hwnd);
    void OnCmb1(HWND hwnd);
};

#endif  // ndef MSETTINGS_DLG_HPP_
