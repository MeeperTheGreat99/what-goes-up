#include "input.h"

Input::~Input() {
    for (auto& pair : m_actions) {
        delete pair.second;
    }
}

void Input::Update() {
    for (auto& pair : m_actions) {
        pair.second->m_heldPrev = pair.second->m_held;
    }
}

void Input::SetActionState(int code, bool held) {
    if (m_actions.count(code)) {
        m_actions[code]->m_held = held;
    }
}

Input::Action* Input::CreateAction(int code) {
    if (m_actions.count(code)) {
        return nullptr;
    }

    Action* action = new Action(code);
    m_actions[code] = action;
    return action;
}