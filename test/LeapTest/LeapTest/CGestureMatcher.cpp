#include "stdafx.h"
#include "CGestureMatcher.h"
#include <glm/glm.hpp>
#include "GestureTest.h"

void CGestureMatcher::GetGestures(const LEAP_HAND* f_hand, std::map<HandGesture, bool>& gestures, std::vector<float>& values, const LEAP_HAND* f_oppHand) {

	static std::map<HandGesture, bool> static_lr_gestures[2];
	static std::vector<float> static_lr_values[2];


	//printf("f_hand %p %_oppHand %p\n", f_hand, f_oppHand);
	//static long count = 0;
	//if (count++ < 1000) {
	//	return;
	//}

	if (f_hand) {

		std::map<HandGesture, bool>& static_gesture = static_lr_gestures[ f_hand->type ];
		std::vector<float>& static_value = static_lr_values[ f_hand->type ];

		std::vector<HandGesture> detect_gestures = {
				HG_Open,
				HG_EmptyHold,
				HG_SolidHold,
				HG_Point,
				HG_IndexTouch,
				HG_PinkyTouch
		};

		if (static_gesture.size() == 0) {
			for (int i = 0; i < detect_gestures.size(); i++) {
				static_gesture[detect_gestures[i]] = false;
			}
		}

		if (static_value.size() == 0) {
			static_value.resize(HGS_MAX);
			for (int i = 0; i < static_value.size(); i++) {
				static_value[i] = 0.0f;
			}
		}

		// ����ͬ�����ƣ�����
		std::map<HandGesture, std::vector<HandGesture>> same_gestures = {
			{HG_EmptyHold,{HG_SolidHold}},
			{HG_SolidHold,{HG_EmptyHold}},
			{HG_Point,{HG_SolidHold}},
		};

		//printf("[gestures] HG_Open %d HG_EmptyHold %d HG_SolidHold %d HGS_Hold %f\n", open_test, emptyhold_test, solidhold_test, hold_value);

		for (int i = 0; i < detect_gestures.size(); i++) {

			HandGesture gesture = detect_gestures[i];
			bool test = GestureTest(f_hand, gesture, f_oppHand);
			if (gesture == HG_Open) {
				if (test) {
					for (auto it : static_gesture) {
						static_gesture[it.first] = false;
					}
					for (int i = 0; i < static_value.size(); i++) {
						static_value[i] = 0.0f;
					}

					printf("reset\n");
				}
			}
			else {

				if (test) {
					// ��黥�������Ƿ���ڣ����ڵĻ���������ʶ��
					auto& same = same_gestures[gesture];
					bool mutex = false;
					for (HandGesture gesture : detect_gestures) {
						if (static_gesture[gesture] && std::find(same.begin(), same.end(), gesture) == same.end()) {
							mutex = true;
							break;
						}
					}
					if (!mutex)
						static_gesture[gesture] = true;

					// �������� clear hold
					if (gesture == HG_Point) {
						static_gesture[HG_SolidHold] = false;
					}
				}
			}
		}

		// ��������
		if (static_gesture[HG_SolidHold])
			static_gesture[HG_EmptyHold] = false;


		static_value[HGS_Hold] = GestureValue(f_hand, HGS_Hold, f_oppHand);
		static_value[HGS_PalmTouch] = GestureValue(f_hand, HGS_PalmTouch, f_oppHand);

		if (static_gesture[HG_Point])
			static_value[HGS_Trigger] = GestureValue(f_hand, HGS_Trigger, f_oppHand);

		// ˫������
		if (f_oppHand) {

			std::map<HandGesture, bool>& static_gesture2 = static_lr_gestures[f_oppHand->type];
			std::vector<float>& static_value2 = static_lr_values[f_oppHand->type];
			if (static_value2.size() == 0) {
				static_value2.resize(HGS_MAX);
				for (int i = 0; i < static_value2.size(); i++) {
					static_value2[i] = 0.0f;
				}
			}

			// ����ʳָ���� + ����ָ��
			if (static_gesture2[HG_EmptyHold]) {
				
				if (static_gesture[HG_Point]) {

					// ȡ��������������Ϊ�ƶ���
					auto vec = GetVector(f_oppHand, P_Palm);
					glm::vec3 l_start(vec.x, vec.y, vec.z);
					
					if (static_value2[__HGS_TrackpadX] == 0.0f && static_value2[__HGS_TrackpadY] == 0.0f) {

						// printf("�������� \n");

						static_value2[__HGS_TrackpadX] = l_start.x;
						static_value2[__HGS_TrackpadY] = l_start.y;
					}
					else {
						// // ���������ĵ�ĽǶ�
						const float length = 25.0f;
						glm::vec2 l_uv(-(l_start.x - static_value2[__HGS_TrackpadX]), l_start.y - static_value2[__HGS_TrackpadY]);

						l_uv /= 25.0f;

						glm::vec2 old_uv = l_uv;

						if (glm::length(l_uv) > 1.f)
							l_uv = glm::normalize(l_uv);

						static_value2[__HGS_TrackpadX2] = l_uv.x;
						static_value2[__HGS_TrackpadY2] = l_uv.y;
					}

					// ���ֿ۶������Ϊ��������
					static_value[HGS_TrackpadClick] = static_value[HGS_Trigger] >= 1.0f;

					// ���ø�������
					static_gesture2[__HG_Disable] = true;
				}
			}

			// ����ʵ���� + ����ָ��
			if (static_gesture2[HG_SolidHold]) {

				if (static_gesture[HG_Point]) {

					// ȡ��������������Ϊ�ƶ���
					auto vec = GetVector(f_oppHand, P_Palm);
					glm::vec3 l_start(vec.x, vec.y, vec.z);

					if (static_value2[__HGS_ThumbstickX] == 0.0f && static_value2[__HGS_ThumbstickY] == 0.0f) {

						// printf("�������� \n");

						static_value2[__HGS_ThumbstickX] = l_start.x;
						static_value2[__HGS_ThumbstickY] = l_start.y;
					}
					else {
						// // ���������ĵ�ĽǶ�
						const float length = 25.0f;
						glm::vec2 l_uv(-(l_start.x - static_value2[__HGS_ThumbstickX]), l_start.y - static_value2[__HGS_ThumbstickY]);

						l_uv /= 35.0f;

						glm::vec2 old_uv = l_uv;

						if (glm::length(l_uv) > 1.f)
							l_uv = glm::normalize(l_uv);

						static_value2[__HGS_ThumbstickX2] = l_uv.x;
						static_value2[__HGS_ThumbstickY2] = l_uv.y;
					}
					
					// ���ֿ۶������Ϊ��������
					static_value[HGS_ThumbstickClick] = static_value[HGS_Trigger] >= 1.0f;

					// ���ø�������
					static_gesture2[__HG_Disable] = true;
				}
			}

			static_value[HGS_TrackpadX] = static_value2[__HGS_TrackpadX2];
			static_value[HGS_TrackpadY] = static_value2[__HGS_TrackpadY2];

			static_value[HGS_ThumbstickX] = static_value2[__HGS_ThumbstickX2];
			static_value[HGS_ThumbstickY] = static_value2[__HGS_ThumbstickY2];
		}

		gestures = static_gesture;
		values = static_value;

		// ��������ܻ���
		if (values[HGS_ThumbstickClick]) {
			values[HGS_Trigger] = 0.0f;
		}
	}
	else {
		if (gestures.size() == 0) {
			for (int i = 0; i < gestures.size(); i++) {
				gestures[(HandGesture)i] = false;
			}
		}

		if (values.size() == 0) {
			values.resize(HGS_MAX);
			for (int i = 0; i < values.size(); i++) {
				values[i] = 0.0f;
			}
		}
	}

	
	

}