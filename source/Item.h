#pragma once
#include "model.h"

class Item {
public: 
    ~Item(){
        delete m_model;
    }
protected:
    Model* m_model = nullptr;
    std::string m_name = "";
}; 

class PornUSB : public Item {
public: 
    PornUSB(){
        m_model = Model::LoadExternal("res/models/thumb_drive.obj");
        m_name = "Thumb drive";
    }
};