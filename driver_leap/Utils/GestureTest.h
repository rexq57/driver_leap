#pragma once

#include <LeapC.h>
#include "CGestureMatcher.h"

enum Position {
	P_Thumb,
	P_Index,
	P_Middle,
	P_Ring,
	P_Pinky,
	P_Palm
};

LEAP_VECTOR GetVector(const LEAP_HAND* f_hand, Position pos);



bool GestureTest(const LEAP_HAND* f_hand, CGestureMatcher::HandGesture handGesture, const LEAP_HAND* f_oppHand);
float GestureValue(const LEAP_HAND* f_hand, CGestureMatcher::HandGestureSub handGesture, const LEAP_HAND* f_oppHand);