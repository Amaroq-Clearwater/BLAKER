#ifndef MPRINT_HELPER_EX_HPP_
#define MPRINT_HELPER_EX_HPP_

#include "MBlakerApp.hpp"
#include "MPrintHelper.hpp"

class MBlakerApp;

class MPrintHelperEx : public MPrintHelper
{
public:
    MPrintHelperEx(MBlakerApp *app);
    virtual ~MPrintHelperEx();
    virtual BOOL DoPrintPages(HWND hwnd, HDC hDC, LPCWSTR pszDocName);

protected:
    MBlakerApp *m_app;
};

#endif  // ndef MPRINT_HELPER_EX_HPP_
