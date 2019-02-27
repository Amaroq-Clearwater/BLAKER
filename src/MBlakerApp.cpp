#include "stdafx.hpp"
#include <vfw.h>
#include "MAboutDlg.hpp"
#include "MSettingsDlg.hpp"

// make 32-bpp top-down DIB
HBITMAP Create32BppBitmap(HDC hDC, INT cx, INT cy, VOID **ppvBits = NULL)
{
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = -cy;   // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    LPVOID pvBits;
    if (!ppvBits)
        ppvBits = &pvBits;
    return CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
}

MBlakerApp::MBlakerApp(HINSTANCE hInst)
    : m_hInst(hInst), m_hIcon(NULL), m_hIconSm(NULL), m_hAccel(NULL),
      m_helper(this)
{
    m_nCommandExecCount = 0;
    m_hIcon = LoadIconDx(1);
    m_hIconSm = LoadSmallIconDx(1);

    m_hMainWnd = NULL;
    m_hListView = NULL;
    m_hStatusWnd = NULL;
    m_hImageList = NULL;
    m_method = METHOD_QRCODE_META;
    m_cParts = 0;
    m_cPages = 0;
    m_bMeTa = FALSE;
}

MBlakerApp::~MBlakerApp()
{
}

LPCTSTR MBlakerApp::GetWndClassNameDx() const
{
    return TEXT("MZC4 MBlakerApp Class");
}

VOID MBlakerApp::ModifyWndClassDx(WNDCLASSEX& wcx)
{
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;

    wcx.lpszMenuName = MAKEINTRESOURCE(1);
    wcx.hIcon = m_hIcon;
    wcx.hIconSm = m_hIconSm;
}

BOOL MBlakerApp::OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    m_hMainWnd = hwnd;
    m_helper.InitPageSetup(hwnd);

    DWORD style, exstyle;

    style = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS |
        LVS_AUTOARRANGE | LVS_SHOWSELALWAYS |
        WS_BORDER | WS_VSCROLL | WS_HSCROLL;
    exstyle = WS_EX_CLIENTEDGE;
    m_hListView = CreateWindowExW(exstyle, WC_LISTVIEW, NULL, style,
        0, 0, 0, 0, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
    if (!m_hListView || !AddListViewColumns() || !ReCreateImageList())
        return FALSE;

    style = WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP;
    m_hStatusWnd = CreateStatusWindow(style, L"", hwnd, 999);
    if (!m_hStatusWnd)
        return FALSE;

    SendMessage(m_hListView, WM_SETFONT,
        (WPARAM)GetStockObject(DEFAULT_GUI_FONT), TRUE);

    m_hAccel = LoadAccelerators(m_hInst, MAKEINTRESOURCE(1));

    PostMessage(hwnd, WM_SIZE, 0, 0);
    PostMessage(hwnd, WM_COMMAND, ID_READY, 0);

    DragAcceptFiles(hwnd, TRUE);

    INT argc;
    LPWSTR *wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc >= 2)
        DoLoad(hwnd, wargv[1]);
    GlobalFree(wargv);

    DoLoadSettings(m_settings);

    return TRUE;
}

void MBlakerApp::OnDestroy(HWND hwnd)
{
    DoSaveSettings(m_settings);

    for (size_t i = 0; i < m_temp_dirs.size(); ++i)
    {
        const std::wstring& dir = m_temp_dirs[i];
        if (dir.empty() || dir == L"." || dir == L".." ||
            dir == L"c:" || dir == L"C:" || dir == L"c:\\" || dir == L"C:\\" ||
            dir == L"c:/" || dir == L"C:/")
        {
            continue;
        }
        DeleteDirectoryW(dir.c_str());
    }

    ::PostQuitMessage(0);
}

BOOL MBlakerApp::StartDx(INT nCmdShow)
{
    m_hIcon = LoadIconDx(1);
    m_hIconSm = LoadSmallIconDx(1);
    m_hAccel = ::LoadAccelerators(m_hInst, MAKEINTRESOURCE(1));

    if (!CreateWindowDx(NULL, LoadStringDx(IDS_APPNAME),
                        WS_OVERLAPPEDWINDOW, 0,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        600, 200))
    {
        MessageBoxA(NULL, "CreateWindow failed", "BLAKER", MB_ICONERROR);
        return FALSE;
    }

    ::ShowWindow(*this, nCmdShow);
    ::UpdateWindow(*this);

    return TRUE;
}

INT MBlakerApp::RunDx()
{
    // message loop
    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        if (!::TranslateAccelerator(m_hwnd, m_hAccel, &msg))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }
    return INT(msg.wParam);
}

void MBlakerApp::EnterCommand(HWND hwnd)
{
    if (m_nCommandExecCount++ == 0)
    {
        SetCursor(LoadCursor(NULL, IDC_WAIT));
        SendMessage(m_hStatusWnd, SB_SETTEXT, 0, (LPARAM)LoadStringDx(IDS_EXECUTING_CMD));
    }
}

void MBlakerApp::LeaveCommand(HWND hwnd)
{
    if (--m_nCommandExecCount == 0)
    {
        SendMessage(m_hStatusWnd, SB_SETTEXT, 0, (LPARAM)LoadStringDx(IDS_READY));
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
}

BOOL MBlakerApp::ReCreateImageList()
{
    if (m_hImageList)
    {
        ImageList_Destroy(m_hImageList);
        m_hImageList = NULL;
    }

    m_hImageList = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 10, 10);
    ListView_SetImageList(m_hListView, m_hImageList, LVSIL_SMALL);

    return m_hImageList != NULL;
}

BOOL MBlakerApp::AddListViewColumns()
{
    LV_COLUMN column;
    ZeroMemory(&column, sizeof(column));
    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
    column.fmt = LVCFMT_LEFT;
    column.pszText = LoadStringDx(IDS_FILENAME);
    column.cx = 220;
    column.iSubItem = 0;
    ListView_InsertColumn(m_hListView, 0, &column);

    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
    column.fmt = LVCFMT_RIGHT;
    column.pszText = LoadStringDx(IDS_FILE_SIZE);
    column.cx = 100;
    column.iSubItem = 1;
    ListView_InsertColumn(m_hListView, 1, &column);

    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
    column.fmt = LVCFMT_LEFT;
    column.pszText = LoadStringDx(IDS_FILE_TYPE);
    column.cx = 140;
    column.iSubItem = 2;
    ListView_InsertColumn(m_hListView, 2, &column);

    column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_FMT;
    column.fmt = LVCFMT_LEFT;
    column.pszText = LoadStringDx(IDS_CRC32);
    column.cx = 110;
    column.iSubItem = 3;
    ListView_InsertColumn(m_hListView, 3, &column);

    return TRUE;
}

INT MBlakerApp::AddFileItem(HWND hwnd, const FILE_ITEM& file_item)
{
    assert(m_hImageList);

    SHFILEINFO shfi;
    ZeroMemory(&shfi, sizeof(shfi));
    SHGetFileInfo(file_item.filename.c_str(), 0, &shfi, sizeof(shfi),
                  SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES |
                  SHGFI_TYPENAME);
    assert(shfi.hIcon);

    INT iIcon = ImageList_AddIcon(m_hImageList, shfi.hIcon);
    assert(iIcon != -1);

    TCHAR szText[128];
    StringCbCopy(szText, sizeof(szText), file_item.filename.c_str());

    INT cItems = ListView_GetItemCount(m_hListView);

    LV_ITEM item;
    ZeroMemory(&item, sizeof(item));
    item.mask = LVIF_TEXT | LVIF_IMAGE;
    item.pszText = szText;
    item.iItem = cItems;
    item.iImage = iIcon;
    INT iItem = ListView_InsertItem(m_hListView, &item);
    assert(iItem != -1);

    ::DestroyIcon(shfi.hIcon);

    StrFormatByteSizeW(file_item.binary.size(), szText, ARRAYSIZE(szText));
    ZeroMemory(&item, sizeof(item));
    item.mask = LVIF_TEXT;
    item.iItem = iItem;
    item.iSubItem = 1;
    item.pszText = szText;
    ListView_SetItem(m_hListView, &item);

    ZeroMemory(&item, sizeof(item));
    item.mask = LVIF_TEXT;
    item.iItem = iItem;
    item.iSubItem = 2;
    item.pszText = shfi.szTypeName;
    ListView_SetItem(m_hListView, &item);

    DWORD crc = crc32(file_item.binary.data(), file_item.binary.size());
    StringCbPrintf(szText, sizeof(szText), L"%08lX", crc);
    ZeroMemory(&item, sizeof(item));
    item.mask = LVIF_TEXT;
    item.iItem = iItem;
    item.iSubItem = 3;
    item.pszText = szText;
    ListView_SetItem(m_hListView, &item);

    if (iItem != -1)
    {
        m_bins.push_back(file_item);
    }

    return iItem;
}

