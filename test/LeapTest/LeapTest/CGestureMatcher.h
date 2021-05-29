#pragma once

#include <vector>
#include <LeapC.h>

class CGestureMatcher
{
    // static float NormalizeRange(float f_val, float f_min, float f_max);
public:
    enum HandGesture : size_t {

        HG_Open,

        HG_EmptyHold,
        HG_SolidHold,
    };

    static void GetGestures(const LEAP_HAND* f_hand, std::vector<float>& f_result, const LEAP_HAND* f_oppHand = nullptr);
};
#pragma once
