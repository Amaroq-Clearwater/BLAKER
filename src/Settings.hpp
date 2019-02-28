#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

struct BLAKER_SETTINGS
{
    float eDotDensity;
    float eMovieDelay;

    void reset()
    {
        eDotDensity = 60;       // 60 dpi
        eMovieDelay = 0.5;      // 0.5 seconds
    }
};

#endif  // ndef SETTINGS_HPP_