BOOL MBlakerApp::DoLoad(HWND hwnd, const wchar_t *pszFile)
{
    BOOL bDidOpen = FALSE;
    std::string binary;
    try
    {
        {
            fp_wrap fp;
            char buf[512];
            if (fp.open(pszFile, L"rb"))
            {
                for (;;)
                {
                    size_t size = fp.read(buf, 1, 512);
                    if (!size)
                        break;
                    binary.insert(binary.end(), &buf[0], &buf[size]);
                }
                bDidOpen = TRUE;
            }
        }
    }
    catch (...)
    {
        ;
    }

    if (!bDidOpen)
    {
        ErrorBoxDx(IDS_CANTOPENFILE);
        return FALSE;
    }

    LPCTSTR pch = PathFindFileName(pszFile);

    FILE_ITEM file_item;
    file_item.filename = pch;
    file_item.binary = binary;
    return AddFileItem(hwnd, file_item) != -1;
}

BOOL MBlakerApp::DoSave(HWND hwnd, const wchar_t *pszFile,
                              const std::string& binary)
{
    BOOL bSaved = FALSE;
    try
    {
        {
            fp_wrap fp;
            if (fp.open(pszFile, L"wb"))
            {
                if (fp.write(binary.data(), binary.size(), 1) == 1)
                {
                    bSaved = TRUE;
                }
                else
                {
                    fp.close();
                    DeleteFileW(pszFile);
                }
            }
        }
    }
    catch (...)
    {
        ;
    }

    if (!bSaved)
    {
        ErrorBoxDx(IDS_CANTSAVEFILE);
        return FALSE;
    }

    return TRUE;
}

BOOL MBlakerApp::DoLoadPdfium(HWND hwnd, katahiromz_pdfium& pdfium)
{
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    LPTSTR pch = PathFindFileName(szPath);
    *pch = 0;
#ifdef _WIN64
    StringCbCatW(szPath, sizeof(szPath), L"katahiromz_pdfium/x64/pdfium.dll");
#else
    StringCbCatW(szPath, sizeof(szPath), L"katahiromz_pdfium/x86/pdfium.dll");
#endif
    if (!pdfium.load(szPath))
    {
        *pch = 0;
#ifdef _WIN64
        StringCbCatW(szPath, sizeof(szPath), L"../katahiromz_pdfium/x64/pdfium.dll");
#else
        StringCbCatW(szPath, sizeof(szPath), L"../katahiromz_pdfium/x86/pdfium.dll");
#endif
        pdfium.load(szPath);
    }
    if (!pdfium.is_loaded())
    {
        *pch = 0;
#ifdef _WIN64
        StringCbCatW(szPath, sizeof(szPath), L"../../katahiromz_pdfium/x64/pdfium.dll");
#else
        StringCbCatW(szPath, sizeof(szPath), L"../../katahiromz_pdfium/x86/pdfium.dll");
#endif
        pdfium.load(szPath);
    }
    if (!pdfium.is_loaded())
    {
        pdfium.load(TEXT("pdfium.dll"));
    }
    if (!pdfium.is_loaded())
    {
        ErrorBoxDx(IDS_CANTLOADPDFLIB);
        return FALSE;
    }
    return TRUE;
}

MBlakerApp::QR_CALLBACK_DATA::QR_CALLBACK_DATA(
    std::vector<std::string>& binaries, int& iPage, BOOL& bMeTa)
    : m_binaries(binaries), m_iPage(iPage), m_error(VALID), m_bMeTa(bMeTa)
{
}

bool MBlakerApp::QR_CALLBACK_DATA::operator()(zbar::Image::SymbolIterator& symbol, LPARAM lParam)
{
    if (symbol->get_type() != zbar::ZBAR_QRCODE)
        return true;

    std::string bin = symbol->get_data();
    INT iPart = 0, cParts = 0;
    std::string hash;
    std::vector<std::string> items;
    if (memcmp(&bin[0], "MeTa>", 5) == 0)
    {
        // MeTa>XX>YY>...|
        size_t iBar = bin.find('|');
        if (iBar != std::string::npos)
        {
            std::string substr = bin.substr(5, iBar - 5);
            mstr_split(items, substr, ">");
            if (items.size() == 3)
            {
                iPart = std::strtoul(items[0].c_str(), NULL, 10);
                cParts = std::strtoul(items[1].c_str(), NULL, 10);
                hash = items[2];
                bin.erase(0, iBar + 1);
                if (m_hash.size() && hash != m_hash)
                {
                    m_error = INVALID_HASH;
                    return false;
                }
            }
        }
    }
    if (iPart > 0 && cParts > 0)
    {
        m_hash = hash;
        --iPart;
        if (m_binaries.size() <= iPart)
            m_binaries.resize(iPart + 1);
        if (m_binaries.size() < cParts)
            m_binaries.resize(cParts);
        m_binaries[iPart] = bin;
        m_bMeTa = TRUE;
    }
    else
    {
        if (m_bMeTa)
        {
            m_error = INVALID_FORMAT;
            return false;
        }
        if (m_binaries.size() <= m_iPage)
            m_binaries.resize(m_iPage + 1);
        m_binaries[m_iPage] = bin;
    }
    return true;
}

void MBlakerApp::scanError(HWND hwnd, QR_CALLBACK_DATA::ErrorType error)
{
    switch (error)
    {
    case QR_CALLBACK_DATA::VALID:
        break;
    case QR_CALLBACK_DATA::INVALID_HASH:
        ErrorBoxDx(IDS_DATA_MINGLED);
        break;
    case QR_CALLBACK_DATA::INVALID_FORMAT:
        ErrorBoxDx(IDS_INVALID_DATA);
        break;
    case QR_CALLBACK_DATA::INCOMPLETE_DATA:
        ErrorBoxDx(IDS_INCOMPLETE_DATA);
        break;
    }
}

BOOL MBlakerApp::DoScanPDF(HWND hwnd, LPCWSTR pszPdfFile)
{
    katahiromz_pdfium pdfium;
    if (!DoLoadPdfium(hwnd, pdfium))
        return FALSE;

    char pdf_file[MAX_PATH] = "";
    WideCharToMultiByte(CP_ACP, 0, pszPdfFile, -1, pdf_file, MAX_PATH, NULL, NULL);

    int page_count = 0;
    std::vector<std::string> binaries;
    m_bMeTa = FALSE;
    QR_CALLBACK_DATA::ErrorType error = QR_CALLBACK_DATA::VALID;

    std::string hash;
    try
    {
        if (FPDF_DOCUMENT pdf_doc = pdfium.FPDF_LoadDocument(pdf_file, NULL))
        {
            page_count = pdfium.FPDF_GetPageCount(pdf_doc);

            if (HDC hDC = CreateCompatibleDC(NULL))
            {
                for (int iPage = 0; iPage < page_count; ++iPage)
                {
                    if (FPDF_PAGE pdf_page = pdfium.FPDF_LoadPage(pdf_doc, iPage))
                    {
                        // get the page size
                        double page_width = pdfium.FPDF_GetPageWidth(pdf_page);
                        double page_height = pdfium.FPDF_GetPageHeight(pdf_page);

                        // page size in pixels
                        SIZE sizPixels;
                        INT logpixelsx = SCAN_DPI;
                        INT logpixelsy = SCAN_DPI;
                        sizPixels.cx = long(page_width * logpixelsx / 72);
                        sizPixels.cy = long(page_height * logpixelsy / 72);

                        LPVOID pvBits;
                        if (HBITMAP hbm = Create32BppBitmap(hDC, sizPixels.cx, sizPixels.cy))
                        {
                            // render page to bitmap
                            HGDIOBJ hbmOld = SelectObject(hDC, hbm);
                            {
                                RECT rc = {0, 0, sizPixels.cx, sizPixels.cy};
                                FillRect(hDC, &rc, (HBRUSH)(GetStockObject(WHITE_BRUSH)));
                                pdfium.FPDF_RenderPage(
                                    hDC, pdf_page,
                                    0, 0, sizPixels.cx, sizPixels.cy,
                                    0, FPDF_ANNOT | FPDF_PRINTING);
                            }
                            SelectObject(hDC, hbmOld);

                            // scan bitmap
                            QR_CALLBACK_DATA callback(binaries, iPage, m_bMeTa);
                            qr_scan_bitmap(hbm, callback, 0);

                            if (callback.m_error != QR_CALLBACK_DATA::VALID)
                            {
                                error = callback.m_error;
                                break;
                            }

                            hash = callback.m_hash;

                            DeleteObject(hbm);
                        }
                        pdfium.FPDF_ClosePage(pdf_page);
                    }
                }
                DeleteDC(hDC);
            }
            pdfium.FPDF_CloseDocument(pdf_doc);
        }
    }
    catch (...)
    {
        ;
    }

    for (size_t i = 0; i < binaries.size(); ++i)
    {
        if (binaries[i].empty())
        {
            error = QR_CALLBACK_DATA::INCOMPLETE_DATA;
        }
    }

    if (error != QR_CALLBACK_DATA::VALID)
    {
        scanError(hwnd, error);
        return FALSE;
    }

    return DoScanBinaries(hwnd, binaries);
}

