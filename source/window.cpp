#include "window.h"
#include "gl.h"
#include "SDL3/SDL_video.h"
#include <stdexcept>

Window::Window() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    m_window = SDL_CreateWindow(
        "What Goes Up...", 800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!m_window) {
        throw std::runtime_error("Failed to create window");
    }

    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext || !SDL_GL_MakeCurrent(m_window, m_glContext)) {
        throw std::runtime_error("Failed to create OpenGL context");
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }

    m_mouseLocked = false;
}

Window::~Window() {
    SDL_GL_MakeCurrent(nullptr, nullptr);
    SDL_GL_DestroyContext(m_glContext);
    SDL_DestroyWindow(m_window);
}

bool Window::Update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            return false;
        case SDL_EVENT_WINDOW_RESIZED:
            glViewport(0, 0, event.window.data1, event.window.data2);
            m_renderer->Resize(event.window.data1, event.window.data2);
            SetMouseLocked(m_mouseLocked);
            break;
        case SDL_EVENT_KEY_DOWN:
            m_input->SetActionState(event.key.scancode, true);
            break;
        case SDL_EVENT_KEY_UP:
            m_input->SetActionState(event.key.scancode, false);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            switch (event.button.button) {
            case SDL_BUTTON_LEFT:
            case SDL_BUTTON_MIDDLE:
            case SDL_BUTTON_RIGHT:
                m_input->SetActionState(event.button.button, event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
                break;
            default:
                break;
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            m_input->AddMouseDelta(event.motion.xrel, event.motion.yrel);
            break;
        default:
            break;
        }
    }

    return true;
}

void Window::SwapScreen() {
    SDL_GL_SwapWindow(m_window);
}

void Window::SetMouseLocked(bool locked) {
    m_mouseLocked = locked;
    if (locked) {
        SDL_Rect rect;
        rect.x = rect.y = 0;
        SDL_GetWindowSize(m_window, &rect.w, &rect.h);
        rect.x = rect.w >> 1;
        rect.y = rect.h >> 1;
        rect.w = 1;
        rect.h = 1;
        SDL_SetWindowMouseRect(m_window, &rect);
    } else {
        SDL_SetWindowMouseRect(m_window, NULL);
    }
    SDL_SetWindowRelativeMouseMode(m_window, locked);
}