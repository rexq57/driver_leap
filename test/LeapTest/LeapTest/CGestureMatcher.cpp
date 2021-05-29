#include "CGestureMatcher.h"
#include <glm/glm.hpp>
#include "GestureTest.h"

void CGestureMatcher::GetGestures(const LEAP_HAND* f_hand, std::vector<float>& f_result, const LEAP_HAND* f_oppHand) {

	//printf("f_hand %p %_oppHand %p\n", f_hand, f_oppHand);

	if (f_hand) {
		bool open_test = GestureTest(f_hand, HG_Open);

		printf("[gestures] %d\n", open_test);
	}

	

	
	

}