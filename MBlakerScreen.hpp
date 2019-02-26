#ifndef MBLAKERSCREEN_HPP_
#define MBLAKERSCREEN_HPP_

#include "MStartScreenDlg.hpp"
#include "MScreenImageDlg.hpp"
#include "MSpecialStatic.hpp"
#include <vector>

class MBlakerScreen : public MDialogBase
{
public:
    HWND m_hwndChildDlg;
    INT m_nID;
    SIZE m_sizImage;
    BOOL m_bOK;
    BOOL m_bMovie;

    MBlakerScreen(BOOL bMovie);
    virtual ~MBlakerScreen();

    BOOL SetControlDialog(HWND hwnd, MDialogBase& dialog, INT nID);
    BOOL SetBitmaps(std::vector<HBITMAP>& bitmaps);

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hwnd, WM_TIMER, OnTimer);
        default:
            return DefaultProcDx();
        }
    }

    void DoUpdateUI(HWND hwnd);

protected:
    HICON m_hIcon;
    HICON m_hIconSm;
    MSpecialStatic m_stc2;
    MScreenImageDlg m_screen_image_dialog;
    INT m_iPage;
    std::vector<HBITMAP> m_bitmaps;
    HBITMAP m_ahbm[3];
    INT m_count;
    HBITMAP m_hbmFinish;

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnDestroy(HWND hwnd);
    void OnNextImage(HWND hwnd);
    void OnGoBack(HWND hwnd);
    void OnCountDown(HWND hwnd);
    void OnTimer(HWND hwnd, UINT id);
};

#endif  // ndef MBLAKERSCREEN_HPP_
