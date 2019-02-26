#ifndef MSCREEN_IMAGE_DLG_HPP_
#define MSCREEN_IMAGE_DLG_HPP_

#include "MWindowBase.hpp"
#include "MResizable.hpp"

#define SCREEN_MARGIN 32

class MScreenImageDlg : public MDialogBase
{
public:
    MScreenImageDlg();
    virtual ~MScreenImageDlg();

    virtual INT_PTR CALLBACK
    DialogProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_SIZE, OnSize);
        HANDLE_MSG(hwnd, WM_DRAWITEM, OnDrawItem);
        default:
            return DefaultProcDx();
        }
    }

    void MakeButtonOwnerDraw(HWND hwndButton);
    void MakeButtonOwnerDraw(UINT id);

protected:
    MResizable m_resizable;

    BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnSize(HWND hwnd, UINT state, int cx, int cy);
    void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);
};

#endif  // ndef MSCREEN_IMAGE_DLG_HPP_
