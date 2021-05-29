
#include "GestureTest.h"
#include <glm/glm.hpp>
#include <numeric>

#define TEST_OPEN

float Distance(const LEAP_VECTOR& vec1, const LEAP_VECTOR& vec2) {

	glm::vec3 l_start(vec1.x, vec1.y, vec1.z);
	glm::vec3 l_end(vec2.x, vec2.y, vec2.z);
	float l_length = glm::distance(l_start, l_end);
	return l_length;
}

float Distance3D(const float x[3])
{
	return sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
}

class DistanceRule {

public:

	enum Position{
		P_Thumb,
		P_Index,
		P_Middle,
		P_Ring,
		P_Pinky,
		P_Palm
	};

	DistanceRule(Position pos1, Position pos2, float min, float max) {

		this->pos1 = pos1;
		this->pos2 = pos2;

		limit.x = min;
		limit.y = max;

		limit_test.x = 10000;
		limit_test.y = 0;
	}

	bool check(const LEAP_HAND* f_hand) {

		LEAP_VECTOR vector_array[] = {
			f_hand->thumb.distal.next_joint,
			f_hand->index.distal.next_joint,
			f_hand->middle.distal.next_joint,
			f_hand->ring.distal.next_joint,
			f_hand->pinky.distal.next_joint,
			f_hand->palm.position
		};

		float len = Distance(vector_array[pos1], vector_array[pos2]);

		data.push_back(len);
		if (data.size() > 10) {
			data.erase(data.begin());
		}

		if (limit_test.x > len) {
			limit_test.x = len;
		}
		if (limit_test.y < len) {
			limit_test.y = len;
		}

		float result = true;

		if (!(limit.x <= len && limit.y >= len)) {
			result = false;
		}
		this->len = len;

		return result;
	}

	float len;
	glm::vec2 limit_test;

private:
	std::vector<float> data;
	glm::vec2 limit;

	Position pos1, pos2;
};

class Gesture {

public:
	Gesture(CGestureMatcher::HandGesture handGesture) {
		
		if (CGestureMatcher::HG_Open == handGesture) {
			// (53.33, 92.13, 63.91) (66.94, 104.35, 101.31) (70.17, 110.85, 102.77) (76.30, 103.76, 96.66) (65.70, 96.55, 89.01)
			rules = {
				DistanceRule(DistanceRule::P_Thumb, DistanceRule::P_Palm, 52, 110),
				DistanceRule(DistanceRule::P_Index, DistanceRule::P_Palm, 65, 112),
				DistanceRule(DistanceRule::P_Middle, DistanceRule::P_Palm, 69, 116),
				DistanceRule(DistanceRule::P_Ring, DistanceRule::P_Palm, 67.60, 114),
				DistanceRule(DistanceRule::P_Pinky, DistanceRule::P_Palm, 65, 101),
			}; 
		}
		else if (CGestureMatcher::HG_EmptyHold == handGesture) {
			// (47.97, 75.96, 56.54) (40.99, 68.35, 44.83) (42.48, 79.68, 54.39) (35.76, 77.91, 55.73) (35.43, 64.02, 49.43)
			rules = {
				DistanceRule(DistanceRule::P_Thumb, DistanceRule::P_Palm, 46, 88),
				DistanceRule(DistanceRule::P_Index, DistanceRule::P_Palm, 30, 78),
				DistanceRule(DistanceRule::P_Middle, DistanceRule::P_Palm, 30, 92),
				DistanceRule(DistanceRule::P_Ring, DistanceRule::P_Palm, 32, 86),
				DistanceRule(DistanceRule::P_Pinky, DistanceRule::P_Palm, 39, 68),
			};
		}
		else if (CGestureMatcher::HG_SolidHold == handGesture) {
			// (42.80, 71.09, 45.65) (31.39, 50.50, 38.48) (29.74, 45.37, 33.52) (30.12, 42.23, 32.66) (28.71, 39.41, 30.61)
			rules = {
				DistanceRule(DistanceRule::P_Thumb, DistanceRule::P_Palm, 24, 82.09),
				DistanceRule(DistanceRule::P_Index, DistanceRule::P_Palm, 16, 66.50),
				DistanceRule(DistanceRule::P_Middle, DistanceRule::P_Palm, 23, 54),
				DistanceRule(DistanceRule::P_Ring, DistanceRule::P_Palm, 22, 51),
				DistanceRule(DistanceRule::P_Pinky, DistanceRule::P_Palm, 25, 48),
			};
		}
		else {
			assert(!"error");
		}
	}

	bool check(const LEAP_HAND* f_hand) {

		bool result = true;
		for (int i = 0; i < rules.size(); i++) {
			if (!rules[i].check(f_hand)) {
				result = false;
			}
		}

		return result;
	}

	void printInfo() {

		// 计算最近几次平均
		printf("printInfo");
		for (int i = 0; i < rules.size(); i++) {
			float len = rules[i].len;

			printf(" (%.2f, %.2f, %.2f)", rules[i].limit_test.x, rules[i].limit_test.y, len);
		}

		printf("\n");

	}

private:
	std::vector<DistanceRule> rules;
};


bool GestureTest(const LEAP_HAND* f_hand, CGestureMatcher::HandGesture handGesture) {

	static Gesture gesture_open = Gesture(CGestureMatcher::HG_Open);
	static Gesture gesture_emptyhold = Gesture(CGestureMatcher::HG_EmptyHold);
	static Gesture gesture_solidhold = Gesture(CGestureMatcher::HG_SolidHold);

	bool result = false;
	
	if (CGestureMatcher::HG_Open == handGesture) {
		result = gesture_open.check(f_hand);
		//gesture_open.printInfo();
	}
	else if (CGestureMatcher::HG_EmptyHold == handGesture) {
		result = gesture_emptyhold.check(f_hand);
		gesture_emptyhold.printInfo();

	}
	else if (CGestureMatcher::HG_SolidHold == handGesture) {
		result = gesture_solidhold.check(f_hand);
		//gesture_solidhold.printInfo();
		
	}

	return result;
}