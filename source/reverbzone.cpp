#include "reverbzone.h"

EFXEAXREVERBPROPERTIES reverbs[] = {
    EFX_REVERB_PRESET_GENERIC,
    EFX_REVERB_PRESET_HALLWAY,
    EFX_REVERB_PRESET_ROOM,
    EFX_REVERB_PRESET_FACTORY_LARGEROOM,
    EFX_REVERB_PRESET_PIPE_LARGE
};

void ReverbZone::ApplyProperty(std::string key, std::string value) {
    Entity::ApplyProperty(key, value);

    if (key == "reverb") {
        m_reverb = reverbs[std::stoi(value)];
    }
}

void ReverbZone::Spawn() {
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
}

void ReverbZone::FixedUpdate(float delta) {
    Entity::FixedUpdate(delta);

    Camera* camera = Renderer::Instance->GetCamera();
    if (!camera) {
        return;
    }

    Vector pos = camera->GetPos();
    if (pos.x >= m_min.x && pos.y >= m_min.y && pos.z >= m_min.z &&
        pos.x < m_max.x && pos.y < m_max.y && pos.z < m_max.z) {
        if (Audio::Instance->GetReverbZone() != this) {
            Audio::Instance->SetReverbZone(this);
        }
    }
}

ENTCLASS(reverb_zone, ReverbZone)