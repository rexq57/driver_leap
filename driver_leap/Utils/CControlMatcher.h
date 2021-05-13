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

        CT_Count
    };

    static void GetGestures(std::vector<float> &f_gestures, std::vector<bool> &f_result);
};
