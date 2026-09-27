#include "light.h"
#include "renderer.h"

Light::Light() {
    Renderer::Instance->m_lights.push_back(this);
}

Light::~Light() {
    Renderer::Instance->m_lights.erase(std::find(
        Renderer::Instance->m_lights.begin(),
        Renderer::Instance->m_lights.end(), this
    ));
}