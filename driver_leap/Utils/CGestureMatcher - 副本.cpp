#include "stdafx.h"

#include "Utils/CGestureMatcher.h"

const float g_pi = glm::pi<float>();
const float g_piHalf = g_pi * 0.5f;
const float g_piQuarter = g_pi * 0.25f;
extern const glm::mat4 g_identityMatrix;

void Merge(float& result, float value)
{
    result = std::max(result, value);
}
 
float Length3D(const float* x)
{
    return sqrtf(x[0]* x[0] + x[1]* x[1] + x[2]* x[2]);
}

static float lastHG_Trigger = 0;
static float lastHG_ThumbMiddleTouch = 0;
static float lastHG_ThumbPinkyTouch = 0;


#define FOLDER_NAME "log"
static bool MTHLogEnabled = true;
static FILE* _logfp = 0;

void MTHLog(const char* str, ...)
{

    if (MTHLogEnabled)
    {
        // ���ﲻ�ܼ���
        va_list pArgs;// = NULL;

        if (!_logfp)
        {
            // �洢��tmpָ���ļ���
            const char* logFilePath = "log.txt";
            _logfp = fopen(logFilePath, "w");
            if (!_logfp)
            {
                assert(!"�޷�����log.txt!");
                exit(0);
            }
            else {
                printf("[%s] �Ѵ�����־�ļ�: %s\n", FOLDER_NAME, logFilePath);
            }
        }

        va_start(pArgs, str);

        // д���ļ�
        vfprintf(_logfp, str, pArgs); fputs("\n", _logfp);

        // �����ӡ������̨
        printf("[%s] ", FOLDER_NAME); vprintf(str, pArgs); printf("\n");

        // ����������
        fflush(_logfp);

        va_end(pArgs);
    }

}

