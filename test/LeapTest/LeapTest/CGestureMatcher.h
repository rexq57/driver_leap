#pragma once

#include <vector>
#include <LeapC.h>
#include <map>

class CGestureMatcher
{
    // static float NormalizeRange(float f_val, float f_min, float f_max);
public:
    enum HandGesture : size_t {

        HG_Open,

        HG_EmptyHold,
        HG_SolidHold,

        HG_Point,

        HG_MAX,
    };

    enum HandGestureSub : size_t {

        HGS_Hold,
        HGS_Trigger,

        HGS_MAX
    };

    static void GetGestures(const LEAP_HAND* f_hand, std::map<HandGesture, bool>& gestures, std::vector<float>& values, const LEAP_HAND* f_oppHand = nullptr);
};
