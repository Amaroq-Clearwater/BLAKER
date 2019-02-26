#ifndef RENDERER_HPP_
#define RENDERER_HPP_

#ifndef _INC_WINDOWS
    #include <windows.h>
#endif
#include <cassert>

#define CX_CHAR 24
#define CY_CHAR 36

struct Renderer
{
    HBITMAP m_hbmChars;  // bitmap font

    Renderer()
    {
        m_hbmChars = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(3),
            IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
        assert(m_hbmChars != NULL);
    }

    ~Renderer()
    {
        DeleteObject(m_hbmChars);
        m_hbmChars = NULL;
    }

    void drawChar(HDC hDC, BYTE ch, INT x, INT y, INT cx, INT cy)
    {
        if (HDC hdcMem = CreateCompatibleDC(NULL))
        {
            HGDIOBJ hbmOld = SelectObject(hdcMem, m_hbmChars);
            {
                StretchBlt(hDC, x, y, cx, cy,
                    hdcMem,
                    (ch % 16) * CX_CHAR, (ch >> 4) * CY_CHAR,
                    CX_CHAR, CY_CHAR, SRCCOPY);
            }
            SelectObject(hdcMem, hbmOld);
            DeleteDC(hdcMem);
        }
    }

    void drawString(HDC hDC, const std::string& str, INT x, INT y, INT cx, INT cy)
    {
        for (size_t i = 0; i < str.size(); ++i)
        {
            drawChar(hDC, str[i], x, y, cx, cy);
            x += cx;
        }
    }
};

#endif  // ndef RENDERER_HPP_
