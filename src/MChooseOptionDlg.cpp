#include "MChooseOptionDlg.hpp"

MChooseOptionDlg::MChooseOptionDlg()
{
}

MChooseOptionDlg::~MChooseOptionDlg()
{
}

BOOL MChooseOptionDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    CenterWindowDx(hwnd);
    return TRUE;
}

void MChooseOptionDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
    case IDCLOSE:
    case psh1:
    case psh2:
    case psh3:
        EndDialog(hwnd, id);
        break;
    }
}
