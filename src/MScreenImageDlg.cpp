#include "MScreenImageDlg.hpp"

MScreenImageDlg::MScreenImageDlg()
{
}

MScreenImageDlg::~MScreenImageDlg()
{
}

BOOL MScreenImageDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    CenterWindowDx(hwnd);

    m_resizable.OnParentCreate(hwnd, FALSE, FALSE);

    m_resizable.SetLayoutAnchor(stc1, mzcLA_TOP_LEFT, mzcLA_TOP_RIGHT);
    m_resizable.SetLayoutAnchor(stc2, mzcLA_TOP_LEFT, mzcLA_BOTTOM_CENTER);
    m_resizable.SetLayoutAnchor(psh1, MSize(50, 23), MSize(100, 46));
    m_resizable.SetLayoutAnchor(psh2, MSize(50, 46), MSize(100, 69));
    m_resizable.SetLayoutAnchor(IDCLOSE, MSize(50, 69), MSize(100, 100));

    MakeButtonOwnerDraw(IDOK);
    MakeButtonOwnerDraw(IDCANCEL);
    MakeButtonOwnerDraw(IDCLOSE);
    MakeButtonOwnerDraw(psh1);
    MakeButtonOwnerDraw(psh2);
    MakeButtonOwnerDraw(psh3);

    return TRUE;
}

void MScreenImageDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
    case IDCLOSE:
        PostMessage(GetParent(hwnd), WM_COMMAND, id, 0);
        DestroyWindow(hwnd);
        break;
    case psh1:
    case psh2:
    case psh3:
        PostMessage(GetParent(hwnd), WM_COMMAND, id, 0);
        break;
    }
}

void MScreenImageDlg::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    m_resizable.OnSize();
}

void MScreenImageDlg::MakeButtonOwnerDraw(UINT id)
{
    MakeButtonOwnerDraw(GetDlgItem(m_hwnd, id));
}

void MScreenImageDlg::MakeButtonOwnerDraw(HWND hwndButton)
{
    DWORD style = GetWindowLong(hwndButton, GWL_STYLE);
    style |= BS_OWNERDRAW;
    SetWindowLong(hwndButton, GWL_STYLE, style);
}

void MScreenImageDlg::OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem)
{
    if (lpDrawItem->CtlType != ODT_BUTTON)
        return;

    RECT rcItem = lpDrawItem->rcItem;
    HDC hDC = lpDrawItem->hDC;

    TCHAR szText[128];
    GetWindowText(lpDrawItem->hwndItem, szText, 128);

    RECT rcText = rcItem;
    DrawText(hDC, szText, lstrlen(szText), &rcText, DT_CENTER | DT_CALCRECT);

    SIZE sizText;
    sizText.cx = rcText.right - rcText.left;
    sizText.cy = rcText.bottom - rcText.top;

    INT x = (rcItem.left + rcItem.right - sizText.cx) / 2;
    INT y = (rcItem.top + rcItem.bottom - sizText.cy) / 2;

    if (lpDrawItem->itemState & ODS_DISABLED)
    {
        SelectObject(hDC, GetStockObject(LTGRAY_BRUSH));
        SelectObject(hDC, GetStockObject(BLACK_PEN));
        Rectangle(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
        GrayString(hDC, NULL, NULL, (LPARAM)szText, lstrlen(szText),
            x, y, sizText.cx, sizText.cy);
    }
    else if (lpDrawItem->itemState & ODS_SELECTED)
    {
        DrawFrameControl(hDC, &rcItem, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_ADJUSTRECT | DFCS_PUSHED);
        HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 192));
        HGDIOBJ hbrOld = SelectObject(hDC, hbr);
        {
            SelectObject(hDC, hbr);
            SelectObject(hDC, GetStockObject(BLACK_PEN));
            Rectangle(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
            rcItem.left = x;
            rcItem.top = y;
            rcItem.right = x + sizText.cx;
            rcItem.bottom = y + sizText.cy;
            OffsetRect(&rcItem, 1, 1);
            DrawText(hDC, szText, lstrlen(szText), &rcItem, DT_CENTER);
        }
        SelectObject(hDC, hbrOld);
        DeleteObject(hbr);
    }
    else
    {
        DrawFrameControl(hDC, &rcItem, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_ADJUSTRECT);
        SelectObject(hDC, GetStockObject(WHITE_BRUSH));
        SelectObject(hDC, GetStockObject(BLACK_PEN));
        Rectangle(hDC, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
        rcItem.left = x;
        rcItem.top = y;
        rcItem.right = x + sizText.cx;
        rcItem.bottom = y + sizText.cy;
        DrawText(hDC, szText, lstrlen(szText), &rcItem, DT_CENTER);
    }

    if (lpDrawItem->itemState & ODS_FOCUS)
    {
        rcItem = lpDrawItem->rcItem;
        InflateRect(&rcItem, -10, -10);
        DrawFocusRect(hDC, &rcItem);
    }
}
