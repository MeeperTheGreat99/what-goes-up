#pragma once

class Clock {
public:
    static long long GetTime();
    
    Clock();

    float GetElapsedTime();

private:

    long long m_initTime;
};