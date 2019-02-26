#include "MStartScreenDlg.hpp"
#include "resource.h"

MStartScreenDlg::MStartScreenDlg() : MDialogBase(IDD_START_SCREEN)
{
}

MStartScreenDlg::~MStartScreenDlg()
{
}

BOOL MStartScreenDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    CenterWindowDx(hwnd);

    return TRUE;
}

void MStartScreenDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    }
}
