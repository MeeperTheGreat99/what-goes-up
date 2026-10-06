#pragma once
#include "item.h"
#include "camera.h"
#include "input.h"
#include "physentity.h"

class Player : public PhysEntity {
public:
    struct {
        std::vector <Item*> items;
    } inventory;

    static constexpr float Friction = 8.0f;
    static constexpr float MaxSpeed = 6.0f;
    static constexpr float MaxStepHeight = 0.25f;
    static constexpr float Reach = 2.0f;
    static constexpr float Mass = 80.0f;
    static constexpr float CollisionRadius = 0.4f;
    static constexpr float CollisionHeight = 1.8f;
    static constexpr float ViewOfs = -0.2f;
    
    static Player* Instance;

    Player() {
        Instance = this;
    }

    virtual void SetAngles(Vector angles) override {
        m_lookAngles = angles;
    }

    virtual void Spawn() override {
        PhysEntity::Spawn();

        m_forwardAction = m_input->CreateAction(SDL_SCANCODE_W);
        m_backwardAction = m_input->CreateAction(SDL_SCANCODE_S);
        m_rightAction = m_input->CreateAction(SDL_SCANCODE_D);
        m_leftAction = m_input->CreateAction(SDL_SCANCODE_A);
        m_jumpAction = m_input->CreateAction(SDL_SCANCODE_SPACE);
        m_useAction = m_input->CreateAction(SDL_SCANCODE_E);
        m_spAction = m_input->CreateAction(SDL_SCANCODE_Q);

        for (int i = 0; i < 4; i++) {
            m_stepSounds[i] = Audio::Instance->LoadSample("res/sounds/footsteps/step" + std::to_string(i + 1) + ".wav");
        }

        m_stepSource = new Audio::Source(m_stepSounds[0]);
        m_stepSource->Set3D();

        m_voice = new Audio::Source(nullptr);
        m_voice->Set3D();
        
        m_onGroundPrev = false;
        m_airJump = false;
        m_shape = new btCapsuleShape(CollisionRadius, CollisionHeight - 2 * CollisionRadius);
        m_trShape = new btSphereShape(CollisionRadius - 0.01f);
        m_trFullShape = new btCapsuleShape(CollisionRadius - 0.01f, CollisionHeight - 2 * (CollisionRadius - 0.01f));
        InitializeRigidbody(m_shape, Mass);
        m_rigidbody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
        m_rigidbody->setFriction(0.0f);
        m_rigidbody->setSleepingThresholds(0.0f, 0.0f);
        m_useEnt = nullptr;
        m_heldItem = nullptr;
        m_folders = 0;
    }

    virtual void Clean() override {
        PhysEntity::Clean();

        PhysicsWorld::SafeDeleteShape(m_trFullShape);
        PhysicsWorld::SafeDeleteShape(m_trShape);
        PhysicsWorld::SafeDeleteShape(m_shape);

        delete m_stepSource;
    }

    virtual void FrameUpdate(float delta) override {
        PhysEntity::FrameUpdate(delta);

        m_stepSource->SetPos(GetPos());
        m_voice->SetPos(GetPos());
        m_camera.SetPos(GetHeadPos());
        m_camera.SetAng(m_lookAngles);

        if (m_heldItem) {
            Vector right = m_lookAngles.direction().cross(Vector(0, 1, 0)).normalized();
            Vector up = right.cross(m_lookAngles.direction()).normalized();
            m_heldItem->SetPos(GetHeadPos() + m_lookAngles.direction() * 0.25f + right * 0.35f - up * 0.2f);
            m_heldItem->SetAngles(m_lookAngles + Vector(70, 0, 0));
        }

        m_moveInput = Vector(0.0f, 0.0f, 0.0f);

        if (m_forwardAction->IsHeld()) {
            m_moveInput += Vector(0, 0, 1);
        }

        if (m_backwardAction->IsHeld()) {
            m_moveInput += Vector(0, 0, -1);
        }

        if (m_rightAction->IsHeld()) {
            m_moveInput += Vector(1, 0, 0);
        }

        if (m_leftAction->IsHeld()) {
            m_moveInput += Vector(-1, 0, 0);
        }

        m_moveInput.normalize();
    }

