#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

struct BLAKER_SETTINGS
{
    float eDotSize;

    void reset()
    {
        eDotSize = 0.018;   // 0.018 inch
    }
};

#endif  // ndef SETTINGS_HPP_
