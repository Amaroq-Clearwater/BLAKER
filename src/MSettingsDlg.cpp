#include "MSettingsDlg.hpp"
#include "SetDlgItemDouble.h"
#include "resource.h"
#include <strsafe.h>

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
    SetDlgItemDouble(hwnd, edt3, m_settings.eMovieDelay, "%.3f");

    CenterWindowDx(hwnd);
    return TRUE;
}

void MSettingsDlg::OnOK(HWND hwnd)
{
    BOOL bTranslated;
    double eValue;

    float eDotSize, eMovieDelay;

    bTranslated = FALSE;
    eValue = GetDlgItemDouble(hwnd, edt1, &bTranslated);
    if (bTranslated && eValue > 0)
    {
        eDotSize = (float)eValue;
    }
    else
    {
        SendDlgItemMessage(hwnd, edt1, EM_SETSEL, 0, -1);
        ::SetFocus(::GetDlgItem(hwnd, edt1));
        ErrorBoxDx(IDS_INVALID_VALUE);
        return;
    }

    bTranslated = FALSE;
    eValue = GetDlgItemDouble(hwnd, edt2, &bTranslated);
    if (bTranslated && eValue > 0)
    {
        eMovieDelay = (float)eValue;
    }
    else
    {
        SendDlgItemMessage(hwnd, edt2, EM_SETSEL, 0, -1);
        ::SetFocus(::GetDlgItem(hwnd, edt2));
        ErrorBoxDx(IDS_INVALID_VALUE);
        return;
    }

    m_settings.eDotSize = eDotSize;
    m_settings.eMovieDelay = eMovieDelay;

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