BOOL MBlakerApp::DoScanImages(HWND hwnd, const std::vector<std::wstring>& files)
{
    std::vector<std::string> binaries;
    m_bMeTa = FALSE;
    QR_CALLBACK_DATA::ErrorType error = QR_CALLBACK_DATA::VALID;

    std::string hash;
    try
    {
        for (size_t i = 0; i < files.size(); ++i)
        {
            const std::wstring& file = files[i];
            char imm_file[MAX_PATH];

            WideCharToMultiByte(CP_ACP, 0, file.c_str(), -1, imm_file, MAX_PATH, NULL, NULL);

            cv::Mat mat = cv::imread(imm_file);
            cv::cvtColor(mat, mat, cv::COLOR_BGR2BGRA);
            cv::Mat tmp;
            mat.convertTo(tmp, CV_8UC4);

            // scan bitmap
            INT iPage = (INT)i;
            QR_CALLBACK_DATA callback(binaries, iPage, m_bMeTa);
            qr_scan_image(tmp, callback, 0);

            if (callback.m_error != QR_CALLBACK_DATA::VALID)
            {
                error = callback.m_error;
                break;
            }

            hash = callback.m_hash;
        }
    }
    catch (...)
    {
        ;
    }

    for (size_t i = 0; i < binaries.size(); ++i)
    {
        if (binaries[i].empty())
        {
            error = QR_CALLBACK_DATA::INCOMPLETE_DATA;
        }
    }

    if (error != QR_CALLBACK_DATA::VALID)
    {
        scanError(hwnd, error);
        return FALSE;
    }

    return DoScanBinaries(hwnd, binaries);
}

BOOL MBlakerApp::DoScanMovie(HWND hwnd, LPCWSTR pszMovieFile, PROGRESS_INFO *info)
{
    cv::VideoCapture cap;

    char fname[MAX_PATH];
    WideCharToMultiByte(CP_ACP, 0, pszMovieFile, -1,
        fname, ARRAYSIZE(fname), NULL, NULL);

    cap.open(fname);
    if (!cap.isOpened())
    {
        assert(0);
        ErrorBoxDx(IDS_INVALID_DATA);
        return FALSE;
    }

    std::vector<std::string> binaries;

    INT iPage = 1;
    m_bMeTa = FALSE;
    QR_CALLBACK_DATA::ErrorType error = QR_CALLBACK_DATA::VALID;

    cv::Mat frame;
    while (cap.read(frame))
    {
        if (info->bCancelled)
        {
            ErrorBoxDx(IDS_CANCELLED);
            return FALSE;
        }

        info->nCount++;

        // scan bitmap
        QR_CALLBACK_DATA callback(binaries, iPage, m_bMeTa);
        qr_scan_image(frame, callback, 0);

        if (callback.m_error != QR_CALLBACK_DATA::VALID)
        {
            error = callback.m_error;
            break;
        }
        ++iPage;
    }

    if (!m_bMeTa)
    {
        error = QR_CALLBACK_DATA::INVALID_FORMAT;
    }

    for (size_t i = 0; i < binaries.size(); ++i)
    {
        if (binaries[i].empty())
        {
            error = QR_CALLBACK_DATA::INCOMPLETE_DATA;
        }
    }

    if (error != QR_CALLBACK_DATA::VALID)
    {
        scanError(hwnd, error);
        return FALSE;
    }

    return DoScanBinaries(hwnd, binaries);
}

BOOL MBlakerApp::DoScanBinaries(HWND hwnd, std::vector<std::string>& binaries)
{
    if (binaries.empty())
    {
        ListView_DeleteAllItems(m_hListView);
        InvalidateRect(hwnd, NULL, TRUE);
        ErrorBoxDx(IDS_INVALID_DATA);
        return FALSE;
    }

    std::string binary;
    for (size_t i = 0; i < binaries.size(); ++i)
    {
        binary += binaries[i];
    }

    if (!m_bMeTa)
    {
        ListView_DeleteAllItems(m_hListView);

        FILE_ITEM file_item;
        file_item.filename = LoadStringDx(IDS_UNTITLED);
        file_item.binary = binary;
        AddFileItem(hwnd, file_item);

        return TRUE;
    }

    std::string& tbz = binary;
    std::string tar;
    if (int error = tbz2tar(&tbz[0], tbz.size(), tar))
    {
        assert(0);
        ErrorBoxDx(IDS_INVALID_DATA);
        return FALSE;
    }

    BINS bins;
    if (int error = tar2bins(&tar[0], tar.size(), bins))
    {
        assert(0);
        ErrorBoxDx(IDS_INVALID_DATA);
        return FALSE;
    }

    ListView_DeleteAllItems(m_hListView);
    for (size_t i = 0; i < bins.size(); ++i)
    {
        AddFileItem(hwnd, bins[i]);
    }

    return TRUE;
}

void MBlakerApp::OnOpen(HWND hwnd)
{
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = MakeFilterDx(LoadStringDx(IDS_ALLFILEFILTER));
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = ARRAYSIZE(szFile);
    ofn.lpstrTitle = LoadStringDx(IDS_OPEN_FILE);
    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST |
                OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = NULL;
    if (GetOpenFileNameW(&ofn))
    {
        DoLoad(hwnd, szFile);
    }
}

void MBlakerApp::OnScanPDF(HWND hwnd)
{
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = MakeFilterDx(LoadStringDx(IDS_PDFFILEFILTER));
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = ARRAYSIZE(szFile);
    ofn.lpstrTitle = LoadStringDx(IDS_CHOOSE_PDF);
    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST |
                OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"pdf";
    if (GetOpenFileNameW(&ofn))
    {
        DoScanPDF(hwnd, szFile);
    }
}

void MBlakerApp::OnScanImages(HWND hwnd)
{
    MScanImagesDlg dialog;
    if (IDOK == dialog.DialogBoxDx(hwnd, MAKEINTRESOURCE(IDD_SCAN_IMAGES)))
    {
        DoScanImages(hwnd, dialog.m_files);
    }
}

void MBlakerApp::OnSaveAs(HWND hwnd)
{
    if (m_bins.empty())
    {
        assert(0);
        ErrorBoxDx(IDS_EMPTY_DATA);
        return;
    }

    if (m_bins.size() == 1)
    {
        OPENFILENAMEW ofn;
        WCHAR szFile[MAX_PATH];
        StringCbCopyW(szFile, sizeof(szFile), m_bins[0].filename.c_str());
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = MakeFilterDx(LoadStringDx(IDS_ALLFILEFILTER));
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = ARRAYSIZE(szFile);
        ofn.lpstrTitle = LoadStringDx(IDS_SAVE_AS);
        ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_PATHMUSTEXIST |
                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
        LPTSTR pch = PathFindExtension(m_bins[0].filename.c_str());
        if (*pch == L'.');
            ++pch;
        ofn.lpstrDefExt = pch;
        if (GetSaveFileNameW(&ofn))
        {
            DoSave(hwnd, szFile, m_bins[0].binary);
        }
    }
    else
    {
        std::string tar, tbz;
        if (int error = bins2tar(m_bins, tar))
        {
            assert(0);
            ErrorBoxDx(IDS_CANTSAVEFILE);
            return;
        }
        if (int error = tar2tbz(tar.data(), tar.size(), tbz))
        {
            assert(0);
            ErrorBoxDx(IDS_CANTSAVEFILE);
            return;
        }

        OPENFILENAMEW ofn;
        WCHAR szFile[MAX_PATH];
        StringCbCopyW(szFile, sizeof(szFile), LoadStringDx(IDS_FILES_TBZ));
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = MakeFilterDx(LoadStringDx(IDS_ALLFILEFILTER));
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = ARRAYSIZE(szFile);
        ofn.lpstrTitle = LoadStringDx(IDS_SAVE_AS);
        ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_PATHMUSTEXIST |
                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = L"tbz";
        if (GetSaveFileNameW(&ofn))
        {
            DoSave(hwnd, szFile, tbz);
        }
    }
}

VOID MBlakerApp::OnPrint(HWND hwnd)
{
    m_cParts = 0;
    m_cPages = 0;
    if (HDC hDC = m_helper.ShowDialog(hwnd))
    {
        if (m_helper.DoPrint(hwnd, hDC, NULL))
        {
            if (MsgBoxDx(LoadStringPrintfDx(IDS_AREYOUPRINT, m_cPages),
                         LoadStringDx(IDS_BLAKER_PAPERS),
                         MB_ICONINFORMATION | MB_YESNOCANCEL) == IDYES)
            {
                std::wstring file = LoadStringDx(IDS_UNTITLED);
                LPCWSTR title = PathFindFileName(file.c_str());
                m_helper.DoPrint(hwnd, hDC, title);
            }
        }
        else
        {
            ErrorBoxDx(IDS_PRINTERR);
        }

        ::DeleteDC(hDC);
    }
}

BOOL MBlakerApp::DoDestroyImages(std::vector<HBITMAP>& bitmaps)
{
    for (size_t i = 0; i < bitmaps.size(); ++i)
    {
        DeleteObject(bitmaps[i]);
    }
    bitmaps.clear();
    return TRUE;
}

