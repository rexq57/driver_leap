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

        HG_IndexTouch,
        HG_PinkyTouch,

        __HG_Disable, // 副手手势禁止标志，表示当前手势被用作双手手势

        HG_MAX,
    };

    enum HandGestureSub : size_t {

        HGS_Hold,
        HGS_Trigger,
        HGS_IndexContact,

        HGS_TrackpadX,
        HGS_TrackpadY,

        HGS_TrackpadClick,

        HGS_ThumbstickX,
        HGS_ThumbstickY,

        HGS_ThumbstickTouch,
        HGS_ThumbstickClick,

        __HGS_LAST,
        
        __HGS_Hold,
        __HGS_Trigger,
        __HGS_IndexContact,

        __HGS_TrackpadX,
        __HGS_TrackpadY,

        __HGS_TrackpadClick,

        __HGS_ThumbstickX,
        __HGS_ThumbstickY,

        __HGS_ThumbstickKeep,
        __HGS_DisableTrigger,

        HGS_MAX,
    };

    static void GetGestures(const LEAP_HAND* f_hand, std::map<HandGesture, bool>& gestures, std::vector<float>& values, const LEAP_HAND* f_oppHand = nullptr);
};
