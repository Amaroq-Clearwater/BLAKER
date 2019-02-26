// MDropSource.cpp
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#include "MDropSource.hpp"
#include <shlobj.h>     // for SHGetDesktopFolder
#include <shobjidl.h>   // for IShellFolder
#include <strsafe.h>

MDropSource::MDropSource() : m_nRefCount(0)
{
}

MDropSource::~MDropSource()
{
}

IDropSource *MDropSource::CreateInstance()
{
    MDropSource *pDropSource = new MDropSource;
    pDropSource->AddRef();
    return pDropSource;
}

HRESULT STDMETHODCALLTYPE
MDropSource::QueryInterface(REFIID riid, void **ppvObject)
{
    OutputDebugString(TEXT("MDropSource::QueryInterface\n"));
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropSource))
    {
        *ppvObject = static_cast<IDropSource *>(this);
        AddRef();
        return S_OK;
    }
    else
    {
        *ppvObject = NULL;
        return E_NOINTERFACE;
    }
}

ULONG STDMETHODCALLTYPE
MDropSource::AddRef()
{
    OutputDebugString(TEXT("MDropSource::AddRef\n"));
    return ++m_nRefCount;
}

ULONG STDMETHODCALLTYPE
MDropSource::Release()
{
    OutputDebugString(TEXT("MDropSource::Release\n"));
    --m_nRefCount;
    if (m_nRefCount > 0)
        return m_nRefCount;
    delete this;
    return 0;
}

HRESULT STDMETHODCALLTYPE
MDropSource::QueryContinueDrag(WINBOOL fEscapePressed, DWORD grfKeyState)
{
    OutputDebugString(TEXT("MDropSource::QueryContinueDrag\n"));

    if (fEscapePressed)
        return DRAGDROP_S_CANCEL;

    if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON)))
        return DRAGDROP_S_DROP;

    return S_OK;
}

HRESULT STDMETHODCALLTYPE
MDropSource::GiveFeedback(DWORD dwEffect)
{
    OutputDebugString(TEXT("MDropSource::GiveFeedback\n"));
    return DRAGDROP_S_USEDEFAULTCURSORS;
}

IDataObject *GetFileDataObject(const std::vector<std::wstring>& files)
{
    HRESULT hr;
    IShellFolder *pDesktop = NULL;
    hr = SHGetDesktopFolder(&pDesktop);
    if (FAILED(hr))
        return NULL;

    std::vector<LPITEMIDLIST> pidls;
    UINT count = files.size();
    for (UINT i = 0; i < count ; ++i)
    {
        const std::wstring& file = files[i];
        LPITEMIDLIST pidl = NULL;
        WCHAR szName[MAX_PATH];
        StringCbCopyW(szName, sizeof(szName), file.c_str());
        hr = pDesktop->ParseDisplayName(HWND_DESKTOP, NULL, szName,
                                        NULL, &pidl, NULL);
        if (FAILED(hr))
        {
            break;
        }
        pidls.push_back(pidl);
    }

    IDataObject *pDataObject = NULL;
    if (SUCCEEDED(hr))
    {
        hr = pDesktop->GetUIObjectOf(HWND_DESKTOP, UINT(pidls.size()),
            (const ITEMIDLIST **)&pidls[0], IID_IDataObject, NULL,
            (void **)&pDataObject);
    }

    for (size_t i = 0; i < pidls.size(); ++i)
    {
        CoTaskMemFree(pidls[i]);
    }

    if (FAILED(hr) && pDataObject)
    {
        pDataObject->Release();
        pDataObject = NULL;
    }

    pDesktop->Release();
    return pDataObject;
}