INT MBlakerApp::DoCreateImages(HWND hwnd, std::vector<HBITMAP>& bitmaps,
                                     SIZE sizImage, BOOL bTest)
{
    if (m_bins.empty())
    {
        assert(0);
        ErrorBoxDx(IDS_EMPTY_DATA);
        return 0;
    }

    std::string tar, tbz;
    if (int error = bins2tar(m_bins, tar))
    {
        assert(0);
        ErrorBoxDx(IDS_INVALID_DATA);
        return 0;
    }
    if (int error = tar2tbz(&tar[0], tar.size(), tbz))
    {
        assert(0);
        ErrorBoxDx(IDS_INVALID_DATA);
        return 0;
    }
    tar.clear();

    if (tbz.size() > MAX_SCREENABLE_BYTES)
    {
        ErrorBoxDx(IDS_TOOLARGEDATA);
        return 0;
    }

    SYSTEMTIME st;
    GetLocalTime(&st);

    std::string hash = Base64Int_Encode((BYTE)(st.wSecond ^ st.wMilliseconds));

    size_t nMaxQRData = QR_MAX_BYTES;

    char szMeTa[64];
    StringCbPrintfA(szMeTa, sizeof(szMeTa), "MeTa>XXX>YYY>%s|", hash.c_str());
    INT nMeTaLen = lstrlenA(szMeTa);
    nMaxQRData -= strlen(szMeTa);
    std::string binary;
    const char *data = &tbz[0];
    size_t total_size = tbz.size();
    size_t count = total_size;
    INT iPart = 1;

    INT x, y, cx, cy;
    if (HDC hDC = CreateCompatibleDC(NULL))
    {
        for (size_t i = 0; i < tbz.size();)
        {
            x = y = 0;
            cx = sizImage.cx;
            cy = sizImage.cy;

            INT cxSpace = cx;
            INT cySpace = cy;
            float space_x = m_helper.InchesFromPixelsX(hDC, cxSpace);
            float space_y = m_helper.InchesFromPixelsY(hDC, cySpace);
            float space = std::min(space_x, space_y);
            size_t bytes;
            if (tbz.size() - i < nMaxQRData)
                bytes = tbz.size() - i;
            else
                bytes = nMaxQRData;

            int qr_width = qr_width_from_bytes(bytes);
            INT width = m_helper.PixelsFromInchesX(hDC, qr_width * m_settings.eDotSize);
            INT height = m_helper.PixelsFromInchesY(hDC, qr_width * m_settings.eDotSize);
            cx = LONG(width);
            cy = LONG(height);
            if (qr_width == 0 || bytes <= nMeTaLen)
                break;

            while (cx > sizImage.cx || cy > sizImage.cy ||
                   m_helper.InchesFromPixelsX(hDC, cx) / qr_width < m_settings.eDotSize ||
                   m_helper.InchesFromPixelsY(hDC, cy) / qr_width < m_settings.eDotSize)
            {
                if (bytes < 17 || bytes <= nMeTaLen)
                    break;
                if (bytes > 2000)
                    bytes -= 116;
                else if (bytes > 1000)
                    bytes -= 74;
                else if (bytes > 200)
                    bytes -= 38;
                else if (bytes > 15)
                    bytes -= 15;
                if (bytes < 17 || bytes <= nMeTaLen)
                    break;
                qr_width = qr_width_from_bytes(bytes);
                if (qr_width == 0)
                    break;
                width = m_helper.PixelsFromInchesX(hDC, qr_width * m_settings.eDotSize);
                height = m_helper.PixelsFromInchesY(hDC, qr_width * m_settings.eDotSize);
                cx = LONG(width);
                cy = LONG(height);
            }
            if (qr_width == 0 || bytes <= nMeTaLen)
                break;

            std::string str(data, bytes);
            StringCbPrintfA(szMeTa, sizeof(szMeTa), "MeTa>%03u>%03u>%s|",
                            iPart, m_cParts, hash.c_str());
            str.insert(0, std::string(szMeTa));

            if (bTest)
            {
                if (HBITMAP hbm = qr_create_mono_bitmap(1, 1))
                {
                    bitmaps.push_back(hbm);
                }
                else
                {
                    ErrorBoxDx(IDS_CANTGENQRCODE);
                    return 0;
                }
            }
            else
            {
                INT cxQR, cyQR;
                if (HBITMAP hbm = qr_create_bitmap(hwnd, &str[0], str.size(), cxQR, cyQR))
                {
                    bitmaps.push_back(hbm);
                }
                else
                {
                    ErrorBoxDx(IDS_CANTGENQRCODE);
                    return 0;
                }
            }

            ++iPart;
            i += bytes;
            data += bytes;
        }
        m_cParts = iPart - 1;

        DeleteDC(hDC);
    }

    return TRUE;
}

BOOL MBlakerApp::DoScreenImages(HWND hwnd)
{
    ::EnableWindow(hwnd, FALSE);

    MBlakerScreen screen(m_renderer, FALSE);

    if (!screen.CreateDialogDx(hwnd, IDD_IMAGES_MOVIE))
    {
        assert(0);
        ::EnableWindow(hwnd, TRUE);
        ::SetForegroundWindow(hwnd);
        return FALSE;
    }

    ::ShowWindow(screen, SW_SHOWNORMAL);
    ::UpdateWindow(screen);

    SIZE sizImage = screen.m_sizImage;
    sizImage.cx -= SCREEN_MARGIN;
    sizImage.cy -= SCREEN_MARGIN;

    std::vector<HBITMAP> bitmaps;
    if (!DoCreateImages(hwnd, bitmaps, sizImage, TRUE))
    {
        assert(0);
        DestroyWindow(screen);
        ::EnableWindow(hwnd, TRUE);
        ::SetForegroundWindow(hwnd);
        return FALSE;
    }
    DoDestroyImages(bitmaps);

    if (!DoCreateImages(hwnd, bitmaps, sizImage, FALSE))
    {
        assert(0);
        DestroyWindow(screen);
        ::EnableWindow(hwnd, TRUE);
        ::SetForegroundWindow(hwnd);
        return FALSE;
    }

    ::ShowWindow(screen, SW_SHOWNORMAL);
    ::UpdateWindow(screen);

    PostMessage(screen, WM_COMMAND, IDNO, 0);

    screen.SetBitmaps(bitmaps);
    screen.m_bOK = TRUE;

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        if (::IsDialogMessage(screen, &msg))
            continue;
        if (::IsDialogMessage(screen.m_hwndChildDlg, &msg))
            continue;

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    ::EnableWindow(hwnd, TRUE);
    ::SetForegroundWindow(hwnd);
    DoDestroyImages(bitmaps);
    return TRUE;
}

BOOL MBlakerApp::DoScreenMovie(HWND hwnd)
{
    ::EnableWindow(hwnd, FALSE);

    MBlakerScreen screen(m_renderer, TRUE);

    if (!screen.CreateDialogDx(hwnd, IDD_IMAGES_MOVIE))
    {
        assert(0);
        ::EnableWindow(hwnd, TRUE);
        ::SetForegroundWindow(hwnd);
        return FALSE;
    }

    SIZE sizImage = screen.m_sizImage;
    sizImage.cx -= SCREEN_MARGIN;
    sizImage.cy -= SCREEN_MARGIN;

    std::vector<HBITMAP> bitmaps;
    if (!DoCreateImages(hwnd, bitmaps, sizImage, TRUE))
    {
        assert(0);
        DestroyWindow(screen);
        ::EnableWindow(hwnd, TRUE);
        ::SetForegroundWindow(hwnd);
        return FALSE;
    }
    DoDestroyImages(bitmaps);

    if (!DoCreateImages(hwnd, bitmaps, sizImage, FALSE))
    {
        assert(0);
        DestroyWindow(screen);
        ::EnableWindow(hwnd, TRUE);
        ::SetForegroundWindow(hwnd);
        return FALSE;
    }

    ::ShowWindow(screen, SW_SHOWNORMAL);
    ::UpdateWindow(screen);

    PostMessage(screen, WM_COMMAND, IDYES, 0);

    screen.SetBitmaps(bitmaps);
    screen.m_bOK = TRUE;

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0))
    {
        if (::IsDialogMessage(screen, &msg))
            continue;
        if (::IsDialogMessage(screen.m_hwndChildDlg, &msg))
            continue;

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    ::EnableWindow(hwnd, TRUE);
    ::SetForegroundWindow(hwnd);
    DoDestroyImages(bitmaps);
    return TRUE;
}

void MBlakerApp::OnScreen(HWND hwnd)
{
    MChooseOptionDlg choose_option;
    INT nID = (INT)choose_option.DialogBoxDx(hwnd, IDD_CHOOSE_OPTION);
    switch (nID)
    {
    case psh1:
        DoScreenImages(hwnd);
        break;
    case psh2:
        DoScreenMovie(hwnd);
        break;
    case IDCLOSE:
        break;
    }
}

