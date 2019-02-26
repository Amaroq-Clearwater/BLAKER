#ifndef MSCAN_IMAGES_DLG_HPP_
#define MSCAN_IMAGES_DLG_HPP_

#include "MWindowBase.hpp"
#include <vector>

class MScanImagesDlg : public MDialogBase
{
public:
    std::vector<std::wstring> m_files;

    MScanImagesDlg();
    virtual ~MScanImagesDlg();

    BOOL IsImageFile(LPCWSTR pszFile);

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_DROPFILES, OnDropFiles);
        default:
            return DefaultProcDx();
        }
    }

protected:
    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnOK(HWND hwnd);
    void OnPsh1(HWND hwnd);
    void OnPsh2(HWND hwnd);
    void OnPsh3(HWND hwnd);
    void OnPsh4(HWND hwnd);
    void OnDblClk(HWND hwnd);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnDropFiles(HWND hwnd, HDROP hdrop);
};

#endif  // ndef MSCAN_IMAGES_DLG_HPP_
