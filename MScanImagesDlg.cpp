#include <shlwapi.h>
#include "MScanImagesDlg.hpp"
#include "resource.h"

MScanImagesDlg::MScanImagesDlg()
{
}

MScanImagesDlg::~MScanImagesDlg()
{
}

BOOL MScanImagesDlg::IsImageFile(LPCWSTR pszFile)
{
    LPWSTR pchDotExt = PathFindExtensionW(pszFile);
    if (*pchDotExt != L'.')
        return FALSE;

    LPWSTR pszzDotExts = MakeFilterDx(LoadStringDx(IDS_IMGDOTEXTS));

    for (LPWSTR pch = pszzDotExts; *pch; pch += lstrlenW(pch) + 1)
    {
        if (lstrcmpiW(pch, pchDotExt) == 0 &&
            GetFileAttributesW(pszFile) != 0xFFFFFFFF &&
            !(GetFileAttributesW(pszFile) & FILE_ATTRIBUTE_DIRECTORY))
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL MScanImagesDlg::OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    CenterWindowDx(hwnd);
    ::DragAcceptFiles(hwnd, TRUE);
    return TRUE;
}

void MScanImagesDlg::OnPsh1(HWND hwnd)
{
    OPENFILENAMEW ofn;
    WCHAR szFile[MAX_PATH] = L"";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = MakeFilterDx(LoadStringDx(IDS_IMGFILEFILTER));
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = ARRAYSIZE(szFile);
    ofn.lpstrTitle = LoadStringDx(IDS_CHOOSE_IMG);
    ofn.Flags = OFN_EXPLORER | OFN_ENABLESIZING | OFN_FILEMUSTEXIST |
                OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"jpg";
    if (GetOpenFileNameW(&ofn))
    {
        if (IsImageFile(szFile))
        {
            HWND hLst1 = GetDlgItem(hwnd, lst1);
            ListBox_AddString(hLst1, szFile);
        }
    }
}

void MScanImagesDlg::OnPsh2(HWND hwnd)
{
    HWND hLst1 = GetDlgItem(hwnd, lst1);
    INT i = ListBox_GetCurSel(hLst1);
    if (i == LB_ERR)
        return;

    ListBox_DeleteString(hLst1, i);
}

void MScanImagesDlg::OnPsh3(HWND hwnd)
{
    HWND hLst1 = GetDlgItem(hwnd, lst1);
    INT i = ListBox_GetCurSel(hLst1);
    if (i == LB_ERR || i == 0)
        return;

    WCHAR szText1[MAX_PATH];
    WCHAR szText2[MAX_PATH];
    ListBox_GetText(hLst1, i - 1, szText1);
    ListBox_GetText(hLst1, i, szText2);

    ListBox_DeleteString(hLst1, i - 1);
    ListBox_DeleteString(hLst1, i - 1);

    ListBox_InsertString(hLst1, i - 1, szText1);
    ListBox_InsertString(hLst1, i - 1, szText2);

    ListBox_SetCurSel(hLst1, i - 1);
}

void MScanImagesDlg::OnPsh4(HWND hwnd)
{
    HWND hLst1 = GetDlgItem(hwnd, lst1);
    INT i = ListBox_GetCurSel(hLst1);
    INT count = ListBox_GetCount(hLst1);
    if (i == LB_ERR || i + 1 == count)
        return;

    WCHAR szText1[MAX_PATH];
    WCHAR szText2[MAX_PATH];
    ListBox_GetText(hLst1, i, szText1);
    ListBox_GetText(hLst1, i + 1, szText2);

    ListBox_DeleteString(hLst1, i);
    ListBox_DeleteString(hLst1, i);

    ListBox_InsertString(hLst1, i, szText1);
    ListBox_InsertString(hLst1, i, szText2);

    ListBox_SetCurSel(hLst1, i + 1);
}

void MScanImagesDlg::OnDblClk(HWND hwnd)
{
    HWND hLst1 = GetDlgItem(hwnd, lst1);
    UINT i = ListBox_GetCurSel(hLst1);
    if (i == LB_ERR)
        return;

    WCHAR szPath[MAX_PATH];
    ListBox_GetText(hLst1, i, szPath);

    ShellExecute(hwnd, NULL, szPath, NULL, NULL, SW_SHOWNORMAL);
}

void MScanImagesDlg::OnOK(HWND hwnd)
{
    HWND hLst1 = GetDlgItem(hwnd, lst1);
    INT i, count = ListBox_GetCount(hLst1);
    if (count <= 0)
    {
        ErrorBoxDx(IDS_NOIMAGES);
        return;
    }

    WCHAR szPath[MAX_PATH];
    std::vector<std::wstring> files;
    for (i = 0; i < count; ++i)
    {
        ListBox_GetText(hLst1, i, szPath);
        files.push_back(szPath);
    }

    std::swap(m_files, files);
    EndDialog(hwnd, IDOK);
}

void MScanImagesDlg::OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
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
    case psh2:
        OnPsh2(hwnd);
        break;
    case psh3:
        OnPsh3(hwnd);
        break;
    case psh4:
        OnPsh4(hwnd);
        break;
    case lst1:
        switch (codeNotify)
        {
        case LBN_DBLCLK:
            OnDblClk(hwnd);
            break;
        }
    }
}

void MScanImagesDlg::OnDropFiles(HWND hwnd, HDROP hdrop)
{
    HWND hLst1 = GetDlgItem(hwnd, lst1);
    WCHAR szPath[MAX_PATH];
    UINT i, count = DragQueryFileW(hdrop, 0xFFFFFFFF, NULL, 0);
    for (i = 0; i < count; ++i)
    {
        DragQueryFileW(hdrop, i, szPath, ARRAYSIZE(szPath));
        if (IsImageFile(szPath))
        {
            ListBox_AddString(hLst1, szPath);
        }
    }

    DragFinish(hdrop);
}
