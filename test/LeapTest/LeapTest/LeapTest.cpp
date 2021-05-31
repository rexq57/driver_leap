// LeapTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include "CGestureMatcher.h"

#ifdef __cplusplus
extern "C" {
#endif
    #include <LeapC.h>
    #include "ExampleConnection.h"
#ifdef __cplusplus
}
#endif


//#include "CGesturMmatcher.h"

//using namespace Leap;
//
//class samplelistener : public listener {
//public:
//    virtual void onconnect(const controller&);
//    virtual void onframe(const controller&);
//};
//
//void samplelistener::onconnect(const controller& controller) {
//    std::cout << "connected" << std::endl;
//}
//
//void samplelistener::onframe(const controller& controller) {
//    std::cout << "frame available" << std::endl;
//
//    std::vector<float> gestures;
//
//    cgesturematcher::getgestures(controller.frame(), gestures);
//}

void onFrame(const LEAP_TRACKING_EVENT* tracking_event) {

    std::map<CGestureMatcher::HandGesture, bool> gestures;
    std::vector<float> values;
    {
        const int LCH_Count = 2;
        LEAP_HAND* l_hands[LCH_Count] = { nullptr };

        const LEAP_TRACKING_EVENT* l_frame = tracking_event;
        {
            for (size_t i = 0U; i < l_frame->nHands; i++)
            {
                if (!l_hands[l_frame->pHands[i].type]) l_hands[l_frame->pHands[i].type] = &l_frame->pHands[i];
            }
        }

        // Update devices
        for (size_t i = 0U; i < LCH_Count; i++)
        {
            CGestureMatcher::GetGestures(l_hands[i], gestures, values, l_hands[(i + 1) % LCH_Count]);
        }
    }

    printf("[gestures] HG_EmptyHold %d HG_SolidHold %d HG_Point %d HG_IndexTouch %d HG_PinkyTouch %d HG_PalmTouch %f HGS_Hold %f HGS_Trigger %f HGS_Touchpad (%.2f, %.2f)\n", 
        (int)gestures[CGestureMatcher::HG_EmptyHold],
        (int)gestures[CGestureMatcher::HG_SolidHold],
        (int)gestures[CGestureMatcher::HG_Point],
        (int)gestures[CGestureMatcher::HG_IndexTouch],
        (int)gestures[CGestureMatcher::HG_PinkyTouch],
        values[CGestureMatcher::HGS_PalmTouch],
        values[CGestureMatcher::HGS_Hold],
        values[CGestureMatcher::HGS_Trigger],
        values[CGestureMatcher::HGS_TouchpadX],
        values[CGestureMatcher::HGS_TouchpadY]
        );

    //std::cout << "on frame" << std::endl;
}

int main()
{
    LEAP_CONNECTION* cnn = OpenConnection();
    ConnectionCallbacks.on_frame = onFrame;

    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    //CloseConnection();

    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
