#pragma once
#include <SDL3/SDL_scancode.h>
#include <map>

class Input {
public:
    friend class Action;

    class Action {
    public:
        friend class Input;

        Action(int code) : m_code(code), m_held(false), m_heldPrev(false) {}

        bool IsHeld() const { return m_held; }
        bool IsPressed() const { return m_held && !m_heldPrev; }
        bool IsReleased() const { return !m_held && m_heldPrev; }

    private:
        int m_code;
        bool m_held, m_heldPrev;
    };

    ~Input();

    void Update();
    void SetActionState(int code, bool held);
    Action* CreateAction(int code);

private:
    std::map<int, Action*> m_actions;
};