    virtual void FixedUpdate(float delta) override {
        float mx, my;
        
        PhysEntity::FixedUpdate(delta);

        if (m_spAction->IsPressed()) {
            m_voice->SetSample(Audio::Instance->LoadSample("res/sounds/voicelines/carl.wav"));
            m_voice->Play();
        }

        m_input->GetMouseDelta(mx, my);
        m_lookAngles += Vector(-my, mx, 0.0f);
        m_lookAngles.x = std::clamp(m_lookAngles.x, -89.99f, 89.99f);

        Vector angles = Vector(0, m_lookAngles.y, 0);
        Vector forward = angles.direction();

        PhysicsWorld::TraceResult result;
        Vector trStart = GetPos() - Vector(0.0f, CollisionHeight * 0.5f - CollisionRadius, 0.0f);
        bool onGround = PhysicsWorld::TraceShape(trStart, trStart - Vector(0.0f, 0.02f, 0.0f), m_trShape, result);
        Vector groundNormal = onGround ? result.normal : Vector(0, 1, 0);
        if (groundNormal.dot(Vector(0, 1, 0)) < 0.5f) {
            groundNormal = Vector(0, 1, 0);
            onGround = false;
        }

        Vector groundRight = forward.cross(groundNormal).normalized();
        Vector groundForward = groundNormal.cross(groundRight).normalized();

        float acceleration = 100.0f * (onGround ? 1.0f : 0.05f) * delta;
        Vector velocity = m_rigidbody->getLinearVelocity();
        velocity += (groundForward * m_moveInput.z + groundRight * m_moveInput.x) * acceleration;

        if (onGround) {
            float friction = 1.0f / (1.0f + delta * Friction);
            velocity *= friction;

            if (!m_onGroundPrev) {
                m_airJump = false;
            }
            
            if (velocity.length2() > MaxSpeed * MaxSpeed) {
                velocity = velocity.normalized() * MaxSpeed;
            }

            if (m_jumpAction->IsPressed()) {
                velocity.y += 4.0f;
                onGround = false;
                m_airJump = true;
            }

            m_rigidbody->setGravity(Vector(0).bt());
        } else {
            m_rigidbody->setGravity(Entity::World->world->getGravity());
        }
        
        if (!onGround && m_onGroundPrev && !m_airJump && velocity.y > 0.0f) {
            velocity.y = 0.0f;
        }


        m_rigidbody->setLinearVelocity(velocity.bt());
        UseTrace();

        if (m_useAction->IsPressed() && m_useEnt) {
            Item* item = dynamic_cast<Item*>(m_useEnt);
            if (item) {
                item->SetCollisionEnabled(false);
                inventory.items.push_back(item);

                if (m_heldItem) {
                    m_heldItem->SetPos(4096.0f);
                }

                m_heldItem = item;

                Folder* folder = dynamic_cast<Folder*>(item);
                if (folder) {
                    switch (m_folders) {
                    case 0:
                        Say("res/sounds/voicelines/keep_looking.wav");
                        break;
                    case 1:
                        Say("res/sounds/voicelines/more_concrete.wav");
                        break;
                    case 2:
                        Say("res/sounds/voicelines/perfect_for_media.wav");
                        break;
                    }

                    m_folders++;
                }
            } else {
                m_useEnt->Use(true);
            }
        }

        if (onGround && Entity::WorldTime >= m_nextStepTime && velocity.length2() > 0.1f) {
            int snd = rand() % 4;
            for (int i = 0; i < rand() % 4; i++) {
                snd = rand() % 4;
            }
            m_stepSource->Stop();
            m_stepSource->SetSample(m_stepSounds[snd]);
            m_stepSource->Play();
            float time = std::clamp(MaxSpeed / velocity.length() * 0.4f, 0.4f, 0.55f);
            m_nextStepTime = Entity::WorldTime + time;
        }

        m_onGroundPrev = onGround;
    }

    Vector GetHeadPos() {
        return GetPos() + Vector(0.0f, CollisionHeight * 0.5f + ViewOfs, 0.0f);
    }

    void UseTrace() {
        PhysicsWorld::TraceResult result;
        Vector forward = m_lookAngles.direction();
        Vector start = GetHeadPos();
        
        if (PhysicsWorld::TraceLine(start, start + forward * Reach, result)) {
            m_useEnt = result.entity;
        } else {
            m_useEnt = nullptr;
        }

        if (m_useEnt) {
            std::string useText;
            Item* item = dynamic_cast<Item*>(m_useEnt);
            if (item) {
                useText = "Pick up " + item->GetName();
            } else {
                useText = m_useEnt->GetUseText();
            }
            
            if (!useText.empty()) {
                Renderer::Instance->SetInfoText("[E] " + useText);
            } else {
                Renderer::Instance->SetInfoText("");
            }
        } else {
            Renderer::Instance->SetInfoText("");
        }
    }

    void Say(std::string filename) {
        m_voice->SetSample(Audio::Instance->LoadSample(filename));
        m_voice->Play();
    }

    Item* GetHeldItem() { return m_heldItem; }

    void SetInput(Input* input) {m_input = input;}

    Camera& GetCamera() {return m_camera;}

private:
    Input* m_input;
    Input::Action* m_forwardAction;
    Input::Action* m_backwardAction;
    Input::Action* m_rightAction;
    Input::Action* m_leftAction;
    Input::Action* m_jumpAction;
    Input::Action* m_useAction;
    Input::Action* m_spAction;
    Audio::Sample* m_stepSounds[4];
    Audio::Source* m_stepSource;
    Audio::Source* m_voice;
    bool m_onGroundPrev;
    bool m_airJump;
    float m_nextStepTime;
    Camera m_camera;
    btCollisionShape* m_shape;
    btCollisionShape* m_trShape;
    btCollisionShape* m_trFullShape;
    Vector m_lookAngles;
    Vector m_moveInput;
    Entity* m_useEnt;
    Item* m_heldItem;
    int m_folders;
};