#include "stdafx.h"
#include "CGestureMatcher.h"
#include <glm/glm.hpp>
#include "GestureTest.h"

void CGestureMatcher::GetGestures(const LEAP_HAND* f_hand, std::map<HandGesture, bool>& gestures, std::vector<float>& values, const LEAP_HAND* f_oppHand) {

	static std::map<HandGesture, bool> static_gesture;

	std::vector<HandGesture> detect_gestures = {
			HG_Open,
			HG_EmptyHold,
			HG_SolidHold,
			HG_Point,
	};
	
	if (static_gesture.size() == 0) {
		for (int i = 0; i < detect_gestures.size(); i++) {
			static_gesture[detect_gestures[i]] = false;
		}

	}

	values.resize(HGS_MAX);


	//printf("f_hand %p %_oppHand %p\n", f_hand, f_oppHand);
	//static long count = 0;
	//if (count++ < 1000) {
	//	return;
	//}

	if (f_hand) {

		// 定义同类手势，互斥
		std::map<HandGesture, std::vector<HandGesture>> same_gestures = {
			{HG_EmptyHold,{HG_Point}},
			{HG_SolidHold,{HG_Point}},
			{HG_Point,{HG_EmptyHold, HG_SolidHold}},
		};

		//printf("[gestures] HG_Open %d HG_EmptyHold %d HG_SolidHold %d HGS_Hold %f\n", open_test, emptyhold_test, solidhold_test, hold_value);

		

		for (int i = 0; i < detect_gestures.size(); i++) {

			HandGesture gesture = detect_gestures[i];
			bool test = GestureTest(f_hand, gesture);
			if (gesture == HG_Open) {
				if (test) {
					for (int i = 0; i < detect_gestures.size(); i++) {
						static_gesture[detect_gestures[i]] = false;
					}
				}
			}
			else {

				if (test) {
					// 检查互斥手势是否存在，存在的话，就跳过识别
					bool mutex = false;
					for (HandGesture gesture : same_gestures[gesture]) {
						if (static_gesture[gesture]) {
							mutex = true;
							break;
						}
					}
					if (!mutex)
						static_gesture[gesture] = true;
				}
			}
		}

		// 互斥手势
		if (static_gesture[HG_SolidHold])
			static_gesture[HG_EmptyHold] = false;


		values[HGS_Hold] = GestureValue(f_hand, HGS_Hold);

		if (static_gesture[HG_Point])
			values[HGS_Trigger] = GestureValue(f_hand, HGS_Trigger);
		
	}

	gestures = static_gesture;

	
	

}