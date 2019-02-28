#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

struct BLAKER_SETTINGS
{
    float eDotDensity;
    float eMovieDelay;

    void reset()
    {
        eDotDensity = 60;       // 60 dpi
        eMovieDelay = 400;      // 400 milliseconds
    }
};

#endif  // ndef SETTINGS_HPP_
