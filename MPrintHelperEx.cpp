#include "stdafx.hpp"

MPrintHelperEx::MPrintHelperEx(MBlakerApp *app) : m_app(app)
{
}

MPrintHelperEx::~MPrintHelperEx()
{
}

BOOL MPrintHelperEx::DoPrintPages(HWND hwnd, HDC hDC, LPCWSTR pszDocName)
{
    return m_app->DoPrintPages(hwnd, hDC, pszDocName);
}
