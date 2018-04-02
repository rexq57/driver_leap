﻿#pragma once

#include "targetver.h"

#define NOMINMAX
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <algorithm>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifdef _WIN32
#include <mmsystem.h>  // for timeBeginPeriod()
#endif

#include <openvr_driver.h>
#include "Leap.h"