void MBlakerApp::OnScanMovie(HWND hwnd)
{
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = MakeFilterDx(LoadStringDx(IDS_MOVFILEFILTER));
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = ARRAYSIZE(szFile);
    ofn.lpstrTitle = LoadStringDx(IDS_CHOOSE_MOVIE);
    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST |
                OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"avi";
    if (GetOpenFileNameW(&ofn))
    {
        MScanningMovieDlg dialog(this, szFile);
        dialog.DialogBoxDx(hwnd);
    }
}

void MBlakerApp::OnNew(HWND hwnd)
{
    ListView_DeleteAllItems(m_hListView);
}

void MBlakerApp::OnSelectAll(HWND hwnd)
{
    INT i, count = ListView_GetItemCount(m_hListView);
    for (i = 0; i < count; ++i)
    {
        ListView_SetItemState(m_hListView, i, LVIS_SELECTED, LVIS_SELECTED);
    }
}

void MBlakerApp::OnBeginDrag(HWND hwnd)
{
    std::vector<INT> selection;

    INT iItem = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
    while (iItem != -1)
    {
        selection.push_back(iItem);
        iItem = ListView_GetNextItem(m_hListView, iItem, LVNI_SELECTED);
    }

    if (selection.empty())
        return;

    std::wstring temp_dir;
    if (!CreateTempDir(temp_dir))
    {
        assert(0);
        ErrorBoxDx(IDS_CANTCREATETEMPDIR);
        return;
    }
    m_temp_dirs.push_back(temp_dir);

    std::vector<std::wstring> names;
    for (size_t i = 0; i < selection.size(); ++i)
    {
        INT iItem = selection[i];

        LV_ITEM item;
        TCHAR szText[100];
        szText[0] = 0;

        // get item text
        ZeroMemory(&item, sizeof(item));
        item.mask = LVIF_TEXT;
        item.iItem = iItem;
        item.pszText = szText;
        item.cchTextMax = ARRAYSIZE(szText);
        if (!ListView_GetItem(m_hListView, &item))
        {
            assert(0);
            return;
        }

        // check name
        std::wstring name = szText;
        if (name != m_bins[iItem].filename)
        {
            assert(0);
            return;
        }

        // get full path
        WCHAR szFile[MAX_PATH];
        StringCbCopyW(szFile, sizeof(szFile), temp_dir.c_str());
        PathAppend(szFile, name.c_str());
        name = szFile;

        // create temporary file
        if (!DoSave(hwnd, szFile, m_bins[i].binary))
        {
            return;
        }

        // add name
        names.push_back(name);
    }
    selection.clear();

    // create drag drop using temporary files
    if (IDataObject *pDataObject = GetFileDataObject(names))
    {
        if (IDropSource *pSource = MDropSource::CreateInstance())
        {
            DWORD dwEffect;
            ::DoDragDrop(pDataObject, pSource, DROPEFFECT_COPY, &dwEffect);

            pSource->Release();
        }
        pDataObject->Release();
    }
}

BOOL MBlakerApp::CreateTempDir(std::wstring& full_path)
{
    full_path = L".";
    std::srand(::GetTickCount());

    WCHAR szPath[MAX_PATH];
    if (!GetTempPathW(MAX_PATH, szPath))
        return FALSE;

    static const WCHAR hex[] = L"0123456789ABCDEF";
    WCHAR szDir[MAX_PATH];
    const INT retry_count = 16;
    for (INT k = 0; k < retry_count; ++k)
    {
        std::wstring hash;
        for (INT i = 0; i < 10; ++i)
        {
            hash += hex[std::rand() % 16];
        }

        StringCbCopyW(szDir, sizeof(szDir), szPath);
        PathAppendW(szDir, hash.c_str());

        if (CreateDirectoryW(szDir, NULL))
        {
            full_path = szDir;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL MBlakerApp::DoLoadSettings(BLAKER_SETTINGS& settings)
{
    settings.reset();

    HKEY hSoftware = NULL;
    ::RegOpenKeyExW(HKEY_CURRENT_USER, L"Software", 0, KEY_READ, &hSoftware);
    if (!hSoftware)
        return FALSE;

    WCHAR szText[300];
    DWORD cb;
    BOOL bLoaded = FALSE;

    HKEY hCompany = NULL;
    ::RegOpenKeyExW(hSoftware, L"Katayama Hirofumi MZ", 0, KEY_READ, &hCompany);
    if (hCompany)
    {
        HKEY hApp = NULL;
        ::RegOpenKeyExW(hCompany, L"BLAKER", 0, KEY_READ, &hApp);
        if (hApp)
        {
            // eDotSize
            cb = sizeof(szText);
            if (::RegQueryValueExW(hApp, L"eDotSize", NULL, NULL,
                                   LPBYTE(szText), &cb) == ERROR_SUCCESS)
            {
                WCHAR *endptr;
                double eValue = wcstod(szText, &endptr);
                if (*endptr == 0 && (float)eValue > 0)
                {
                    settings.eDotSize = (float)eValue;
                }
            }

            bLoaded = TRUE;
            ::RegCloseKey(hApp);
        }
        ::RegCloseKey(hCompany);
    }
    ::RegCloseKey(hSoftware);

    return bLoaded;
}

BOOL MBlakerApp::DoSaveSettings(const BLAKER_SETTINGS& settings)
{
    HKEY hSoftware = NULL;
    ::RegCreateKeyExW(HKEY_CURRENT_USER, L"Software", 0, NULL, 0, KEY_ALL_ACCESS,
                      NULL, &hSoftware, NULL);
    if (!hSoftware)
        return FALSE;

    BOOL bSaved = FALSE;
    WCHAR szText[300];

    HKEY hCompany = NULL;
    ::RegCreateKeyExW(hSoftware, L"Katayama Hirofumi MZ", 0, NULL, 0, KEY_ALL_ACCESS,
                      NULL, &hCompany, NULL);
    if (hCompany)
    {
        HKEY hApp = NULL;
        ::RegCreateKeyExW(hSoftware, L"BLAKER", 0, NULL, 0, KEY_ALL_ACCESS,
                          NULL, &hApp, NULL);
        if (hApp)
        {
            // eDotSize
            StringCbPrintfW(szText, sizeof(szText), L"%f", settings.eDotSize);
            DWORD cb = (lstrlenW(szText) + 1) * sizeof(WCHAR);
            ::RegSetValueExW(hApp, L"eDotSize", 0, REG_SZ, (LPBYTE)szText, cb);

            bSaved = TRUE;

            ::RegCloseKey(hApp);
        }
        ::RegCloseKey(hCompany);
    }
    ::RegCloseKey(hSoftware);

    return bSaved;
}

void MBlakerApp::OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
    ::SetFocus(m_hListView);
}

void MBlakerApp::OnRename(HWND hwnd)
{
    INT iItem = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
    if (iItem != -1)
    {
        ListView_EditLabel(m_hListView, iItem);
    }
}

void MBlakerApp::OnDelete(HWND hwnd)
{
    INT iItem = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
    while (iItem != -1)
    {
        ListView_DeleteItem(m_hListView, iItem);
        iItem = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
    }
}

void MBlakerApp::OnAbout(HWND hwnd)
{
    MAboutDlg dialog;
    dialog.DialogBoxDx(hwnd);
}

void MBlakerApp::OnOpenReadMe(HWND hwnd)
{
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath));
    LPTCH pch = PathFindFileName(szPath);
    *pch = 0;
    PathAppend(szPath, TEXT("ReadMe.txt"));
    if (!PathFileExists(szPath))
    {
        *pch = 0;
        PathAppend(szPath, TEXT("..\\ReadMe.txt"));
    }
    if (!PathFileExists(szPath))
    {
        *pch = 0;
        PathAppend(szPath, TEXT("..\\..\\ReadMe.txt"));
    }
    ShellExecute(hwnd, NULL, szPath, NULL, NULL, 0);
}

void MBlakerApp::OnSettings(HWND hwnd)
{
    MSettingsDlg dialog(m_settings);
    dialog.DialogBoxDx(hwnd);
}

