#pragma once

class CControlMatcher
{
    // static float NormalizeRange(float f_val, float f_min, float f_max);
public:
    enum Control : size_t
    {
        // Finger bends
        CT_KeyDown_A = 0U,

        CT_KeyDown_B,

        CT_ThumbPress,
        CT_ThumbPressX,
        CT_ThumbPressY,

        CT_Count
    };

    static void GetGestures(const LEAP_HAND *f_hand, std::vector<float> &f_gestures, std::vector<float> &f_result);
};
