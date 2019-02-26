#include "resource.h"
#include "MBlakerScreen.hpp"

#define ONE_SECOND 1000
#define QR_INTERVAL 540

#define TIMER_1 777
#define TIMER_2 888
#define TIMER_3 999

MBlakerScreen::MBlakerScreen(BOOL bMovie)
{
    m_hIcon = LoadIconDx(1);
    m_hIconSm = LoadSmallIconDx(1);
    m_hwndChildDlg = NULL;
    m_nID = 0;
    m_sizImage.cx = m_sizImage.cy = 0;
    m_iPage = 0;
    m_bOK = FALSE;
    m_bMovie = bMovie;
    m_ahbm[0] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(4));
    m_ahbm[1] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(5));
    m_ahbm[2] = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(6));
    m_hbmFinish = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(7));
}

MBlakerScreen::~MBlakerScreen()
{
    ::DeleteObject(m_ahbm[0]);
    ::DeleteObject(m_ahbm[1]);
    ::DeleteObject(m_ahbm[2]);
    ::DeleteObject(m_hbmFinish);
    ::DestroyIcon(m_hIcon);
    ::DestroyIcon(m_hIconSm);
}

BOOL MBlakerScreen::SetControlDialog(HWND hwnd, MDialogBase& dialog, INT nID)
{
    if (m_hwndChildDlg)
    {
        ::DestroyWindow(m_hwndChildDlg);
        m_hwndChildDlg = NULL;
    }

    if (dialog.CreateDialogDx(hwnd, nID))
    {
        ::ShowWindow(dialog, SW_SHOWMAXIMIZED);
        ::UpdateWindow(dialog);

        m_hwndChildDlg = dialog;
        m_nID = nID;
        return TRUE;
    }
    return FALSE;
}

BOOL MBlakerScreen::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    CenterWindowDx(hwnd);
    ::SetCursor(::LoadCursor(NULL, IDC_WAIT));

    // set icons
    ::SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)m_hIcon);
    ::SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIconSm);

    // don't move
    HMENU hSysMenu = ::GetSystemMenu(hwnd, FALSE);
    ::RemoveMenu(hSysMenu, SC_MOVE, MF_BYCOMMAND);

    // fit to the work area
    RECT rcWork;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
    MoveWindow(hwnd, rcWork.left, rcWork.top,
        rcWork.right - rcWork.left, rcWork.bottom - rcWork.top, TRUE);

    if (m_bMovie)
        SetControlDialog(hwnd, m_screen_image_dialog, IDD_SCREEN_MOVIE);
    else
        SetControlDialog(hwnd, m_screen_image_dialog, IDD_SCREEN_IMAGE);

    HWND hStc2 = GetDlgItem(m_screen_image_dialog, stc2);
    assert(hStc2);

    RECT rc;
    GetClientRect(hStc2, &rc);

    POINT pt;
    pt.x = (rc.left + rc.right) / 2;
    pt.y = (rc.top + rc.bottom) / 2;

    SIZE siz;
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    m_sizImage = siz;

    return TRUE;
}

void MBlakerScreen::DoUpdateUI(HWND hwnd)
{
    if (m_bMovie)
        return;

    if (m_iPage == (UINT)m_bitmaps.size())
    {
        EnableWindow(GetDlgItem(m_screen_image_dialog, psh1), FALSE);
    }
    else
    {
        EnableWindow(GetDlgItem(m_screen_image_dialog, psh1), TRUE);
    }

    if (m_iPage <= 1)
    {
        EnableWindow(GetDlgItem(m_screen_image_dialog, psh2), FALSE);
    }
    else
    {
        EnableWindow(GetDlgItem(m_screen_image_dialog, psh2), TRUE);
    }
}

