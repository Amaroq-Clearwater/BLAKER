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
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"120");
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"100");
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"75");
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"60");
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"40");
    SendDlgItemMessage(hwnd, cmb1, CB_ADDSTRING, 0, (LPARAM)L"20");

    SendDlgItemMessage(hwnd, cmb2, CB_ADDSTRING, 0, (LPARAM)L"75");
    SendDlgItemMessage(hwnd, cmb2, CB_ADDSTRING, 0, (LPARAM)L"100");
    SendDlgItemMessage(hwnd, cmb2, CB_ADDSTRING, 0, (LPARAM)L"200");
    SendDlgItemMessage(hwnd, cmb2, CB_ADDSTRING, 0, (LPARAM)L"300");
    SendDlgItemMessage(hwnd, cmb2, CB_ADDSTRING, 0, (LPARAM)L"400");
    SendDlgItemMessage(hwnd, cmb2, CB_ADDSTRING, 0, (LPARAM)L"500");

    SetDlgItemDouble(hwnd, cmb1, m_settings.eDotDensity, "%g");
    SetDlgItemDouble(hwnd, cmb2, m_settings.eMovieDelay, "%g");

    OnCmb1(hwnd);

    CenterWindowDx(hwnd);
    return TRUE;
}

void MSettingsDlg::OnOK(HWND hwnd)
{
    BOOL bTranslated;
    double eValue;

    float eDotDensity, eMovieDelay;

    bTranslated = FALSE;
    eValue = GetDlgItemDouble(hwnd, cmb1, &bTranslated);
    if (bTranslated && eValue > 0)
    {
        eDotDensity = (float)eValue;
    }
    else
    {
        SendDlgItemMessage(hwnd, cmb1, CB_SETEDITSEL, 0, MAKELPARAM(0, -1));
        ::SetFocus(::GetDlgItem(hwnd, cmb1));
        ErrorBoxDx(IDS_INVALID_VALUE);
        return;
    }

    bTranslated = FALSE;
    eValue = GetDlgItemDouble(hwnd, cmb2, &bTranslated);
    if (bTranslated && eValue > 0)
    {
        eMovieDelay = (float)eValue;
    }
    else
    {
        SendDlgItemMessage(hwnd, cmb2, CB_SETEDITSEL, 0, MAKELPARAM(0, -1));
        ::SetFocus(::GetDlgItem(hwnd, cmb2));
        ErrorBoxDx(IDS_INVALID_VALUE);
        return;
    }

    m_settings.eDotDensity = eDotDensity;
    m_settings.eMovieDelay = eMovieDelay;

    EndDialog(hwnd, IDOK);
}

void MSettingsDlg::OnPsh1(HWND hwnd)
{
    m_settings.reset();
    EndDialog(hwnd, IDOK);
}

void MSettingsDlg::OnCmb1(HWND hwnd)
{
    float eDotDensity;
    BOOL bTranslated;

    bTranslated = FALSE;
    eDotDensity = GetDlgItemDouble(hwnd, cmb1, &bTranslated);
    if (!bTranslated)
    {
        eDotDensity = 0;
    }

    if (eDotDensity > 0)
    {
        float eDotSize = 25.4 / eDotDensity;
        SetDlgItemText(hwnd, stc1, LoadStringPrintfDx(IDS_DOTSIZE, eDotSize));
    }
    else
    {
        SetDlgItemText(hwnd, stc1, NULL);
    }
}

void MSettingsDlg::OnCmb1SelEndOK(HWND hwnd)
{
    INT iSel = (INT)SendDlgItemMessage(hwnd, cmb1, CB_GETCURSEL, 0, 0);

    WCHAR szText[128];
    SendDlgItemMessage(hwnd, cmb1, CB_GETLBTEXT, iSel, (LPARAM)szText);

    float eDotDensity = wcstod(szText, NULL);
    if (eDotDensity > 0)
    {
        float eDotSize = 25.4 / eDotDensity;
        SetDlgItemText(hwnd, stc1, LoadStringPrintfDx(IDS_DOTSIZE, eDotSize));
    }
    else
    {
        SetDlgItemText(hwnd, stc1, NULL);
    }
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
    case cmb1:
        switch (codeNotify)
        {
        case CBN_EDITCHANGE:
            OnCmb1(hwnd);
            break;
        case CBN_SELENDOK:
            OnCmb1SelEndOK(hwnd);
            break;
        }
        break;
    }
}
