#include "stdafx.h"
#include "Utils/CControlMatcher.h"
#include "Utils/CGestureMatcher.h"

static std::vector<bool> last_result;

void CControlMatcher::GetGestures(std::vector<float> &f_gestures, std::vector<bool> &f_result) {

    if (last_result.size() == 0) {
        last_result.resize(CT_Count);
    }

    if (f_gestures[CGestureMatcher::HG_ThumbMiddleTouch] >= 0.85f && !last_result[CT_KeyDown_B]) {
        last_result[CT_KeyDown_A] = true;
    } else if (f_gestures[CGestureMatcher::HG_ThumbMiddleTouch] <= 0.15f){
        last_result[CT_KeyDown_A] = false;
    }

    if (f_gestures[CGestureMatcher::HG_ThumbPinkyTouch] >= 0.85f && !last_result[CT_KeyDown_A]) {
        last_result[CT_KeyDown_B] = true;
    } else if (f_gestures[CGestureMatcher::HG_ThumbPinkyTouch] <= 0.15f){
        last_result[CT_KeyDown_B] = false;
    }

    

    f_result = last_result;
}