void MBlakerScreen::OnNextImage(HWND hwnd)
{
    if (m_iPage == (UINT)m_bitmaps.size())
    {
        return;
    }

    m_iPage++;
    m_stc2.SetBitmap(m_bitmaps[m_iPage - 1]);

    WCHAR szText[64];
    StringCbPrintfW(szText, sizeof(szText), L"%u / %u", m_iPage, (UINT)m_bitmaps.size());
    SetDlgItemTextW(m_screen_image_dialog, stc3, szText);

    DoUpdateUI(hwnd);
}

void MBlakerScreen::OnGoBack(HWND hwnd)
{
    if (m_iPage == 0)
        return;

    m_iPage--;
    m_stc2.SetBitmap(m_bitmaps[m_iPage - 1]);

    WCHAR szText[64];
    StringCbPrintfW(szText, sizeof(szText), L"%u / %u", m_iPage, (UINT)m_bitmaps.size());
    SetDlgItemTextW(m_screen_image_dialog, stc3, szText);

    DoUpdateUI(hwnd);
}

void MBlakerScreen::OnCountDown(HWND hwnd)
{
    m_stc2.SetBitmap(m_ahbm[--m_count]);

    SetDlgItemTextW(m_screen_image_dialog, stc3, NULL);

    DoUpdateUI(hwnd);

    if (m_count <= 0)
    {
        m_iPage = 0;
        ::KillTimer(hwnd, TIMER_2);
        ::SetTimer(hwnd, TIMER_1, QR_INTERVAL, NULL);
    }
}

void MBlakerScreen::OnTimer(HWND hwnd, UINT id)
{
    switch (id)
    {
    case TIMER_1:
        OnNextImage(hwnd);
        if (m_iPage == (UINT)m_bitmaps.size())
        {
            ::KillTimer(hwnd, TIMER_1);
            if (m_bMovie)
                ::SetTimer(hwnd, TIMER_3, QR_INTERVAL, NULL);
        }
        break;
    case TIMER_2:
        OnCountDown(hwnd);
        break;
    case TIMER_3:
        m_stc2.SetBitmap(m_hbmFinish);
        SetDlgItemTextW(m_screen_image_dialog, stc3, NULL);
        ::KillTimer(hwnd, TIMER_3);
        break;
    }
}

void MBlakerScreen::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
    case IDCLOSE:
        ::DestroyWindow(hwnd);
        break;
    case psh1:
        switch (m_nID)
        {
        case IDD_SCREEN_IMAGE:
            OnNextImage(hwnd);
            break;
        case IDD_SCREEN_MOVIE:
            ::KillTimer(hwnd, TIMER_1);
            ::KillTimer(hwnd, TIMER_2);
            m_count = 3;
            OnTimer(hwnd, TIMER_2);
            ::SetTimer(hwnd, TIMER_2, ONE_SECOND, NULL);
            break;
        }
        break;
    case psh2:
        if (m_nID == IDD_SCREEN_IMAGE)
        {
            OnGoBack(hwnd);
        }
        break;
    case IDYES:
        {
            MStartScreenDlg dialog;
            dialog.DialogBoxDx(hwnd);

            assert(IsWindow(m_screen_image_dialog));
            m_screen_image_dialog.SubclassChildDx(m_stc2, stc2);

            ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
            m_count = 3;
            OnTimer(hwnd, TIMER_2);
            ::SetTimer(hwnd, TIMER_2, ONE_SECOND, NULL);
        }
        break;
    case IDNO:
        {
            MStartScreenDlg dialog;
            dialog.DialogBoxDx(hwnd);

            assert(IsWindow(m_screen_image_dialog));
            m_screen_image_dialog.SubclassChildDx(m_stc2, stc2);

            PostMessage(hwnd, WM_COMMAND, psh1, 0);
            ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
        }
        break;
    }
}

void MBlakerScreen::OnDestroy(HWND hwnd)
{
    if (m_bOK)
        ::PostQuitMessage(0);
}

BOOL MBlakerScreen::SetBitmaps(std::vector<HBITMAP>& bitmaps)
{
    m_bitmaps = bitmaps;
    return TRUE;
}
