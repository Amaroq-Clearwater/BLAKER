#include "QRCode.hpp"

size_t qr_next_bytes(size_t bytes)
{
    if (bytes > 2000)
        return bytes - 116 / 2;
    if (bytes > 1000)
        return bytes - 74 / 2;
    if (bytes > 200)
        return bytes - 38 / 2;
    if (bytes > 100)
        return bytes - 28 / 2;
    if (bytes > 17)
        return bytes - 15 / 2;
    return QR_MIN_BYTES;
}

int qr_width_from_bytes(size_t bytes)
{
    struct SIZE_INFO
    {
        size_t bytes;
        int width;
    };
    static const SIZE_INFO s_size_info[] =
    {
        { 17, 21 },
        { 32, 25 },
        { 53, 29 },
        { 78, 33 },
        { 106, 37 },
        { 134, 41 },
        { 154, 45 },
        { 192, 49 },
        { 230, 53 },
        { 271, 57 },
        { 321, 61 },
        { 367, 65 },
        { 425, 69 },
        { 458, 73 },
        { 520, 77 },
        { 586, 81 },
        { 644, 85 },
        { 718, 89 },
        { 792, 93 },
        { 858, 97 },
        { 929, 101 },
        { 1003, 105 },
        { 1091, 109 },
        { 1171, 113 },
        { 1273, 117 },
        { 1367, 121 },
        { 1465, 125 },
        { 1528, 129 },
        { 1628, 133 },
        { 1732, 137 },
        { 1840, 141 },
        { 1952, 145 },
        { 2068, 149 },
        { 2188, 153 },
        { 2303, 157 },
        { 2431, 161 },
        { 2563, 165 },
        { 2699, 169 },
        { 2809, 173 },
        { 2953, 177 }
    };
    const int count = ARRAYSIZE(s_size_info);
    for (int i = 0; i < count; ++i)
    {
        if (bytes < s_size_info[i].bytes)
            return s_size_info[i].width;
    }
    return 0;
}

size_t qr_best_bytes_from_space(float space, size_t bytes, float eDotSize)
{
    if (bytes > QR_MAX_BYTES)
        bytes = QR_MAX_BYTES;

    space -= QR_MARGIN * 2;
    if (space <= 0)
        return 0;

    for (ssize_t n = bytes; n >= 21; --n)
    {
        int width = qr_width_from_bytes(n);
        if (space > width * eDotSize)
            return n;
    }

    return 0;
}

QRcode* qr_code_create(const void *data, size_t size)
{
    const unsigned char *src = (const unsigned char *)data;
    return QRcode_encodeData(size, src, 0, QR_ECLEVEL_L);
}

HBITMAP qr_create_mono_bitmap(int cx, int cy)
{
    struct BITMAPINFOMONO
    {
        BITMAPINFOHEADER bmiHeader;
        RGBQUAD bmiColors[2];
    } bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = -cy;   // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 1;
    bmi.bmiColors[0].rgbBlue = 0;
    bmi.bmiColors[0].rgbGreen = 0;
    bmi.bmiColors[0].rgbRed = 0;
    bmi.bmiColors[0].rgbReserved = 0;
    bmi.bmiColors[1].rgbBlue = 255;
    bmi.bmiColors[1].rgbGreen = 255;
    bmi.bmiColors[1].rgbRed = 255;
    bmi.bmiColors[1].rgbReserved = 0;
    LPVOID pvBits;
    return CreateDIBSection(NULL, (BITMAPINFO *)&bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
}

HBITMAP qr_create_bitmap(HWND hwnd, const void *data, size_t size, INT& cx, INT& cy)
{
    QRcode* qr = qr_code_create(data, size);
    if (!qr)
    {
        return NULL;
    }

    cx = cy = qr->width;
    HBITMAP hbm = NULL;
    if (HDC hDC = CreateCompatibleDC(NULL))
    {
        hbm = qr_create_mono_bitmap(qr->width, qr->width);
        HGDIOBJ hbmOld = SelectObject(hDC, hbm);
        {
            for (int y = 0; y < qr->width; ++y)
            {
                for (int x = 0; x < qr->width; ++x)
                {
                    if (qr->data[y * qr->width + x] & 1)
                        SetPixelV(hDC, x, y, RGB(0, 0, 0));
                    else
                        SetPixelV(hDC, x, y, RGB(255, 255, 255));
                }
            }
        }
        SelectObject(hDC, hbmOld);
        DeleteDC(hDC);
    }

    QRcode_free(qr);
    return hbm;
}
