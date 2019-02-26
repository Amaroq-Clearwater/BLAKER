#ifndef TEXT_TO_TEXT_HPP_
#define TEXT_TO_TEXT_HPP_

inline std::wstring AnsiToWide(const std::string& str)
{
    WCHAR szText[256];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, szText, 256);
    std::wstring ret = szText;
    return ret;
}

inline std::string WideToAnsi(const std::wstring& str)
{
    CHAR szText[256], ch = '_';
    WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, szText, 256, &ch, NULL);
    std::string ret = szText;
    return ret;
}

inline std::wstring Utf8ToWide(const std::string& str)
{
    WCHAR szText[256];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szText, 256);
    std::wstring ret = szText;
    return ret;
}

inline std::string WideToUtf8(const std::wstring& str)
{
    CHAR szText[256], ch = '_';
    WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, szText, 256, &ch, NULL);
    std::string ret = szText;
    return ret;
}

#endif  // ndef TEXT_TO_TEXT_HPP_
