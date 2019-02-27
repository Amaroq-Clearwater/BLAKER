#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

struct BLAKER_SETTINGS
{
    float eDotSize;
    float eMovieDelay;

    void reset()
    {
        eDotSize = 0.018;       // 0.018 inches
        eMovieDelay = 0.540;    // 0.540 seconds
    }
};

#endif  // ndef SETTINGS_HPP_
