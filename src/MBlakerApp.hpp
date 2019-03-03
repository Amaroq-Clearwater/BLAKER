#ifndef MBLAKER_PAPERS_APP_HPP_
#define MBLAKER_PAPERS_APP_HPP_

#include "MWindowBase.hpp"
#include "MPrintHelperEx.hpp"
#include "MBlakerScreen.hpp"
#include "Renderer.hpp"
#include "katahiromz_pdfium.h"
#include "QRCode.hpp"
#include "tbz.hpp"
#include "Settings.hpp"

class MScanningMovieDlg;

struct PROGRESS_INFO
{
    MBlakerApp *        app;
    MScanningMovieDlg * dialog;
    BOOL                bCancelled;
    INT                 nCount;
    LPCWSTR             pszFile;

    PROGRESS_INFO()
    {
        app = NULL;
        dialog = NULL;
        bCancelled = FALSE;
        nCount = 0;
        pszFile = NULL;
    }
};

class MBlakerApp : public MWindowBase
{
public:
    enum METHOD
    {
        METHOD_QRCODE,
        METHOD_QRCODE_META,
        METHOD_BASE64,
    };

    BOOL AddListViewColumns();
    BOOL ReCreateImageList();

    BOOL CreateTempDir(std::wstring& full_path);

    MBlakerApp(HINSTANCE hInst);
    virtual ~MBlakerApp();

    virtual LPCTSTR GetWndClassNameDx() const;
    virtual VOID ModifyWndClassDx(WNDCLASSEX& wcx);

    BOOL StartDx(INT nCmdShow);
    INT RunDx();

    INT AddFileItem(HWND hwnd, const FILE_ITEM& file_item);
    BOOL DoLoad(HWND hwnd, const wchar_t *pszFile);
    BOOL DoSave(HWND hwnd, const wchar_t *pszFile, const std::string& binary);
    BOOL DoLoadPdfium(HWND hwnd, katahiromz_pdfium& pdfium);
    BOOL DoScanPDF(HWND hwnd, LPCWSTR pszPdfFile);
    BOOL DoScanImages(HWND hwnd, const std::vector<std::wstring>& files);
    BOOL DoScanBinaries(HWND hwnd, std::vector<std::string>& binaries);
    BOOL DoScanMovie(HWND hwnd, LPCWSTR pszMovieFile, PROGRESS_INFO *info);
    BOOL DoPrintPages(HWND hwnd, HDC hDC, LPCWSTR pszDocName);
    BOOL DoScreenImages(HWND hwnd);
    BOOL DoScreenMovie(HWND hwnd);
    INT DoCreateImages(HWND hwnd, std::vector<HBITMAP>& bitmaps,
                       SIZE sizImage, BOOL bTest);
    BOOL DoDestroyImages(std::vector<HBITMAP>& bitmaps);

    BOOL DoLoadSettings(BLAKER_SETTINGS& settings);
    BOOL DoSaveSettings(const BLAKER_SETTINGS& settings);
    BOOL DoContextMenu(HWND hwnd, POINT pt);

    virtual LRESULT CALLBACK
    WindowProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
            HANDLE_MSG(hwnd, WM_ACTIVATE, OnActivate);
            HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
            HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
            HANDLE_MSG(hwnd, WM_SIZE, OnSize);
            HANDLE_MSG(hwnd, WM_INITMENUPOPUP, OnInitMenuPopup);
            HANDLE_MSG(hwnd, WM_DROPFILES, OnDropFiles);
            HANDLE_MSG(hwnd, WM_NOTIFY, OnNotify);
            HANDLE_MSG(hwnd, WM_CONTEXTMENU, OnContextMenu);
        default:
            return DefaultProcDx(hwnd, uMsg, wParam, lParam);
        }
        return 0;
    }

protected:
    HINSTANCE   m_hInst;        // the instance handle
    HICON       m_hIcon;        // the main icon
    HICON       m_hIconSm;      // the small icon
    HACCEL      m_hAccel;       // the access keys
    HWND        m_hMainWnd;
    HWND        m_hListView;
    HWND        m_hStatusWnd;
    HIMAGELIST  m_hImageList;
    INT         m_nCommandExecCount;
    BINS        m_bins;
    METHOD      m_method;
    INT         m_cParts;
    INT         m_cPages;
    BOOL        m_bMeTa;
    Renderer    m_renderer;
    std::vector<std::wstring>   m_temp_dirs;
    MPrintHelperEx              m_helper;
    BLAKER_SETTINGS             m_settings;

    BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnDestroy(HWND hwnd);
    void OnOpen(HWND hwnd);
    void OnScanPDF(HWND hwnd);
    void OnScanImages(HWND hwnd);
    void OnSaveAs(HWND hwnd);
    void OnPrint(HWND hwnd);
    void OnSize(HWND hwnd, UINT state, int cx, int cy);
    void OnInitMenuPopup(HWND hwnd, HMENU hMenu, UINT item, BOOL fSystemMenu);
    void OnDropFiles(HWND hwnd, HDROP hdrop);
    void OnScreen(HWND hwnd);
    void OnScanMovie(HWND hwnd);
    void OnNew(HWND hwnd);
    void OnSelectAll(HWND hwnd);
    LRESULT OnNotify(HWND hwnd, int idFrom, LPNMHDR pnmhdr);
    void OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
    void OnBeginDrag(HWND hwnd);
    void OnRename(HWND hwnd);
    void OnDelete(HWND hwnd);
    void OnSaveSelection(HWND hwnd);
    void OnAbout(HWND hwnd);
    void OnOpenReadMe(HWND hwnd);
    void OnSettings(HWND hwnd);
    void OnContextMenu(HWND hwnd, HWND hwndContext, UINT xPos, UINT yPos);
    void OnCopy(HWND hwnd);
    void OnPaste(HWND hwnd);

    struct QR_CALLBACK_DATA
    {
        MBlakerApp *m_app;
        std::vector<std::string>& m_binaries;
        int& m_iPage;
        std::string m_hash;
        enum ErrorType
        {
            VALID,
            INVALID_FORMAT,
            INVALID_HASH,
            INCOMPLETE_DATA
        } m_error;
        BOOL& m_bMeTa;

        QR_CALLBACK_DATA(std::vector<std::string>& binaries, int& iPage, BOOL& bMeTa);
        bool operator()(zbar::Image::SymbolIterator& symbol, LPARAM lParam);
    };

    void EnterCommand(HWND hwnd);
    void LeaveCommand(HWND hwnd);
    void scanError(HWND hwnd, QR_CALLBACK_DATA::ErrorType error);
};

#endif  // ndef MBLAKER_PAPERS_APP_HPP_
