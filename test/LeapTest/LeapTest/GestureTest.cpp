#include "stdafx.h"
#include "GestureTest.h"
#include <numeric>

#define TEST_OPEN

LEAP_VECTOR GetVector(const LEAP_HAND* f_hand, Position pos) {
	LEAP_VECTOR vector_array[] = {
		f_hand->thumb.distal.next_joint,
		f_hand->index.distal.next_joint,
		f_hand->middle.distal.next_joint,
		f_hand->ring.distal.next_joint,
		f_hand->pinky.distal.next_joint,
		f_hand->palm.position
	};

	return vector_array[pos];
}

glm::vec3 GetVec3(const LEAP_HAND* f_hand, Position pos) {
	auto vec = GetVector(f_hand, pos);
	glm::vec3 l_start(vec.x, vec.y, vec.z);
	return l_start;
}

float Distance(const std::vector<LEAP_VECTOR>& vecList, const LEAP_VECTOR& vec2) {

	float result = 0;
	for (int i = 0; i < vecList.size(); i++) {

		const LEAP_VECTOR& vec1 = vecList[i];
		glm::vec3 l_start(vec1.x, vec1.y, vec1.z);
		glm::vec3 l_end(vec2.x, vec2.y, vec2.z);
		float l_length = glm::distance(l_start, l_end);

		result += l_length;
	}
	result /= vecList.size();

	return result;
}

float DistanceNormalization(const LEAP_VECTOR& vec1, const LEAP_VECTOR& vec2, float min, float max) {

	float length = Distance({ vec1 }, vec2);
	float value = (Distance({ vec1 }, vec2) - min) / (max - min);
	return fminf(fmaxf(1.0f - value, 0.0f), 1.0f);
}

