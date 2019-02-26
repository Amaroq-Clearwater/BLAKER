#ifndef MSCANNING_MOVIE_DLG_HPP_
#define MSCANNING_MOVIE_DLG_HPP_

#include "MBlakerApp.hpp"

class MScanningMovieDlg : public MDialogBase
{
public:
    HANDLE m_hThread;
    PROGRESS_INFO *m_info;

    MScanningMovieDlg(MBlakerApp *app, LPCWSTR pszFile);
    virtual ~MScanningMovieDlg();

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
        default:
            return DefaultProcDx();
        }
    }

    BOOL JustDoIt(HWND hwnd);

protected:
    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnTimer(HWND hwnd, UINT id);
};

#endif  // ndef MSCANNING_MOVIE_DLG_HPP_
