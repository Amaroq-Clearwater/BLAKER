#include "MScanningMovieDlg.hpp"
#include "resource.h"

MScanningMovieDlg::MScanningMovieDlg(MBlakerApp *app, LPCWSTR pszFile)
    : MDialogBase(IDD_SCANNING_MOVIE), m_hThread(NULL), m_info(NULL)
{
    PROGRESS_INFO *info = new PROGRESS_INFO;
    info->app = app;
    info->dialog = this;
    info->pszFile = pszFile;
    m_info = info;
}

MScanningMovieDlg::~MScanningMovieDlg()
{
    ::CloseHandle(m_hThread);
}

BOOL MScanningMovieDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    CenterWindowDx(hwnd);

    PostMessage(hwnd, WM_COMMAND, IDYES, 0);

    return TRUE;
}

void MScanningMovieDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
        SendDlgItemMessage(hwnd, ctl1, PBM_SETPOS, 100, 0);
        SetDlgItemText(hwnd, stc1, LoadStringDx(IDS_DONE));
        EndDialog(hwnd, id);
        break;
    case IDCANCEL:
        m_info->bCancelled = TRUE;
        EndDialog(hwnd, id);
        break;
    case IDYES:
        JustDoIt(hwnd);
        break;
    }
}

void MScanningMovieDlg::OnTimer(HWND hwnd, UINT id)
{
    if (!m_info)
    {
        KillTimer(hwnd, 999);
        return;
    }
    SetDlgItemText(hwnd, stc1, LoadStringPrintfDx(IDS_PROGRESSING, m_info->nCount));
    SendDlgItemMessage(hwnd, ctl1, PBM_STEPIT, 0, 0);
}

static unsigned __stdcall the_thread_function(void *arg)
{
    PROGRESS_INFO *info = (PROGRESS_INFO *)arg;

    info->bCancelled = FALSE;
    info->nCount = 0;

    info->app->DoScanMovie(*info->app, info->pszFile, info);

    KillTimer(*info->dialog, 999);
    ::PostMessage(*info->dialog, WM_COMMAND, IDOK, 0);

    delete info;

    _endthreadex(0);

    return 0;
}

BOOL MScanningMovieDlg::JustDoIt(HWND hwnd)
{
    m_hThread = (HANDLE)_beginthreadex(NULL, 0, the_thread_function, m_info, 0, NULL);
    if (m_hThread)
    {
        ::SetTimer(hwnd, 999, 250, NULL);
        return TRUE;
    }
    else
    {
        ErrorBoxDx(L"_beginthreadex");
        return FALSE;
    }
}
