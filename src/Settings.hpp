#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

struct BLAKER_SETTINGS
{
    float eDotSize;
    float eMovieDelay;

    void reset()
    {
        eDotSize = 0.018;       // 0.018 inches == 0.46mm
        eMovieDelay = 0.500;    // 0.500 seconds
    }
};

#endif  // ndef SETTINGS_HPP_
