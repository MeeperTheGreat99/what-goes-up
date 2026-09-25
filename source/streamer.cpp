#include "streamer.h"

Streamer::Streamer() {
    m_runThread = true;
    m_workThread = std::thread([this]() {
        ThreadMain();
    });
}

Streamer::~Streamer() {
    m_runThread = false;
    m_workThread.join();
}

void Streamer::ThreadMain() {
    while (m_runThread) {
        
    }
}