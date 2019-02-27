#include "MSettingsDlg.hpp"
#include <strsafe.h>
#include "SetDlgItemDouble.h"
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
    SetDlgItemDouble(hwnd, edt1, m_settings.eDotSize, "%.3f");

    CenterWindowDx(hwnd);
    return TRUE;
}

void MSettingsDlg::OnOK(HWND hwnd)
{
    BOOL bTranslated;
    double eValue;

    bTranslated = FALSE;
    eValue = GetDlgItemDouble(hwnd, edt1, &bTranslated);
    if (bTranslated && eValue > 0)
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
