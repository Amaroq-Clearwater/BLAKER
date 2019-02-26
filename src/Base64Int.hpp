// Base64Int.hpp --- base64 integer
// Copyright (C) 2019 Katayama Hirofumi MZ <katayama.hirofumi.mz@gmail.com>.
// This file is public domain software.

#ifndef BASE64INT_HPP_
#define BASE64INT_HPP_

#include "base64.hpp"
#include <cstdio>
#include <cassert>

inline std::string
Base64Int_Encode(unsigned int number)
{
    std::string ret;
    const char *table = base64_table();
    while (number)
    {
        ret += table[number % 64];
        number /= 64;
    }
    std::reverse(ret.begin(), ret.end());
    if (ret.empty())
        ret = "A";
    return ret;
}

inline unsigned int
Base64Int_Decode(const std::string& encoded)
{
    unsigned int ret = 0;;
    const char *table = base64_table();
    for (size_t i = 0; i < encoded.size(); ++i)
    {
        unsigned char index = base64_index(encoded[i]);
        if (index == 0xFF)
            return 0;
        ret *= 64;
        ret += index;
    }
    return ret;
}

inline bool
Base64Int_TestEntry(unsigned int number, const std::string& expected)
{
    std::string encoded = Base64Int_Encode(number);
    int decoded = Base64Int_Decode(encoded);
    if (decoded != number)
    {
        printf("FAILED --- %d, encoded '%s', decoded '%s'\n",
               number, encoded.c_str(), decoded);
        fflush(stdout);
        assert(0);
        return false;
    }
    if (encoded != expected)
    {
        printf("FAILED --- %d, encoded '%s', expected '%s'\n",
               number, encoded.c_str(), expected.c_str());
        fflush(stdout);
        assert(0);
        return false;
    }
    return true;
}

inline bool
Base64Int_UnitTest(void)
{
    return
        Base64Int_TestEntry(0, "A") &&
        Base64Int_TestEntry(1, "B") &&
        Base64Int_TestEntry(63, "/") &&
        Base64Int_TestEntry(64, "BA") &&
        Base64Int_TestEntry(80, "BQ") &&
        Base64Int_TestEntry(100, "Bk") &&
        Base64Int_TestEntry(1000, "Po") &&
        Base64Int_TestEntry(10000, "CcQ") &&
        Base64Int_TestEntry(999999, "D0I/") &&
        Base64Int_TestEntry(0xFFFF, "P//") &&
        Base64Int_TestEntry(0x10000, "QAA") &&
        Base64Int_TestEntry(0x7FFFFFFF, "B/////") &&
        Base64Int_TestEntry(0xFFFFFFFF, "D/////");
}

#endif  // ndef BASE64INT_HPP_
