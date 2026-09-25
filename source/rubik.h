#pragma once
#include "entity.h"

class Rubik : public Entity {
public:
    Rubik() {
        m_model = Model::LoadExternal("res/models/utah_teapot.obj");
    }
};