void CGestureMatcher::GetGestures(const LEAP_HAND *f_hand, std::vector<float> &f_result, const LEAP_HAND *f_oppHand)
{
    f_result.resize(HG_Count, 0.f);

    // Finger bends
    float l_fingerBend[5U] = { 0.f };
    for(size_t i = 0U; i < 5U; i++)
    {
        const LEAP_DIGIT &l_finger = f_hand->digits[i];
        glm::vec3 l_prevDirection;
        for(size_t j = 0U; j < 4U; j++)
        {
            const LEAP_BONE &l_bone = l_finger.bones[j];
            glm::vec3 l_direction(l_bone.next_joint.x - l_bone.prev_joint.x, l_bone.next_joint.y - l_bone.prev_joint.y, l_bone.next_joint.z - l_bone.prev_joint.z);
            l_direction = glm::normalize(l_direction);
            if(j > 0) l_fingerBend[i] += glm::acos(glm::dot(l_direction, l_prevDirection));
            l_prevDirection = l_direction;
        }
    }

    for(size_t i = 0U; i <= HG_PinkyBend; i++) f_result[i] = NormalizeRange(l_fingerBend[i], g_piHalf, g_pi);

    bool fast_moving = Length3D(f_hand->palm.velocity.v) > 300.0f;

    // Simple gestures
    float now_value = f_result[HG_IndexBend];
    // f_result[HG_Trigger] = now_value;
    if (fast_moving && now_value <= 0.1f && lastHG_Trigger >= 0.1f) {
        f_result[HG_Trigger] = 0.1f;
    }
    else {
        f_result[HG_Trigger] = now_value;
    }
    lastHG_Trigger = f_result[HG_Trigger];


    f_result[HG_Grab] = NormalizeRange((l_fingerBend[2U] + l_fingerBend[3U] + l_fingerBend[4U]) / 3.f, g_piHalf, g_pi);

    // Little complex gestures
    glm::vec3 l_start(f_hand->thumb.distal.next_joint.x, f_hand->thumb.distal.next_joint.y, f_hand->thumb.distal.next_joint.z);
    glm::vec3 l_end(f_hand->index.intermediate.prev_joint.x, f_hand->index.intermediate.prev_joint.y, f_hand->index.intermediate.prev_joint.z);
    f_result[HG_ThumbPress] = NormalizeRange(glm::distance(l_start, l_end), 35.f, 20.f);

    // 摁下拇指后，记录XZ中心点，并算出水平位移，在XZ面上
    // if(f_result[HG_ThumbPress] >= 0.5f) {
    //     if (last_ThumbPressX == 0 && last_ThumbPressY == 0) {
    //         last_ThumbPressX = l_start.x;
    //         last_ThumbPressY = l_start.y;
    //         // MTHLog("%f %f %f", l_start.x, l_start.y, l_start.z);
    //     }

    //     // 计算与中心点的角度
    //     glm::vec2 l_uv(-(l_start.x - last_ThumbPressX), l_start.y - last_ThumbPressY);
    //     l_uv /= 25.0f;

    //     glm::vec2 old_uv = l_uv;
 
    //     if(glm::length(l_uv) > 1.f)
    //         l_uv = glm::normalize(l_uv);

    //     f_result[HG_PalmPointX] = l_uv.x;
    //     f_result[HG_PalmPointY] = l_uv.y;

    //     MTHLog("%f %f %f (%f %f)", glm::length(old_uv), old_uv.x, old_uv.y, l_uv.x, l_uv.y);
    // }

    // Thumb Middle
    {
        glm::vec3 l_start(f_hand->thumb.distal.next_joint.x, f_hand->thumb.distal.next_joint.y, f_hand->thumb.distal.next_joint.z);
        glm::vec3 l_end(f_hand->middle.distal.next_joint.x, f_hand->middle.distal.next_joint.y, f_hand->middle.distal.next_joint.z);
        float l_length = glm::distance(l_start, l_end);
        float now_value = (l_length <= 50.f) ? std::min((50.f - l_length) / 20.f, 1.f) : 0.f;

        // f_result[HG_ThumbMiddleTouch] = now_value;

        // MTHLog("fuck %f", l_length);
        // �����ٶȣ�����һ�εĲ���
        if (fast_moving && now_value <= 0.5f && lastHG_ThumbMiddleTouch >= 0.5f) {
            f_result[HG_ThumbMiddleTouch] = 0.5f;
            //MTHLog("fuck last %f %f", lastHG_ThumbMiddleTouch, Length3D(f_hand->palm.velocity.v) / 10000.0f);
        } else {
            f_result[HG_ThumbMiddleTouch] = now_value;
        }
        lastHG_ThumbMiddleTouch = f_result[HG_ThumbMiddleTouch];

        //MTHLog("A %f %f (%f) (%f,%f,%f) %d %d ", f_result[HG_ThumbMiddleTouch], now_value, Length3D(f_hand->palm.velocity.v), f_hand->thumb.distal.next_joint.x, f_hand->thumb.distal.next_joint.y, f_hand->thumb.distal.next_joint.z, Length3D(f_hand->palm.velocity.v) > 300.0f, now_value < 0.0f);
    }

    

    // Thumb Pinky
    {
        glm::vec3 l_start(f_hand->thumb.distal.next_joint.x, f_hand->thumb.distal.next_joint.y, f_hand->thumb.distal.next_joint.z);
        glm::vec3 l_end(f_hand->pinky.distal.next_joint.x, f_hand->pinky.distal.next_joint.y, f_hand->pinky.distal.next_joint.z);
        float l_length = glm::distance(l_start, l_end);
        float now_value = (l_length <= 50.f) ? std::min((50.f - l_length) / 20.f, 1.f) : 0.f;

        // f_result[HG_ThumbPinkyTouch] = now_value;

        if (fast_moving && now_value <= 0.5f && lastHG_ThumbPinkyTouch >= 0.5f) {
            f_result[HG_ThumbPinkyTouch] = 0.5f;
        }
        else {
            f_result[HG_ThumbPinkyTouch] = now_value;
        }
        lastHG_ThumbPinkyTouch = f_result[HG_ThumbPinkyTouch];
    }


    // Two-handed gestures
    if(f_oppHand)
    {
        // Thumb Ring Root
        if (f_result[HG_Grab] > 0.0f)
        {
            glm::vec3 l_start(f_oppHand->thumb.distal.next_joint.x, f_oppHand->thumb.distal.next_joint.y, f_oppHand->thumb.distal.next_joint.z);
            glm::vec3 l_end(f_oppHand->index.metacarpal.next_joint.x, f_oppHand->index.metacarpal.next_joint.y, f_oppHand->index.metacarpal.next_joint.z);
            f_result[HG_ThumbIndexTouch] = NormalizeRange(glm::distance(l_start, l_end), 35.f, 20.f);
        }




        l_start = glm::vec3(f_oppHand->index.distal.next_joint.x, f_oppHand->index.distal.next_joint.y, f_oppHand->index.distal.next_joint.z);
        l_end = glm::vec3(f_hand->thumb.distal.next_joint.x, f_hand->thumb.distal.next_joint.y, f_hand->thumb.distal.next_joint.z);
        f_result[HG_ThumbCrossTouch] = NormalizeRange(glm::distance(l_start, l_end), 35.f, 20.f);

        l_end = glm::vec3(f_hand->middle.distal.next_joint.x, f_hand->middle.distal.next_joint.y, f_hand->middle.distal.next_joint.z);
        f_result[HG_MiddleCrossTouch] = NormalizeRange(glm::distance(l_start, l_end), 35.f, 20.f);

        const glm::vec3 l_handNormal(f_hand->palm.normal.x, f_hand->palm.normal.y, f_hand->palm.normal.z);

        l_start = glm::vec3(f_hand->palm.position.x, f_hand->palm.position.y, f_hand->palm.position.z);
        l_end = glm::vec3(f_oppHand->index.distal.next_joint.x, f_oppHand->index.distal.next_joint.y, f_oppHand->index.distal.next_joint.z);

        if(glm::acos(glm::dot(glm::normalize(l_end - l_start), -l_handNormal)) <= g_piQuarter)
        {
            f_result[HG_OpisthenarTouch] = NormalizeRange(glm::distance(l_start, l_end), 50.f, 30.f);
        }

        if(glm::acos(glm::dot(glm::normalize(l_end - l_start), l_handNormal)) <= g_piQuarter)
        {
            f_result[HG_PalmTouch] = NormalizeRange(glm::distance(l_start, l_end), 50.f, 30.f);
        }

        const glm::vec3 l_handPos(f_hand->palm.position.x, f_hand->palm.position.y, f_hand->palm.position.z);
        const glm::quat l_handRot(f_hand->palm.orientation.w, f_hand->palm.orientation.x, f_hand->palm.orientation.y, f_hand->palm.orientation.z);
        const glm::mat4 l_handTransform = glm::translate(g_identityMatrix, l_handPos)*glm::mat4_cast(l_handRot);

        l_start = glm::vec3(f_oppHand->index.distal.next_joint.x, f_oppHand->index.distal.next_joint.y, f_oppHand->index.distal.next_joint.z);
        glm::vec3 l_planePoint = glm::inverse(l_handTransform)*glm::vec4(l_start, 1.f);
        if((l_planePoint.y < 0.f) && (l_planePoint.y >= -150.f))
        {
            glm::vec2 l_uv(-l_planePoint.x, -l_planePoint.z);
            if(glm::length(l_uv) <= 125.f)
            {
                l_uv /= (f_hand->palm.width*0.5f);
                if(glm::length(l_uv) > 1.f) l_uv = glm::normalize(l_uv);

                f_result[HG_PalmPointX] = l_uv.x;
                f_result[HG_PalmPointY] = l_uv.y;
            }
        }
    }

    // 排除一些手势识别

    if (f_result[HG_ThumbCrossTouch] > 0.0f) {

        f_result[HG_ThumbPinkyTouch] = 0;
        f_result[HG_ThumbMiddleTouch] = 0;
        f_result[HG_PalmTouch] = 0;
    }

    if (f_result[HG_ThumbPress] >= 0.5f) {

        f_result[HG_ThumbPinkyTouch] = 0;
        f_result[HG_ThumbMiddleTouch] = 0;
        f_result[HG_PalmTouch] = 0;
    }

    if (f_result[HG_ThumbMiddleTouch] > 0.0f) {
        f_result[HG_ThumbPinkyTouch] = 0;
        f_result[HG_PalmTouch] = 0;
    }

    if (f_result[HG_ThumbPinkyTouch] > 0.0f) {
        f_result[HG_PalmTouch] = 0;
    }

    // 抓取
    if (f_result[HG_Grab] >= 0.25f) {
        f_result[HG_ThumbPinkyTouch] = 0;
        f_result[HG_ThumbMiddleTouch] = 0;
        f_result[HG_PalmTouch] = 0;
        f_result[HG_ThumbPress] = 0;

        if (f_result[HG_Grab] < 0.75f) {
            f_result[HG_Trigger] = 0;
        }
    }
}

float CGestureMatcher::NormalizeRange(float f_val, float f_min, float f_max)
{
    const float l_mapped = (f_val - f_min) / (f_max - f_min);
    return glm::clamp(l_mapped, 0.f, 1.f);
}
