#pragma once

#include "input.h"
#include "renderer.h"
#include <SDL3/SDL.h>

class Window {
public:
    Window();
    ~Window();

    bool Update();
    void SwapScreen();

    void SetRenderer(Renderer* renderer) {m_renderer = renderer;}
    void SetInput(Input* input) {m_input = input;}

private:
    SDL_Window* m_window;
    SDL_GLContext m_glContext;
    Renderer* m_renderer;
    Input* m_input;
};