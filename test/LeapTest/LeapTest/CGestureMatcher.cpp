#include "CGestureMatcher.h"
#include <glm/glm.hpp>
#include "GestureTest.h"

void CGestureMatcher::GetGestures(const LEAP_HAND* f_hand, std::vector<float>& f_result, const LEAP_HAND* f_oppHand) {

	//printf("f_hand %p %_oppHand %p\n", f_hand, f_oppHand);
	static long count = 0;
	if (count++ < 1000) {
		return;
	}

	if (f_hand) {
		bool open_test = GestureTest(f_hand, HG_Open);
		bool emptyhold_test = GestureTest(f_hand, HG_EmptyHold);
		bool solidhold_test = GestureTest(f_hand, HG_SolidHold);

		printf("[gestures] HG_Open %d HG_EmptyHold %d HG_SolidHold %d\n", open_test, emptyhold_test, solidhold_test);
	}

	

	
	

}