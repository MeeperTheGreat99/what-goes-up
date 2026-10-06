#include "impulse.h"
#include "map.h"
#include "player.h"

void Impulse::Spawn() {
    Entity::Spawn();

    m_source = new Audio::Source(Audio::Instance->LoadSample("res/sounds/voicelines/voice_message_edited.wav"));
    m_source->Set3D();
    m_collapseAudio = nullptr;
    m_titleCardTime = 0.0f;

    if (m_spawnflags & 2) {
        m_collapseAudio = new Audio::Sequence(m_source);
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/environment/gore.wav"));
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/voicelines/OH_DEAR.wav"));
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/voicelines/binifit_research.wav"));
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/voicelines/binifit_how.wav"));
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/voicelines/not_paid.wav"));
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/voicelines/waiting_for_raise.wav"));
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/environment/punishment.wav"));
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/voicelines/carl.wav"));
        m_collapseAudio->AddSample(Audio::Instance->LoadSample("res/sounds/environment/irm.wav"));
    }
}

void Impulse::Clean() {
    Entity::Clean();
}

void Impulse::Trigger(bool state) {
    if (m_spawnflags & 1) {
        Renderer::Instance->SetBlackScreen(true);
    } 

    if (m_spawnflags & 2) {
        Player::Instance->Say("res/sounds/voicelines/groan.wav");
        m_collapseAudio->Play();
        m_titleCardTime = Entity::WorldTime + 18.63f;
    }

    if (m_spawnflags & 4) {
        m_source->Play();
        m_mapChangeTime = Entity::WorldTime + 24.0f;
    }

    if (m_spawnflags & 8) {
        Player::Instance->Say("res/sounds/voicelines/door_unlocked.wav");
    }

    if (m_spawnflags & 16) {
        Player::Instance->Say("res/sounds/voicelines/guessed_wrong.wav");
    }
}

void Impulse::FrameUpdate(float delta)  {
    Entity::FrameUpdate(delta);

    m_source->SetPos(GetPos());
    if (m_collapseAudio) {
        m_collapseAudio->Update();
    }

    if (m_titleCardTime && Entity::WorldTime >= m_titleCardTime) {
        Renderer::Instance->SetTitleCard(true);
    }

    if (m_mapChangeTime && Entity::WorldTime >= m_mapChangeTime) {
        Map::QueuedMap = "intro";
        m_mapChangeTime = 0.0f;
    }
}

ENTCLASS(impulse, Impulse)