void MBlakerApp::OnSaveSelection(HWND hwnd)
{
    if (m_bins.empty())
    {
        assert(0);
        ErrorBoxDx(IDS_EMPTY_DATA);
        return;
    }

    std::vector<INT> selection;
    INT iItem = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
    while (iItem != -1)
    {
        selection.push_back(iItem);
        iItem = ListView_GetNextItem(m_hListView, iItem, LVNI_SELECTED);
    }

    if (selection.empty())
    {
        return;
    }

    if (selection.size() == 1)
    {
        const FILE_ITEM& file_item = m_bins[selection[0]];
        OPENFILENAMEW ofn;
        WCHAR szFile[MAX_PATH];
        StringCbCopyW(szFile, sizeof(szFile), file_item.filename.c_str());
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = MakeFilterDx(LoadStringDx(IDS_ALLFILEFILTER));
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = ARRAYSIZE(szFile);
        ofn.lpstrTitle = LoadStringDx(IDS_SAVE_AS);
        ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_PATHMUSTEXIST |
                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
        LPTSTR pch = PathFindExtension(file_item.filename.c_str());
        if (*pch == L'.');
            ++pch;
        ofn.lpstrDefExt = pch;
        if (GetSaveFileNameW(&ofn))
        {
            DoSave(hwnd, szFile, file_item.binary);
        }
    }
    else
    {
        BINS bins;
        for (size_t i = 0; i < selection.size(); ++i)
        {
            const FILE_ITEM& file_item = m_bins[selection[i]];
            bins.push_back(file_item);
        }

        std::string tar, tbz;
        if (int error = bins2tar(bins, tar))
        {
            assert(0);
            ErrorBoxDx(IDS_CANTSAVEFILE);
            return;
        }
        if (int error = tar2tbz(tar.data(), tar.size(), tbz))
        {
            assert(0);
            ErrorBoxDx(IDS_CANTSAVEFILE);
            return;
        }

        OPENFILENAMEW ofn;
        WCHAR szFile[MAX_PATH];
        StringCbCopyW(szFile, sizeof(szFile), LoadStringDx(IDS_FILES_TBZ));
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = MakeFilterDx(LoadStringDx(IDS_ALLFILEFILTER));
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = ARRAYSIZE(szFile);
        ofn.lpstrTitle = LoadStringDx(IDS_SAVE_AS);
        ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_PATHMUSTEXIST |
                    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
        ofn.lpstrDefExt = L"tbz";
        if (GetSaveFileNameW(&ofn))
        {
            DoSave(hwnd, szFile, tbz);
        }
    }
}

void MBlakerApp::OnRClick(HWND hwnd)
{
    HMENU hMenu = ::LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(2));
    HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
    if (!hMenu || !hSubMenu)
        return;

    POINT pt;
    ::GetCursorPos(&pt);
    ::SetForegroundWindow(hwnd);
    UINT uFlags = TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_RETURNCMD;
    UINT nID = ::TrackPopupMenu(hSubMenu, uFlags, pt.x, pt.y, 0, hwnd, NULL);
    ::PostMessage(hwnd, WM_NULL, 0, 0);

    ::DestroyMenu(hMenu);

    ::PostMessage(hwnd, WM_COMMAND, nID, 0);
}

LRESULT MBlakerApp::OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr)
{
    if (pnmhdr->hwndFrom == m_hListView)
    {
        NM_LISTVIEW *pListView = (NM_LISTVIEW *)pnmhdr;
        LV_DISPINFO *pDispInfo = (LV_DISPINFO *)pnmhdr;
        LV_KEYDOWN *pKeyDown = (LV_KEYDOWN *)pnmhdr;
        switch (pnmhdr->code)
        {
        case LVN_DELETEITEM:
            m_bins.erase(m_bins.begin() + pListView->iItem);
            if (m_bins.empty())
                ReCreateImageList();
            break;
        case LVN_DELETEALLITEMS:
            ReCreateImageList();
            break;
        case LVN_BEGINLABELEDIT:
            if (size_t(pDispInfo->item.iItem) < m_bins.size())
            {
                return FALSE;
            }
            return TRUE;
        case LVN_ENDLABELEDIT:
            if (size_t(pDispInfo->item.iItem) < m_bins.size())
            {
                if (LPTSTR pszText = pDispInfo->item.pszText)
                {
                    for (size_t i = 0; pszText[i]; ++i)
                    {
                        for (size_t i = 0; pszText[i]; ++i)
                        {
                            if (wcschr(L"\\/:*?\"<>|", pszText[i]) != NULL)
                            {
                                pszText[i] = L'_';
                            }
                        }
                    }
                    m_bins[pDispInfo->item.iItem].filename = pszText;
                    return TRUE;
                }
            }
            break;
        case LVN_BEGINDRAG:
            OnBeginDrag(hwnd);
            break;
        case NM_RCLICK:
            OnRClick(hwnd);
            break;
        }
    }
    return FALSE;
}

void MBlakerApp::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    EnterCommand(hwnd);

    switch (id)
    {
    case ID_OPEN:
        OnOpen(hwnd);
        break;
    case ID_SAVEAS:
        OnSaveAs(hwnd);
        break;
    case ID_PAGESETUP:
        m_helper.DoPageSetupDlg(hwnd);
        break;
    case ID_PRINT:
        OnPrint(hwnd);
        break;
    case ID_EXIT:
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        break;
    case ID_METHOD_QRCODE:
        m_method = METHOD_QRCODE;
        break;
    case ID_METHOD_QRCODE_META:
        m_method = METHOD_QRCODE_META;
        break;
    case ID_METHOD_BASE64:
        m_method = METHOD_BASE64;
        break;
    case ID_SCAN_PDF:
        OnScanPDF(hwnd);
        break;
    case ID_SCAN_IMAGES:
        OnScanImages(hwnd);
        break;
    case ID_READY:
        break;
    case ID_SCREEN:
        OnScreen(hwnd);
        break;
    case ID_SCAN_MOVIE:
        OnScanMovie(hwnd);
        break;
    case ID_NEW:
        OnNew(hwnd);
        break;
    case ID_SELECT_ALL:
        OnSelectAll(hwnd);
        break;
    case ID_RENAME:
        OnRename(hwnd);
        break;
    case ID_DELETE:
        OnDelete(hwnd);
        break;
    case ID_SAVE_SELECTION:
        OnSaveSelection(hwnd);
        break;
    case ID_ABOUT:
        OnAbout(hwnd);
        break;
    case ID_OPEN_README:
        OnOpenReadMe(hwnd);
        break;
    case ID_SETTINGS:
        OnSettings(hwnd);
        break;
    }

    LeaveCommand(hwnd);
}

void MBlakerApp::OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    RECT rc;
    GetClientRect(hwnd, &rc);

    SendMessage(m_hStatusWnd, WM_SIZE, 0, 0);

    RECT rcStatus;
    GetWindowRect(m_hStatusWnd, &rcStatus);
    INT cyStatus = rcStatus.bottom - rcStatus.top;

    MoveWindow(m_hListView, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top - cyStatus, TRUE);
}

void MBlakerApp::OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu)
{
    INT iItem = ListView_GetNextItem(m_hListView, -1, LVNI_SELECTED);
    if (iItem == -1)
    {
        EnableMenuItem(hMenu, ID_RENAME, MF_GRAYED);
        EnableMenuItem(hMenu, ID_DELETE, MF_GRAYED);
        EnableMenuItem(hMenu, ID_SAVE_SELECTION, MF_GRAYED);
    }
    else
    {
        EnableMenuItem(hMenu, ID_RENAME, MF_ENABLED);
        EnableMenuItem(hMenu, ID_DELETE, MF_ENABLED);
        EnableMenuItem(hMenu, ID_SAVE_SELECTION, MF_ENABLED);
    }

    INT cItems = ListView_GetItemCount(m_hListView);
    if (cItems == 0)
    {
        EnableMenuItem(hMenu, ID_SELECT_ALL, MF_GRAYED);
        EnableMenuItem(hMenu, ID_PRINT, MF_GRAYED);
        EnableMenuItem(hMenu, ID_SCREEN, MF_GRAYED);
        EnableMenuItem(hMenu, ID_SAVEAS, MF_GRAYED);
    }
    else
    {
        EnableMenuItem(hMenu, ID_SELECT_ALL, MF_ENABLED);
        EnableMenuItem(hMenu, ID_PRINT, MF_ENABLED);
        EnableMenuItem(hMenu, ID_SCREEN, MF_ENABLED);
        EnableMenuItem(hMenu, ID_SAVEAS, MF_ENABLED);
    }

    switch (m_method)
    {
    case METHOD_QRCODE:
        CheckMenuRadioItem(hMenu, ID_METHOD_QRCODE, ID_METHOD_BASE64, ID_METHOD_QRCODE,
                           MF_BYCOMMAND);
        break;
    case METHOD_QRCODE_META:
        CheckMenuRadioItem(hMenu, ID_METHOD_QRCODE, ID_METHOD_BASE64, ID_METHOD_QRCODE_META,
                           MF_BYCOMMAND);
        break;
    case METHOD_BASE64:
        CheckMenuRadioItem(hMenu, ID_METHOD_QRCODE, ID_METHOD_BASE64, ID_METHOD_BASE64,
                           MF_BYCOMMAND);
        break;
    }
}

void MBlakerApp::OnDropFiles(HWND hwnd, HDROP hdrop)
{
    EnterCommand(hwnd);

    WCHAR szFile[MAX_PATH];
    UINT i, count = DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, 0);
    for (i = 0; i < count; ++i)
    {
        DragQueryFileW(hdrop, i, szFile, ARRAYSIZE(szFile));
        DoLoad(hwnd, szFile);
    }
    DragFinish(hdrop);

    SetForegroundWindow(hwnd);

    LeaveCommand(hwnd);
}

