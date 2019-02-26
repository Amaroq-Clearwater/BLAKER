#include "MBlakerScreen.hpp"

MSpecialStatic::MSpecialStatic() : m_hbm(NULL)
{
}

MSpecialStatic::~MSpecialStatic()
{
    DeleteObject(m_hbm);
}

void MSpecialStatic::SetBitmap(HBITMAP hbm)
{
    if (m_hbm)
    {
        DeleteObject(m_hbm);
        m_hbm = NULL;
    }
    m_hbm = (HBITMAP)CopyImage(hbm, IMAGE_BITMAP, 0, 0,
                               LR_COPYRETURNORG);
    InvalidateRect(m_hwnd, NULL, TRUE);
}

BOOL MSpecialStatic::OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    return TRUE;
}

void MSpecialStatic::OnPaint(HWND hwnd)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    POINT pt;
    pt.x = (rc.left + rc.right) / 2;
    pt.y = (rc.top + rc.bottom) / 2;

    SIZE siz;
    siz.cx = rc.right - rc.left;
    siz.cy = rc.bottom - rc.top;

    INT nSize = std::min(siz.cx, siz.cy) - SCREEN_MARGIN;

    BITMAP bm;
    GetObject(m_hbm, sizeof(bm), &bm);

    PAINTSTRUCT ps;
    if (HDC hDC = BeginPaint(hwnd, &ps))
    {
        SetStretchBltMode(hDC, STRETCH_DELETESCANS);

        SelectObject(hDC, (HBRUSH)GetStockObject(WHITE_BRUSH));
        SelectObject(hDC, (HBRUSH)GetStockObject(BLACK_PEN));
        Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);

        if (HDC hdcMem = CreateCompatibleDC(NULL))
        {
            HGDIOBJ hbmOld = SelectObject(hdcMem, m_hbm);
            {
                StretchBlt(hDC, pt.x - nSize / 2, pt.y - nSize / 2, nSize, nSize,
                           hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
            }
            SelectObject(hdcMem, hbmOld);
            DeleteDC(hdcMem);
        }

        EndPaint(hwnd, &ps);
    }
}
