// QRCode.hpp --- QR code manipulator
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#ifndef QRCODE_HPP_
#define QRCODE_HPP_

#ifndef _INC_WINDOWS
    #include "MWindowBase.hpp"
#endif

#include <qrencode.h>
#include <zbar.h>
#include <opencv2/opencv.hpp>

#define QR_MAX_BYTES    2953
#define QR_MIN_BYTES    17
#define QR_MARGIN       0.2     // 0.2 inch

size_t qr_next_bytes(size_t bytes);
int qr_width_from_bytes(size_t bytes);

size_t qr_best_bytes_from_space(float space, size_t bytes, float eDotSize);

QRcode* qr_code_create(const void *data, size_t size);

template <typename T_CALLBACK>
inline bool qr_scan_image(cv::Mat& img, T_CALLBACK& callback, LPARAM lParam = 0)
{
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    int width = img.cols;
    int height = img.rows;
    void *raw = img.data;

    zbar::Image image(width, height, "Y800", raw, width * height);

    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    int n = scanner.scan(image);

    for (zbar::Image::SymbolIterator symbol = image.symbol_begin();
         symbol != image.symbol_end();
         ++symbol)
    {
        // bool callback(zbar::Image::SymbolIterator& symbol, LPARAM lParam);
        if (!callback(symbol, lParam))
            return false;
    }

    return n > 0;
}

#ifdef _WIN32
    template <typename T_CALLBACK>
    inline bool qr_scan_bitmap(HBITMAP hbm, T_CALLBACK& callback, LPARAM lParam = 0)
    {
        // parameter hbm must be 32-bpp top-down DIB.
        BITMAP bm;
        if (!GetObject(hbm, sizeof(bm), &bm) || bm.bmBitsPixel != 32)
            return false;

        cv::Mat img(bm.bmHeight, bm.bmWidth, CV_8UC4, bm.bmBits);

        return qr_scan_image(img, callback, lParam);
    }

    HBITMAP qr_create_mono_bitmap(int cx, int cy);
    HBITMAP qr_create_bitmap(HWND hwnd, const void *data, size_t size, INT& cx, INT& cy);
#endif  // def _WIN32

#endif  // ndef QRCODE_HPP_
