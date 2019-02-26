#ifndef MSTART_SCREEN_DLG_HPP_
#define MSTART_SCREEN_DLG_HPP_

#include "MWindowBase.hpp"

class MStartScreenDlg : public MDialogBase
{
public:
    MStartScreenDlg();
    virtual ~MStartScreenDlg();

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
    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
};

#endif  // ndef MSTART_SCREEN_DLG_HPP_
