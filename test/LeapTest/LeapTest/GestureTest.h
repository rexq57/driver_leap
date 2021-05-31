#pragma once

#include <LeapC.h>
#include "CGestureMatcher.h"

bool GestureTest(const LEAP_HAND* f_hand, CGestureMatcher::HandGesture handGesture);
float GestureValue(const LEAP_HAND* f_hand, CGestureMatcher::HandGestureSub handGesture);