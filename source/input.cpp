#include "input.h"

Input* Input::Instance = nullptr;

Input::Input() {
    Instance = this;
}

Input::~Input() {
    for (auto& pair : m_actions) {
        delete pair.second;
    }
}

void Input::Update() {
    for (auto& pair : m_actions) {
        pair.second->m_heldPrev = pair.second->m_held;
    }
    m_mouseDX = m_mouseDY = 0.0f;
}

void Input::SetActionState(int code, bool held) {
    if (m_actions.count(code)) {
        m_actions[code]->m_held = held;
    }
}

void Input::AddMouseDelta(float x, float y) {
    m_mouseDX += x * m_mouseSens;
    m_mouseDY += y * m_mouseSens;
}

void Input::GetMouseDelta(float& x, float& y) {
    x = m_mouseDX;
    y = m_mouseDY;
}

Input::Action* Input::CreateAction(int code) {
    if (m_actions.count(code)) {
        return m_actions[code];
    }

    Action* action = new Action(code);
    m_actions[code] = action;
    return action;
}