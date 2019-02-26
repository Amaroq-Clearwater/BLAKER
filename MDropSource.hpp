// MDropSource.hpp
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>
// This file is public domain software.

#ifndef MDROPSOURCE_HPP_
#define MDROPSOURCE_HPP_

#define COBJMACROS
#include <windows.h>
#include <oleidl.h>     // for IDropSource, IDataObject
#include <vector>
#include <string>

class MDropSource : public IDropSource
{
public:
    static IDropSource *CreateInstance();

private:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    HRESULT STDMETHODCALLTYPE QueryContinueDrag(WINBOOL fEscapePressed, DWORD grfKeyState);
    HRESULT STDMETHODCALLTYPE GiveFeedback(DWORD dwEffect);

    MDropSource();
    virtual ~MDropSource();

protected:
    LONG m_nRefCount;
};

IDataObject *GetFileDataObject(const std::vector<std::wstring>& files);

#endif  // ndef MDROPSOURCE_HPP_
