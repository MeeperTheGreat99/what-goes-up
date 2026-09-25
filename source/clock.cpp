#include "clock.h"
#include <chrono>

using namespace std::chrono;

long long Clock::GetTime() {
    return high_resolution_clock::now().time_since_epoch().count();
}

Clock::Clock() {
    m_initTime = GetTime();
}

float Clock::GetElapsedTime() {
    long long time = GetTime() - m_initTime;
    return (float)time / std::nano::den;
}