#ifndef MSPECIAL_STATIC_HPP_
#define MSPECIAL_STATIC_HPP_

#include "MWindowBase.hpp"

class MSpecialStatic : public MWindowBase
{
public:
    MSpecialStatic();
    virtual ~MSpecialStatic();

    void SetBitmap(HBITMAP hbm);

    virtual LRESULT CALLBACK
    WindowProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_ERASEBKGND, OnEraseBkgnd);
        HANDLE_MSG(hwnd, WM_PAINT, OnPaint);
        default:
            return DefaultProcDx();
        }
    }

protected:
    HBITMAP m_hbm;

    BOOL OnEraseBkgnd(HWND hwnd, HDC hdc);
    void OnPaint(HWND hwnd);
};

#endif  // ndef MSPECIAL_STATIC_HPP_
