
#include "GestureTest.h"
#include <glm/glm.hpp>
#include <numeric>

//#define TEST

float Distance(const LEAP_VECTOR& vec1, const LEAP_VECTOR& vec2) {

	glm::vec3 l_start(vec1.x, vec1.y, vec1.z);
	glm::vec3 l_end(vec2.x, vec2.y, vec2.z);
	float l_length = glm::distance(l_start, l_end);
	return l_length;
}


bool GestureTest(const LEAP_HAND* f_hand, CGestureMatcher::HandGesture handGesture) {

	bool result = false;

	static std::vector<glm::vec2> open_limit;
	if (open_limit.size() == 0) {
		open_limit.resize(5);
		for (int i = 0; i < 5; i++) {
			open_limit[i].x = 10000;
			open_limit[i].y = 0;
		}
	}

	if (CGestureMatcher::HG_Open == handGesture) {

		static std::vector<std::vector<float>> open_data;
		if (open_data.size() == 0) {
			open_data.resize(5);
		}

		LEAP_VECTOR vector_array[] = {
			f_hand->thumb.distal.next_joint,
			f_hand->index.distal.next_joint,
			f_hand->middle.distal.next_joint,
			f_hand->ring.distal.next_joint,
			f_hand->pinky.distal.next_joint
		};


		for (int i = 0; i < 5; i++) {
			float len = Distance(vector_array[i], f_hand->palm.position);

			open_data[i].push_back(len);
			if (open_data[i].size() > 10) {
				open_data[i].erase(open_data[i].begin());
			}

			if (open_limit[i].x > len) {
				open_limit[i].x = len;
			}
			if (open_limit[i].y < len) {
				open_limit[i].y = len;
			}
		}

#ifdef TEST
		// 计算最近几次平均
		printf("HG_Open");
		for (int i = 0; i < 5; i++) {
			float len = std::accumulate(open_data[i].begin(), open_data[i].end(), 0) / open_data[i].size();

			//printf(" %.2f", len);

			printf(" (%.2f, %.2f, %.2f)", open_limit[i].x, open_limit[i].y, len);
		}

		printf("\n");
#endif

		// (63.08, 89.18) (76.53, 104.76) (86.93, 108.62) (79.24, 103.45) (61.99, 94.02)
		glm::vec2 limits[] = {
			{63.08, 89.18},
			{76.53, 104.76},
			{86.93, 108.62},
			{79.24, 103.45},
			{61.99, 94.02}
		};
		bool match = true;
		for (int i = 0; i < 5; i++) {
			float len = open_data[i].at(open_data[i].size() - 1);
			if (!(limits[i].x <= len && limits[i].y >= len)) {
				match = false;
				break;
			}
		}
		result = match;
	}
	


	return result;
}