#include "MAboutDlg.hpp"
#include "resource.h"

MAboutDlg::MAboutDlg() : MDialogBase(IDD_ABOUT)
{
}

MAboutDlg::~MAboutDlg()
{
}

BOOL MAboutDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    CenterWindowDx(hwnd);

    return TRUE;
}

void MAboutDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    }
}