float Distance3D(const float x[3])
{
	return sqrtf(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
}



static LEAP_VECTOR GetCenter(const LEAP_HAND* f_hand, const std::vector<Position>& positions) {

	LEAP_VECTOR sum = {0};
	for (int i = 0; i < positions.size(); i++) {
		LEAP_VECTOR vector = GetVector(f_hand, positions[i]);

		sum.x += vector.x;
		sum.y += vector.y;
		sum.z += vector.z;
	}
	sum.x /= positions.size();
	sum.y /= positions.size();
	sum.z /= positions.size();
	return sum;
}


class DistanceRule {

public:


	DistanceRule(Position pos1, Position pos2, float min, float max) {

		this->pos1 = pos1;
		this->pos2 = pos2;

		limit.x = min;
		limit.y = max;

		limit_test.x = 10000;
		limit_test.y = 0;
	}

	bool check(const LEAP_HAND* f_hand) {

		float len = Distance({ GetVector(f_hand, pos1) }, GetVector(f_hand, pos2));

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

// ���������Ƿ�����������
bool DistanceLimit(const LEAP_HAND* f_hand, std::vector<DistanceRule>& dis_rules) {

	bool result = true;
	for (int i = 0; i < dis_rules.size(); i++) {
		if (!dis_rules[i].check(f_hand)) {
			result = false;
		}
	}

	return result;
}

float Distance(const LEAP_HAND* f_hand, Position pos1, Position pos2) {

	const LEAP_VECTOR& vec1 = GetVector(f_hand, pos1);
	const LEAP_VECTOR& vec2 = GetVector(f_hand, pos2);
	glm::vec3 l_start(vec1.x, vec1.y, vec1.z);
	glm::vec3 l_end(vec2.x, vec2.y, vec2.z);
	float l_length = glm::distance(l_start, l_end);

	return l_length;
}

bool PalmIsUp(const LEAP_VECTOR& v) {
	return v.x < 0.5 && v.y < 0 && v.z < 0;
}

class Gesture {

public:
	Gesture(CGestureMatcher::HandGesture handGesture) {

		this->handGesture = handGesture;
	}

	bool check(const LEAP_HAND* f_hand, const LEAP_HAND* f_oppHand, float* value=0) {

		bool result = true;
		

		if (CGestureMatcher::HG_Open == handGesture) {
			// (53.33, 92.13, 63.91) (66.94, 104.35, 101.31) (70.17, 110.85, 102.77) (76.30, 103.76, 96.66) (65.70, 96.55, 89.01)
			std::vector<DistanceRule> dis_rules = {
				DistanceRule(P_Thumb, P_Palm, 52, 110),
				DistanceRule(P_Index, P_Palm, 65, 112),
				DistanceRule(P_Middle, P_Palm, 69, 116),
				DistanceRule(P_Ring, P_Palm, 67.60, 114),
				//DistanceRule(P_Pinky, P_Palm, 65, 101),
				DistanceRule(P_Index, P_Thumb, 50, 1000),
				DistanceRule(P_Pinky, P_Thumb, 69, 1000),
			};

			//printf("test %f %f %f %f\n", Distance(f_hand, P_Index, P_Thumb), Distance(f_hand, P_Middle, P_Thumb), Distance(f_hand, P_Ring, P_Thumb), Distance(f_hand, P_Pinky, P_Thumb));

			// 打印手掌朝向
			printf("朝向 %f %f %f %d\n", f_hand->palm.normal.x, f_hand->palm.normal.y, f_hand->palm.normal.z, PalmIsUp(f_hand->palm.normal));

			result = DistanceLimit(f_hand, dis_rules);
		}
		else if (CGestureMatcher::HG_EmptyHold == handGesture) {
			// (47.97, 75.96, 56.54) (40.99, 68.35, 44.83) (42.48, 79.68, 54.39) (35.76, 77.91, 55.73) (35.43, 64.02, 49.43)
			std::vector<DistanceRule> dis_rules = {
				DistanceRule(P_Index, P_Thumb, 30, 70),
				DistanceRule(P_Index, P_Palm, 30, 78),
				DistanceRule(P_Middle, P_Palm, 30, 92),
				DistanceRule(P_Ring, P_Palm, 32, 86),
				DistanceRule(P_Pinky, P_Palm, 39, 68),
			};
			result = DistanceLimit(f_hand, dis_rules);
		}
		else if (CGestureMatcher::HG_SolidHold == handGesture) {
			// (42.80, 71.09, 45.65) (31.39, 50.50, 38.48) (29.74, 45.37, 33.52) (30.12, 42.23, 32.66) (28.71, 39.41, 30.61)
			std::vector<DistanceRule> dis_rules = {
				DistanceRule(P_Index, P_Thumb, 30, 70),
				DistanceRule(P_Index, P_Palm, 16, 66.50),
				DistanceRule(P_Middle, P_Palm, 23, 54),
				DistanceRule(P_Ring, P_Palm, 22, 51),
				DistanceRule(P_Pinky, P_Palm, 25, 48),
			};

			// �����ĸ���ָ�ķ�Χ
			result = DistanceLimit(f_hand, dis_rules);
		}
		else if (CGestureMatcher::HG_Point == handGesture) {
			// (42.80, 71.09, 45.65) (31.39, 50.50, 38.48) (29.74, 45.37, 33.52) (30.12, 42.23, 32.66) (28.71, 39.41, 30.61)
			std::vector<DistanceRule> dis_rules = {
				DistanceRule(P_Index, P_Palm, 65, 112),
				DistanceRule(P_Middle, P_Palm, 23, 54),
				DistanceRule(P_Ring, P_Palm, 22, 51),
				DistanceRule(P_Pinky, P_Palm, 25, 48),
			};

			// �����ĸ���ָ�ķ�Χ
			result = DistanceLimit(f_hand, dis_rules);
		}
		else if (CGestureMatcher::HG_IndexTouch == handGesture) {
			std::vector<DistanceRule> dis_rules = {
				DistanceRule(P_Index, P_Thumb, 10, 30),
				DistanceRule(P_Middle, P_Palm, 69, 116),
				DistanceRule(P_Ring, P_Palm, 67.60, 114),
				DistanceRule(P_Pinky, P_Palm, 65, 101),
			};

			//printf("%f \n", Distance(f_hand, P_Index, P_Thumb));

			// �����ĸ���ָ�ķ�Χ
			result = DistanceLimit(f_hand, dis_rules);

			if (!PalmIsUp(f_hand->palm.normal)) result = false;
		}
		else if (CGestureMatcher::HG_PinkyTouch == handGesture) {
			std::vector<DistanceRule> dis_rules = {
				DistanceRule(P_Index, P_Thumb, 65, 112),
				//DistanceRule(P_Middle, P_Palm, 69, 116),
				//DistanceRule(P_Ring, P_Palm, 67.60, 114),
				DistanceRule(P_Pinky, P_Thumb, 10, 68),
			};

			// 食指与拇指距离
			//printf("距离 %f\n", Distance(f_hand, P_Thumb, P_Index));

			// �����ĸ���ָ�ķ�Χ
			result = DistanceLimit(f_hand, dis_rules);

			if (!PalmIsUp(f_hand->palm.normal)) result = false;
		}
		else {
			assert(!"error");
		}

		return result;
	}

	void printInfo() {

		// �����������ƽ��
		/*printf("printInfo");
		for (int i = 0; i < dis_rules.size(); i++) {
			float len = dis_rules[i].len;

			printf(" (%.2f, %.2f, %.2f)", dis_rules[i].limit_test.x, dis_rules[i].limit_test.y, len);
		}

		printf("\n");*/

	}

private:

	
	
	CGestureMatcher::HandGesture handGesture;
};


bool GestureTest(const LEAP_HAND* f_hand, CGestureMatcher::HandGesture handGesture, const LEAP_HAND* f_oppHand) {

	bool result = false;
	
	Gesture gesture_emptyhold = Gesture(handGesture);
	result = gesture_emptyhold.check(f_hand, f_oppHand);

	return result;
}



float GestureValue(const LEAP_HAND* f_hand, CGestureMatcher::HandGestureSub handGesture, const LEAP_HAND* f_oppHand) {

	if (CGestureMatcher::HGS_Hold == handGesture) {
		// ����4ָ���ľ������ĵľ���

		std::vector<Position> positions = {
				P_Index,
				P_Middle,
				P_Ring,
				P_Pinky
		};

	 	LEAP_VECTOR vec1 = GetCenter(f_hand, positions);
		LEAP_VECTOR vec2 = GetVector(f_hand, P_Palm);

		// printf("拇指 %f\n", Distance(f_hand, P_Thumb, P_Palm));

		return DistanceNormalization(vec1, vec2, 29, 88);
	}
	else if (CGestureMatcher::HGS_Trigger == handGesture) {
		
		LEAP_VECTOR vec1 = GetVector(f_hand, P_Index);
		LEAP_VECTOR vec2 = GetVector(f_hand, P_Palm);

		return DistanceNormalization(vec1, vec2, 60, 90);
	}
	else if (CGestureMatcher::HGS_IndexContact == handGesture) {
		if (f_hand && f_oppHand) {

			auto vec1 = GetVector(f_hand, P_Index);
			auto vec2 = GetVector(f_oppHand, P_Index);

			//printf("%f\n", DistanceNormalization(vec1, vec2, 20, 40));

			// �����ĸ���ָ�ķ�Χ
			return DistanceNormalization(vec1, vec2, 20, 40);
		}
	}
	else {
		assert(!"error");
	}

	return 0.0f;
}