BOOL MBlakerApp::DoPrintPages(HWND hwnd, HDC hDC, LPCWSTR pszDocName)
{
    SetStretchBltMode(hDC, STRETCH_DELETESCANS);

    // physical paper size in millimeters
    const int nHorzSize = ::GetDeviceCaps(hDC, HORZSIZE);
    const int nVertSize = ::GetDeviceCaps(hDC, VERTSIZE);

    // DPI (dots per inch)
    const int nLogPixelX = ::GetDeviceCaps(hDC, LOGPIXELSX);
    const int nLogPixelY = ::GetDeviceCaps(hDC, LOGPIXELSY);

    // width and height of paper in pixels
    const int cxPaper = ::GetDeviceCaps(hDC, HORZRES);
    const int cyPaper = ::GetDeviceCaps(hDC, VERTRES);

    // margin and paper size in 1000th inches
    RECT rtMargin = m_helper.m_psd.rtMargin;
    POINT ptPaperSize = m_helper.m_psd.ptPaperSize;
    if (!(m_helper.PageFlags() & PSD_INTHOUSANDTHSOFINCHES))
    {
        rtMargin.left = m_helper.Inch1000FromMM100(rtMargin.left);
        rtMargin.top = m_helper.Inch1000FromMM100(rtMargin.top);
        rtMargin.right = m_helper.Inch1000FromMM100(rtMargin.right);
        rtMargin.bottom = m_helper.Inch1000FromMM100(rtMargin.bottom);

        ptPaperSize.x = m_helper.Inch1000FromMM100(ptPaperSize.x);
        ptPaperSize.y = m_helper.Inch1000FromMM100(ptPaperSize.y);
    }

    // printable area in pixels
    RECT rcPrintArea;
    rcPrintArea.left = (nLogPixelX * rtMargin.left) / 1000;
    rcPrintArea.right = cxPaper - (nLogPixelX * rtMargin.right) / 1000;
    rcPrintArea.top = (nLogPixelY * rtMargin.top) / 1000;
    rcPrintArea.bottom = cyPaper - (nLogPixelY * rtMargin.bottom) / 1000;

    SIZE sizPrintArea;
    sizPrintArea.cx = rcPrintArea.right - rcPrintArea.left;
    sizPrintArea.cy = rcPrintArea.bottom - rcPrintArea.top;

    HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);

    INT cxChar = nLogPixelX * 10 / 100;    // 0.10 inch     // 2
    INT cyChar = nLogPixelY * 15 / 100;    // 0.15 inch     // 3

    size_t nMaxQRData = QR_MAX_BYTES;
    INT cyHeader = cyChar * 1;
    INT nColumns = sizPrintArea.cx / cxChar;
    INT nRows = (sizPrintArea.cy - cyHeader) / cyChar;
    INT nCells = nColumns * nRows;

    INT cxMargin = m_helper.PixelsFromInchesX(hDC, QR_MARGIN);
    INT cyMargin = m_helper.PixelsFromInchesY(hDC, QR_MARGIN);

    SYSTEMTIME st;
    GetLocalTime(&st);

    std::string hash = Base64Int_Encode((BYTE)(st.wSecond ^ st.wMilliseconds));

    std::string tar, bin;
    if (m_method == METHOD_QRCODE_META)
    {
        if (int error = bins2tar(m_bins, tar))
        {
            assert(0);
            ErrorBoxDx(IDS_INVALID_DATA);
            return 0;
        }
        if (int error = tar2tbz(&tar[0], tar.size(), bin))
        {
            assert(0);
            ErrorBoxDx(IDS_INVALID_DATA);
            return 0;
        }
    }
    else
    {
        if (m_bins.size() != 1)
        {
            assert(0);
            ErrorBoxDx(IDS_RAWQRLIMIT);
            return 0;
        }
        bin = m_bins[0].binary;
    }

    CHAR szText[256];
    INT iPage = 0;
    switch (m_method)
    {
    case METHOD_BASE64:
        {
            const char *method_str = "BASE64";
            std::string encoded = base64_encode(bin, 0);
            const char *data = &encoded[0];
            size_t total_size = encoded.size();
            if (total_size > MAX_PRINTABLE_BYTES)
            {
                ErrorBoxDx(IDS_TOOLARGEDATA);
                return 0;
            }
            size_t count = total_size;
            for (size_t i = 0; i < encoded.size(); i += nCells)
            {
                if (pszDocName)
                {
                    if (::StartPage(hDC) <= 0)
                        return 0;
                }

                INT x, y, cx, cy;
                x = rcPrintArea.left;
                y = rcPrintArea.top;

                if (pszDocName)
                {
                    if (m_cPages >= 10)
                    {
                        StringCbPrintfA(szText, sizeof(szText),
                            "\x02 %04u.%02u.%02u %02u:%02u:%02u \x02 Page:%02u/%02u \x02",
                            st.wYear, st.wMonth, st.wDay,
                            st.wHour, st.wMinute, st.wSecond,
                            iPage + 1, m_cPages);
                    }
                    else
                    {
                        StringCbPrintfA(szText, sizeof(szText),
                            "\x02 %04u.%02u.%02u %02u:%02u:%02u \x02 Page:%01u/%01u \x02",
                            st.wYear, st.wMonth, st.wDay,
                            st.wHour, st.wMinute, st.wSecond,
                            iPage + 1, m_cPages);
                    }
                    m_renderer.drawString(hDC, szText, x, y, cxChar, cyChar);
                }
                y += cyChar;

                if (pszDocName)
                {
                    if (HDC hdcMem = CreateCompatibleDC(NULL))
                    {
                        RECT rc;
                        SetRect(&rc, 0, 0, CX_CHAR * nColumns, CY_CHAR * nRows);
                        if (HBITMAP hbm = Create24BppBitmapDx(rc.right, rc.bottom))
                        {
                            HGDIOBJ hbmOld = SelectObject(hdcMem, hbm);
                            {
                                FillRect(hdcMem, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                                size_t k = 0;
                                INT x0 = 0, y0 = 0;
                                for (int iy = 0; iy < nRows; ++iy)
                                {
                                    for (int ix = 0; ix < nColumns; ++ix)
                                    {
                                        m_renderer.drawChar(hdcMem, data[k++], x0, y0, CX_CHAR, CY_CHAR);
                                        if (--count == 0)
                                            goto quit;
                                        x0 += CX_CHAR;
                                    }
                                    x0 = 0;
                                    y0 += CY_CHAR;
                                }
                            }
quit:
                            StretchBlt(hDC, x, y, cxChar * nColumns, cyChar * nRows,
                                hdcMem, 0, 0, rc.right, rc.bottom, SRCCOPY);
                            SelectObject(hdcMem, hbmOld);
                        }
                        DeleteDC(hdcMem);
                    }
                }
                if (pszDocName)
                    ::EndPage(hDC);

                data += nCells;
                ++iPage;

                if (iPage >= MAX_PAGES)
                {
                    ErrorBoxDx(IDS_EXCEEDMAXPAGE);
                    iPage = 0;
                    break;
                }
            }
        }
        break;
    case METHOD_QRCODE:
        {
            const char *method_str = "QR";
            const char *data = &bin[0];
            size_t total_size = bin.size();
            if (total_size > MAX_PRINTABLE_BYTES)
            {
                ErrorBoxDx(IDS_TOOLARGEDATA);
                return 0;
            }
            size_t count = total_size;
            size_t bytes;

            INT cxSpace = sizPrintArea.cx;
            INT cySpace = sizPrintArea.cy - cyHeader;
            float space_x = m_helper.InchesFromPixelsX(hDC, cxSpace);
            float space_y = m_helper.InchesFromPixelsY(hDC, cySpace);
            float space = std::min(space_x, space_y);
            bytes = qr_best_bytes_from_space(space, nMaxQRData, m_settings.eDotSize);

            for (size_t i = 0; i < bin.size(); i += bytes)
            {
                if (bin.size() - i < nMaxQRData)
                    bytes = bin.size() - i;

                if (pszDocName)
                {
                    if (::StartPage(hDC) <= 0)
                        return 0;
                }

                INT x, y, cx, cy;
                x = rcPrintArea.left;
                y = rcPrintArea.top;

                if (pszDocName)
                {
                    if (m_cPages >= 10)
                    {
                        StringCbPrintfA(szText, sizeof(szText),
                            "\x02 %04u.%02u.%02u %02u:%02u:%02u \x02 Page:%02u/%02u \x02",
                            st.wYear, st.wMonth, st.wDay,
                            st.wHour, st.wMinute, st.wSecond,
                            iPage + 1, m_cPages);
                    }
                    else
                    {
                        StringCbPrintfA(szText, sizeof(szText),
                            "\x02 %04u.%02u.%02u %02u:%02u:%02u \x02 Page:%01u/%01u \x02",
                            st.wYear, st.wMonth, st.wDay,
                            st.wHour, st.wMinute, st.wSecond,
                            iPage + 1, m_cPages);
                    }
                    m_renderer.drawString(hDC, szText, x, y, cxChar, cyChar);
                }
                y += cyChar;

                if (pszDocName)
                {
                    INT cxQR, cyQR;
                    if (HBITMAP hbm = qr_create_bitmap(hwnd, data, bytes, cxQR, cyQR))
                    {
                        float fx = m_helper.InchesFromPixelsX(hDC, sizPrintArea.cx) - QR_MARGIN;
                        float fy = m_helper.InchesFromPixelsY(hDC, sizPrintArea.cy - cyHeader) - QR_MARGIN;
                        INT f = std::min(fx, fy);
                        cx = m_helper.PixelsFromInchesX(hDC, f);
                        cy = m_helper.PixelsFromInchesY(hDC, f);
                        if (HDC hdcMem = CreateCompatibleDC(NULL))
                        {
                            HGDIOBJ hbmOld = SelectObject(hdcMem, hbm);
                            {
                                x = (rcPrintArea.left + rcPrintArea.right - cx) / 2;
                                y += cxMargin;
                                StretchBlt(hDC, x, y, cx, cy,
                                           hdcMem, 0, 0, cxQR, cyQR, SRCCOPY);
                                m_renderer.drawString(hDC, "BLAKER",
                                    x, y - cyChar / 2, cxChar / 2, cyChar / 2);
                            }
                            SelectObject(hdcMem, hbmOld);
                            DeleteDC(hdcMem);
                        }
                        DeleteObject(hbm);
                    }
                    else
                    {
                        ErrorBoxDx(IDS_CANTGENQRCODE);
                        return 0;
                    }
                }

                if (pszDocName)
                {
                    ::EndPage(hDC);
                }

                data += bytes;
                ++iPage;
                if (iPage >= MAX_PAGES)
                {
                    ErrorBoxDx(IDS_EXCEEDMAXPAGE);
                    iPage = 0;
                    break;
                }
            }
        }
        break;
    case METHOD_QRCODE_META:
        {
            const char *method_str = "Blaker QR";
            char szMeTa[64];
            StringCbPrintfA(szMeTa, sizeof(szMeTa), "MeTa>XX>YY>%s|", hash.c_str());
            INT nMeTaLen = lstrlenA(szMeTa);
            nMaxQRData -= strlen(szMeTa);
            std::string binary;
            const char *data = &bin[0];
            size_t total_size = bin.size();
            if (total_size > MAX_PRINTABLE_BYTES)
            {
                ErrorBoxDx(IDS_TOOLARGEDATA);
                return FALSE;
            }
            size_t count = total_size;
            INT iPart = 1;

            INT x, y, cx, cy;
            for (size_t i = 0; i < bin.size();)
            {
                if (pszDocName)
                {
                    if (::StartPage(hDC) <= 0)
                        return 0;
                }

                x = rcPrintArea.left;
                y = rcPrintArea.top;

                if (pszDocName)
                {
                    if (m_cPages >= 10)
                    {
                        StringCbPrintfA(szText, sizeof(szText),
                            "\x02 %04u.%02u.%02u %02u:%02u:%02u \x02 Page:%02u/%02u \x02",
                            st.wYear, st.wMonth, st.wDay,
                            st.wHour, st.wMinute, st.wSecond,
                            iPage + 1, m_cPages);
                    }
                    else
                    {
                        StringCbPrintfA(szText, sizeof(szText),
                            "\x02 %04u.%02u.%02u %02u:%02u:%02u \x02 Page:%01u/%01u \x02",
                            st.wYear, st.wMonth, st.wDay,
                            st.wHour, st.wMinute, st.wSecond,
                            iPage + 1, m_cPages);
                    }
                    m_renderer.drawString(hDC, szText, x, y, cxChar, cyChar);
                }
                y += cyChar;

                INT cxColumn = 0;
                for (x = rcPrintArea.left; x < rcPrintArea.right; x += cxColumn + cxMargin)
                {
                    cxColumn = 0;
                    for (y = rcPrintArea.top + cyHeader + cyMargin;
                         y < rcPrintArea.bottom && i < bin.size();
                         y += cy + cyMargin)
                    {
                        INT cxSpace = rcPrintArea.right - x - cxMargin;
                        INT cySpace = rcPrintArea.bottom - y;
                        float space_x = m_helper.InchesFromPixelsX(hDC, cxSpace);
                        float space_y = m_helper.InchesFromPixelsY(hDC, cySpace);
                        float space = std::min(space_x, space_y);
                        size_t bytes;
                        if (bin.size() - i < nMaxQRData)
                            bytes = bin.size() - i;
                        else
                            bytes = nMaxQRData;

                        int qr_width = qr_width_from_bytes(bytes);
                        INT width = m_helper.PixelsFromInchesX(hDC, qr_width * m_settings.eDotSize);
                        INT height = m_helper.PixelsFromInchesY(hDC, qr_width * m_settings.eDotSize);
                        cx = LONG(width);
                        cy = LONG(height);
                        if (qr_width == 0 || bytes <= nMeTaLen)
                        {
                            break;
                        }

                        while (x + cx > rcPrintArea.right || y + cy > rcPrintArea.bottom ||
                               m_helper.InchesFromPixelsX(hDC, cx) / qr_width < m_settings.eDotSize ||
                               m_helper.InchesFromPixelsY(hDC, cy) / qr_width < m_settings.eDotSize)
                        {
                            if (bytes < 17 || bytes <= nMeTaLen)
                                break;
                            if (bytes > 2000)
                                bytes -= 116;
                            else if (bytes > 1000)
                                bytes -= 74;
                            else if (bytes > 200)
                                bytes -= 38;
                            else if (bytes > 15)
                                bytes -= 15;
                            if (bytes < 17 || bytes <= nMeTaLen)
                                break;
                            qr_width = qr_width_from_bytes(bytes);
                            if (qr_width == 0)
                            {
                                break;
                            }
                            width = m_helper.PixelsFromInchesX(hDC, qr_width * m_settings.eDotSize);
                            height = m_helper.PixelsFromInchesY(hDC, qr_width * m_settings.eDotSize);
                            cx = LONG(width);
                            cy = LONG(height);
                        }
                        if (qr_width == 0 || bytes <= nMeTaLen)
                        {
                            break;
                        }

                        cxColumn = std::max(cx, cxColumn);

                        if (x + cx > rcPrintArea.right)
                        {
                            break;
                        }
                        if (y + cy > rcPrintArea.bottom)
                        {
                            break;
                        }

                        INT cxQR, cyQR;
                        std::string str(data, bytes);
                        StringCbPrintfA(szMeTa, sizeof(szMeTa), "MeTa>%02u>%02u>%s|",
                                        iPart, m_cParts, hash.c_str());
                        str.insert(0, std::string(szMeTa));

                        if (HBITMAP hbm = qr_create_bitmap(hwnd, &str[0], str.size(), cxQR, cyQR))
                        {
                            if (pszDocName)
                            {
                                if (HDC hdcMem = CreateCompatibleDC(NULL))
                                {
                                    HGDIOBJ hbmOld = SelectObject(hdcMem, hbm);
                                    {
                                        StretchBlt(hDC, x, y, cx, cy,
                                                   hdcMem, 0, 0, cxQR, cyQR, SRCCOPY);
                                        m_renderer.drawString(hDC, "BLAKER",
                                            x, y - cyChar / 2, cxChar / 2, cyChar / 2);
                                    }
                                    SelectObject(hdcMem, hbmOld);
                                    DeleteDC(hdcMem);
                                }
                            }
                            DeleteObject(hbm);
                        }
                        else
                        {
                            ErrorBoxDx(IDS_CANTGENQRCODE);
                            return 0;
                        }

                        ++iPart;
                        i += bytes;
                        data += bytes;
                    }
                }

                if (pszDocName)
                {
                    ::EndPage(hDC);
                }

                ++iPage;
                if (iPage >= MAX_PAGES)
                {
                    ErrorBoxDx(IDS_EXCEEDMAXPAGE);
                    iPage = 0;
                    break;
                }
            }
            m_cParts = iPart - 1;
        }
        break;
    default:
        break;
    }

    m_cPages = iPage;
    return iPage > 0;
}

extern "C"
INT APIENTRY WinMain(
    HINSTANCE   hInstance,
    HINSTANCE   hPrevInstance,
    LPSTR       lpCmdLine,
    INT         nCmdShow)
{
    int ret = -1;

    {
        MBlakerApp app(hInstance);

        ::InitCommonControls();
        ::OleInitialize(NULL);
        ::AVIFileInit();

        if (app.StartDx(nCmdShow))
        {
            ret = app.RunDx();
        }

        ::AVIFileExit();
        ::OleUninitialize();
    }

#if (WINVER >= 0x0500)
    HANDLE hProcess = GetCurrentProcess();
    DebugPrintDx(TEXT("Count of GDI objects: %ld\n"),
                 GetGuiResources(hProcess, GR_GDIOBJECTS));
    DebugPrintDx(TEXT("Count of USER objects: %ld\n"),
                 GetGuiResources(hProcess, GR_USEROBJECTS));
#endif

#if defined(_MSC_VER) && !defined(NDEBUG)
    // for detecting memory leak (MSVC only)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    return ret;
}
