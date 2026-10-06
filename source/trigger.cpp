#include "trigger.h"

void Trigger::Spawn() {
    Entity::Spawn();

    std::vector<Mesh*> meshes = m_model->GetMeshes();
    Vector p = meshes[0]->GetVertices()[0].position;
    Vector min = p, max = p;

    for (Mesh* mesh : meshes) {
        for (const Vertex& v : mesh->GetVertices()) {
            min = Vector::min(v.position, min);
            max = Vector::max(v.position, max);
        }
    }

    m_min = min;
    m_max = max;

    m_triggered = false;
}

void Trigger::FixedUpdate(float delta) {
    Entity::FixedUpdate(delta);

    Camera* camera = Renderer::Instance->GetCamera();
    if (!camera || m_triggered) {
        return;
    }

    Vector pos = camera->GetPos();
    if (pos.x >= m_min.x && pos.y >= m_min.y && pos.z >= m_min.z &&
        pos.x < m_max.x && pos.y < m_max.y && pos.z < m_max.z) {
        m_triggered = true;
        
        Use(true);
    }
}

ENTCLASS(trigger, Trigger)