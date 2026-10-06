#include "fakefloor.h"
#include "player.h"

FakeFloor::FakeFloor() {
    m_collapseSound = new Audio::Source(Audio::Instance->LoadSample("res/sounds/environment/collapse.wav"));
    m_collapseSound->Set3D();
}

void FakeFloor::Spawn() {
    PhysEntity::Spawn();

    m_shape = PhysicsWorld::ShapeFromModel(m_model, true);
    InitializeRigidbody(m_shape, 0.0f);

    std::vector<Mesh*> meshes = m_model->GetMeshes();
    Vector center(0.0f);

    for (Mesh* mesh : meshes) {
        center += mesh->GetCentroid();
    }

    center /= (float)meshes.size();

    m_collapseSound->SetPos(center);
    m_triggered = false;

}

void FakeFloor::Clean() {
    PhysEntity::Clean();

    PhysicsWorld::SafeDeleteShape(m_shape);
}

void FakeFloor::Trigger(bool state) {
    if (m_triggered) {
        return;
    }

    m_triggered = true;
    m_collapseSound->Play();
    DestroyRigidbody();

    Player::Instance->Say("res/sounds/voicelines/gasp.wav");
}

void FakeFloor::FrameUpdate(float delta) {
    m_collapseSound->SetPos(GetPos());
}

ENTCLASS(fakefloor, FakeFloor);