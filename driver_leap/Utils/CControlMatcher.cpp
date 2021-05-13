#include "stdafx.h"
#include "Utils/CControlMatcher.h"
#include "Utils/CGestureMatcher.h"

static std::vector<float> last_result;
static float last_ThumbPressX = 0;
static float last_ThumbPressY = 0;

void CControlMatcher::GetGestures(const LEAP_HAND *f_hand, std::vector<float> &f_gestures, std::vector<float> &f_result) {

    if (last_result.size() == 0) {
        last_result.resize(CT_Count);
    }

    if (f_gestures[CGestureMatcher::HG_ThumbMiddleTouch] >= 0.85f && !last_result[CT_KeyDown_B]) {
        last_result[CT_KeyDown_A] = 1.0f;
    } else if (f_gestures[CGestureMatcher::HG_ThumbMiddleTouch] <= 0.15f){
        last_result[CT_KeyDown_A] = 0.0f;
    }

    if (f_gestures[CGestureMatcher::HG_ThumbPinkyTouch] >= 0.85f && !last_result[CT_KeyDown_A]) {
        last_result[CT_KeyDown_B] = 1.0f;
    } else if (f_gestures[CGestureMatcher::HG_ThumbPinkyTouch] <= 0.15f){
        last_result[CT_KeyDown_B] = 0.0f;
    }

    if (f_gestures[CGestureMatcher::HG_ThumbPress] >= 0.75f) {

        glm::vec3 l_start(f_hand->thumb.distal.next_joint.x, f_hand->thumb.distal.next_joint.y, f_hand->thumb.distal.next_joint.z);
        
        if (last_result[CT_ThumbPress] == 0.0f) {
            last_ThumbPressX = l_start.x;
            last_ThumbPressY = l_start.y;
            // MTHLog("%f %f %f", l_start.x, l_start.y, l_start.z);
        }

        // // 计算与中心点的角度
        const float length = 25.0f;
        glm::vec2 l_uv(-(l_start.x - last_ThumbPressX), l_start.y - last_ThumbPressY);

        if (l_uv.x > length) {
            last_ThumbPressX = length + l_start.x;
        } else if (l_uv.x < -length) {
            last_ThumbPressX = -length + l_start.x;
        }
        if (l_uv.y > length) {
            last_ThumbPressY = l_start.y - length;
        } else if (l_uv.y < -length) {
            last_ThumbPressY = l_start.y + length;
        }

        

        l_uv /= 25.0f;

        glm::vec2 old_uv = l_uv;

        if(glm::length(l_uv) > 1.f)
            l_uv = glm::normalize(l_uv);

        last_result[CT_ThumbPressX] = l_uv.x;
        last_result[CT_ThumbPressY] = l_uv.y;

        // MTHLog("%f %f %f (%f %f)", glm::length(old_uv), old_uv.x, old_uv.y, l_uv.x, l_uv.y);

        last_result[CT_ThumbPress] = f_gestures[CGestureMatcher::HG_ThumbPress];

    } else if(f_gestures[CGestureMatcher::HG_ThumbPress] <= 0.25f) {
        last_result[CT_ThumbPress] = 0.0f;
    }


    f_result = last_result;
}