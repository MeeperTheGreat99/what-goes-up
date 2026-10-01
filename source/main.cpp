#include "accept.h"
#include "audio.h"
#include "clock.h"
#include "map.h"
#include "physdebugger.h"
#include "player.h"
#include "input.h"
#include "physics.h"
#include "renderer.h"
#include "window.h"
#include <SDL3/SDL_main.h>
#include <ctime>
#include <exception>

#define CON
#ifdef CON
#include <Windows.h>
#undef DrawText
#endif

int main(int argc, char* argv[]) {
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);

#ifdef CON
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
#endif

    Window* window = nullptr;
    Renderer* renderer = nullptr;
    Audio* audio = nullptr;
    Input* input = nullptr;
    PhysicsWorld* physics = nullptr;
    PhysDebugger* physicsDebugger = nullptr;
    try {
        window = new Window();
        renderer = new Renderer();
        audio = new Audio();
        input = new Input();
        physics = new PhysicsWorld();
        physicsDebugger = new PhysDebugger(renderer);
        Model::LoadErrorModel();
    } catch (const std::exception& e) {
        reportException(e);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    window->SetRenderer(renderer);
    window->SetInput(input);
    // physics->SetDebugger(physicsDebugger);
    Entity::World = physics;

    window->SetMouseLocked(true);

    Map::Load("intro");

    Player* player = new Player();
    player->SetInput(input);
    player->SetPos(Map::GetPlayerPosition());
    player->SetAngles(Map::GetPlayerAngles());
    player->Spawn();
    renderer->SetCamera(&player->GetCamera());

    /* Audio::Sample* sample = audio->LoadSample("res/sounds/voicelines/voice_message_edited.wav");
    Audio::Source* source = new Audio::Source(sample);
    source->Set3D();
    source->SetPos(phone->GetPos());
    source->Play(); */

    audio->SetReverb(EFX_REVERB_PRESET_ROOM);

    Clock clock;
    float accumulator = 0.0f, last = 0.0f;
    constexpr float logicPeriod = 1.0f / 60.0f;
    while (window->Update()) {
        float time = clock.GetElapsedTime();
        float delta = std::min(0.1f, time - last);
        accumulator += delta;

        while (accumulator >= logicPeriod) {
            physics->Update(logicPeriod);
            for (auto& entry : Entity::Entities) {
                Entity* entity = entry.second;
                if (entity->IsSpawned()) {
                    entity->FixedUpdate(logicPeriod);
                }
            }

            input->Update();
            accumulator -= logicPeriod;
            Entity::WorldTime += logicPeriod;
        }

        for (auto& entry : Entity::Entities) {
            Entity* entity = entry.second;
            if (entity->IsSpawned()) {
                entity->FrameUpdate(delta);
            }
        }

        last = time;

        renderer->Draw();
        std::string fps = "FPS: " + std::to_string((int)(1.0f / delta));
        renderer->DrawText(fps.c_str(), nullptr, 16, 0, 0);
        window->SwapScreen();
    }

    delete physicsDebugger;
    delete physics;
    delete input;
    delete audio;
    delete renderer;
    delete window;

#ifdef CON
    FreeConsole();
#endif

    SDL_Quit();
    return EXIT_SUCCESS;
}