#include "stdafx.h"
#include "CGestureMatcher.h"
#include <glm/glm.hpp>
#include "GestureTest.h"

float DistanceCenter(float x, float y) {
	return sqrtf(x * x + y * y);
}

glm::vec2 CalThumbstick(const glm::vec2& vec) {
	float l = sqrtf(vec.x * vec.x + vec.y*vec.y);
	float s = l;
	return vec * s;
}

void CGestureMatcher::GetGestures(const LEAP_HAND* f_hand, std::map<HandGesture, bool>& gestures, std::vector<float>& values, const LEAP_HAND* f_oppHand) {

	// 手势缓存支持
	static std::map<HandGesture, bool> static_lr_gestures[2];
	static std::vector<float> static_lr_values[2];

	// 需要检测的有效手势
	static const std::vector<HandGesture> detect_gestures = {
			HG_Open,
			HG_EmptyHold,
			HG_SolidHold,
			HG_Point,
			HG_IndexTouch,
			HG_PinkyTouch
	};

	// 使用前，对手势缓存初始化
	for (int i = 0; i < 2; i++) {
		std::map<HandGesture, bool>& gestures = static_lr_gestures[i];
		if (gestures.size() == 0) {
			for (int j = 0; j < detect_gestures.size(); j++) {
				gestures[detect_gestures[j]] = false;
			}
		}

		std::vector<float>& values = static_lr_values[i];
		if (values.size() == 0) {
			values.resize(HGS_MAX);
			for (int i = 0; i < values.size(); i++) {
				values[i] = 0.0f;
			}
		}
	}

	if (f_hand) {

		std::map<HandGesture, bool>& static_gesture = static_lr_gestures[ f_hand->type ];
		std::vector<float>& static_value = static_lr_values[ f_hand->type ];

		

		//printf("[gestures] HG_Open %d HG_EmptyHold %d HG_SolidHold %d HGS_Hold %f\n", open_test, emptyhold_test, solidhold_test, hold_value);

		// 先进行单个手势的基础识别
		for (int i = 0; i < detect_gestures.size(); i++) {

			HandGesture gesture = detect_gestures[i];

			// 检测手势是否被识别
			if (!GestureTest(f_hand, gesture, f_oppHand)) continue;

			// 空手手势总是被初始化为false
			static_gesture[HG_Open] = false;

			// 空杯手势将清空当前手的所有值
			if (gesture == HG_Open) {
				for (auto it : static_gesture) {
					static_gesture[it.first] = false;
				}
				static_gesture[gesture] = true;

				for (int i = 0; i < static_value.size(); i++) {
					static_value[i] = 0.0f;
				}

				printf("[%d] reset\n", f_hand->type);
			}
			else {

				// 说明：握拳动作必须先于手枪手势出现才有效

				// 不可重复识别的手势，value中出现了有效手势，则当前手势不能被识别
				static std::map<HandGesture, std::vector<HandGesture>> truepass_gestures = {
					{HG_EmptyHold,{HG_SolidHold, HG_Point}}, // 如果实心拳头出现，则空拳不能被二次识别
					{HG_SolidHold,{HG_Point}}, // 如果手枪手势出现，则实心手势不能被二次识别，防止扣动扳机时，识别为握拳
				};

				// 对当前已经识别到的手势进行同类手势检测
				bool truepass = false;
				for (HandGesture truepass_gesture : truepass_gestures[gesture]) {
					if (static_gesture[truepass_gesture]) {
						truepass = true;
						break;
					}
				}
				if (!truepass)
					static_gesture[gesture] = true;

				// 互斥手势，对低级别手势进行强行清除（比如一些过渡手势，例如半握），value中出现了有效手势，则当前手势被确定为互斥状态（清除true状态）
				static std::map<HandGesture, std::vector<HandGesture>> mutex_gestures = {
					{HG_EmptyHold,{HG_SolidHold, HG_Point}},
				};
				for (auto it : mutex_gestures) {
					for (HandGesture mutex_gesture : it.second) {
						if (static_gesture[mutex_gesture]) {
							static_gesture[it.first] = false;
							break;
						}
					}
				}
			}
		}

		// 如果手势存在，则进行值的更新，否则置零
		#define UpdateValue(c, g) {static_value[__HGS_LAST + 1 + g] = static_value[g]; \
if (c == -1 || static_gesture[(HandGesture)c]) static_value[g] = GestureValue(f_hand, g, f_oppHand); \
else static_value[g] = 0.0f;}

		// static_value[HGS_Hold] = GestureValue(f_hand, HGS_Hold, f_oppHand);
		// static_value[HGS_IndexContact] = GestureValue(f_hand, HGS_IndexContact, f_oppHand);
		/*if (static_gesture[HG_Point]) {
			static_value[__HGS_Trigger] = static_value[HGS_Trigger]; // 记录最后一次的值
			static_value[HGS_Trigger] = GestureValue(f_hand, HGS_Trigger, f_oppHand);
		}*/

		// 先处理三个距离值
		UpdateValue(-1, HGS_Hold);
		UpdateValue(HG_Point, HGS_Trigger);
		UpdateValue(-1, HGS_IndexContact);

		// 副手识别
		if (f_oppHand) {

			std::map<HandGesture, bool>& static_gesture2 = static_lr_gestures[f_oppHand->type];

			// 副手握拳+主手手枪，捕捉握拳水平移动
			if (static_gesture2[HG_EmptyHold] || static_gesture2[HG_SolidHold]) {
				
				if (static_gesture[HG_Point]) {

					// 当前手掌中心点
					glm::vec3 l_start = GetVec3(f_oppHand, P_Palm);
					
					// 当触摸板没有值的时候，初始化中心点坐标
					if (static_value[__HGS_TrackpadX] == 0.0f && static_value[__HGS_TrackpadY] == 0.0f) {

						// printf("[%d] �������� \n", f_oppHand->type);

						static_value[__HGS_TrackpadX] = l_start.x;
						static_value[__HGS_TrackpadY] = l_start.y;
					}
					else {
						// 计算圆盘中心偏移距离（归一化）
						glm::vec2 l_uv(-(l_start.x - static_value[__HGS_TrackpadX]), l_start.y - static_value[__HGS_TrackpadY]);

						l_uv /= 25.0f; // 圆盘半径

						glm::vec2 old_uv = l_uv;

						if (glm::length(l_uv) > 1.f)
							l_uv = glm::normalize(l_uv);

						static_value[HGS_TrackpadX] = l_uv.x;
						static_value[HGS_TrackpadY] = l_uv.y;
					}

					// 触摸板按下由主手扳机控制
					static_value[HGS_TrackpadClick] = static_value[HGS_Trigger] >= 1.0f;

					// 禁止副手手势
					static_gesture2[__HG_Disable] = true;
				}
			}

			// 双手手枪时的手势确认(为了防止误识别，设定双手扳机不能扣下)
			float oppTrigger = GestureValue(f_oppHand, HGS_Trigger, f_hand);
			if (static_gesture[HG_Point] && static_value[HGS_Trigger] < 0.25f && oppTrigger < 0.25f) {

				// 确定第一时间初始化
				bool reset = static_gesture2[HG_Point] && static_value[__HGS_ThumbstickKeep] == 0.0f;

				// 已初始化，但未进入单手控制状态时
				if (static_value[__HGS_ThumbstickKeep] == 0.5f) {
					// 副手空手势时的操作
					if (static_gesture2[HG_Open]) {
						// 如果主手位移超过0.5半径，重设状态（保证主手以当前位置再次初始化）
						if (DistanceCenter(static_value[HGS_ThumbstickX], static_value[HGS_ThumbstickY]) >= 0.5f) {
							reset = true;
						}
						else {
							// 主手不动，副手立马空手势，即希望副手腾出来，设置此单手摇杆持续标记
							static_value[__HGS_ThumbstickKeep] = 1.0f;
						}
					}
				}

				if (reset) {

					// printf("[%d] ҡ������ \n", f_hand->type);

					// 主手中心点
					glm::vec3 l_start = GetVec3(f_hand, P_Palm);

					static_value[HGS_ThumbstickX] = 0.0f;
					static_value[HGS_ThumbstickY] = 0.0f;
					static_value[__HGS_ThumbstickX] = l_start.x; // 记录中心点
					static_value[__HGS_ThumbstickY] = l_start.y;
					
					static_value[__HGS_ThumbstickKeep] = 0.5f; // 初始化过的标记
					static_value[HGS_ThumbstickTouch] = 0.0f;
					//static_value[HGS_ThumbstickClick] = 0.0f;
				}
			}
		}

		// 对已初始化过的主手进行位移监测，来设置摇杆坐标
		if (static_value[__HGS_ThumbstickKeep] > 0.0f) {

			// 主手中心点
			glm::vec3 l_start = GetVec3(f_hand, P_Palm);

			// 计算当前偏移
			glm::vec2 l_uv(-(l_start.x - static_value[__HGS_ThumbstickX]), l_start.y - static_value[__HGS_ThumbstickY]);

			l_uv /= 35.0f + 20;

			glm::vec2 old_uv = l_uv;

			float invalid_length = 0.15f;

			if (glm::length(l_uv) > 1.f) {
				l_uv = glm::normalize(l_uv);
			} else if (glm::length(l_uv) < invalid_length) {
				l_uv = { 0.0f , 0.0f };
			}
				
			l_uv = CalThumbstick(l_uv);

			// 更新摇杆值
			static_value[HGS_ThumbstickX] = l_uv.x;
			static_value[HGS_ThumbstickY] = l_uv.y;

			// printf("[%d] ҡ������ %.2f, %.2f \n", f_hand->type, static_value[HGS_ThumbstickX], static_value[HGS_ThumbstickY]);

			// 只要触发坐标位移，则表示触摸板被接触
			static_value[HGS_ThumbstickTouch] = static_value[HGS_ThumbstickX] != 0.0f || static_value[HGS_ThumbstickY] != 0.0f;//static_value[HGS_Trigger] > 0.0f;
			// 扳机控制触摸板按下
			static_value[HGS_ThumbstickClick] = static_value[HGS_Trigger] >= 1.0f;
			
			// printf("%.2f %.2f %.2f %.2f\n", DistanceCenter(static_value[HGS_ThumbstickX], static_value[HGS_ThumbstickY]), static_value[HGS_ThumbstickX], static_value[HGS_ThumbstickY], static_value[__HGS_ThumbstickKeep]);
		}

		// 直接复制返回值
		gestures = static_gesture;
		values = static_value;

		// 摇杆启用时，扳机值在返回值中无效
		if (values[__HGS_ThumbstickKeep] > 0.0f) {
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