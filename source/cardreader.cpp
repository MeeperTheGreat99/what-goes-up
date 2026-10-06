#include "cardreader.h"
#include "player.h"

CardReader::CardReader() {
    m_model = Model::LoadExternal("res/models/reader.glb");
    m_accessSound = new Audio::Source(Audio::Instance->LoadSample("res/sounds/environment/access_granted.wav"));
    m_accessSound->Set3D();
    m_swipes = 0;
    m_pranked = false;
}

void CardReader::Spawn() {
    PhysEntity::Spawn();

    m_shape = PhysicsWorld::ShapeFromModel(m_model, true);
    InitializeRigidbody(m_shape, 0.0f);
}

void CardReader::Clean() {
    PhysEntity::Clean();

    PhysicsWorld::SafeDeleteShape(m_shape);
}

void CardReader::Use(bool keydown) {
    Player* player = Player::Instance;
    Keycard* card = nullptr;

    for (Item* item : player->inventory.items) {
        card = dynamic_cast<Keycard*>(item);

        if (card) {
            break;
        }
    }

    if (card) {
        PhysEntity::Use(keydown);
        m_accessSound->Play();
        
        if (++m_swipes == 3 && !m_pranked) {
            m_pranked = true;
            player->Say("res/sounds/voicelines/dumbass.wav");
        }
    } else {
        player->Say("res/sounds/voicelines/need_card.wav");
    }
}

void CardReader::FrameUpdate(float delta) {
    PhysEntity::FrameUpdate(delta);

    m_accessSound->SetPos(GetPos());
}

ENTCLASS(keycard_reader, CardReader)