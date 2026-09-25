#pragma once
#include <thread>
#include <atomic>

class Streamer {
public:
    Streamer();
    ~Streamer();

private:
    std::atomic_bool m_runThread;
    std::thread m_workThread;

    void ThreadMain();
};