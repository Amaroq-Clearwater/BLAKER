#ifndef MCHOOSE_OPTION_DLG_HPP_
#define MCHOOSE_OPTION_DLG_HPP_

#include "MWindowBase.hpp"

class MChooseOptionDlg : public MDialogBase
{
public:
    MChooseOptionDlg();
    virtual ~MChooseOptionDlg();

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

#endif  // ndef MCHOOSE_OPTION_DLG_HPP_
