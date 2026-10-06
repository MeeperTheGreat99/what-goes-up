#pragma once
#include "model.h"
#include "physentity.h"

class Item : public PhysEntity {
public:
    virtual void Spawn() override {
        PhysEntity::Spawn();
        m_shape = PhysicsWorld::ShapeFromModel(m_model, false);
        InitializeRigidbody(m_shape, 1.0f);
    }

    virtual void Clean() override {
        PhysEntity::Clean();
        PhysicsWorld::SafeDeleteShape(m_shape);
    }

    std::string GetName() const { return m_name; }

protected:
    std::string m_name = "";

private:
    btCollisionShape* m_shape = nullptr;
}; 

class PornUSB : public Item {
public: 
    PornUSB(){
        m_model = Model::LoadExternal("res/models/thumb_drive.obj");
        m_name = "Thumb drive";
    }
};

class Keycard : public Item {
public:
    Keycard() {
        m_model = Model::LoadExternal("res/models/card.glb");
        m_name = "Keycard";
    }
};

class Folder : public Item {
public:
    Folder() {
        m_model = Model::LoadExternal("res/models/folder.glb");
        m_name = "Confidential Information";
    }
};