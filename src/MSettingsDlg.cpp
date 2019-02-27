#include "MSettingsDlg.hpp"
#include <strsafe.h>
#include "resource.h"

MSettingsDlg::MSettingsDlg(BLAKER_SETTINGS& settings)
    : MDialogBase(IDD_SETTINGS), m_settings(settings)
{
}

MSettingsDlg::~MSettingsDlg()
{
}

BOOL MSettingsDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    WCHAR szText[64];
    StringCbPrintfW(szText, sizeof(szText), L"%.3f", m_settings.eDotSize);
    SetDlgItemTextW(hwnd, edt1, szText);

    CenterWindowDx(hwnd);
    return TRUE;
}

void MSettingsDlg::OnOK(HWND hwnd)
{
    WCHAR szText[64];
    GetDlgItemText(hwnd, edt1, szText, ARRAYSIZE(szText));

    WCHAR *endptr;
    double eValue = wcstod(szText, &endptr);
    if (*endptr == 0 && (float)eValue > 0)
    {
        m_settings.eDotSize = (float)eValue;
    }

    EndDialog(hwnd, IDOK);
}

void MSettingsDlg::OnPsh1(HWND hwnd)
{
    m_settings.reset();
    EndDialog(hwnd, IDOK);
}

void MSettingsDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
        OnOK(hwnd);
        break;
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    case psh1:
        OnPsh1(hwnd);
        break;
    }
}
