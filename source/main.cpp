#include "accept.h"
#include "clock.h"
#include "physdebugger.h"
#include "player.h"
#include "world.h"
#include "evelator.h"
#include "input.h"
#include "physics.h"
#include "renderer.h"
#include "window.h"
#include <SDL3/SDL_main.h>
#include <exception>

#define CON
#ifdef CON
#include <Windows.h>
#endif

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

#ifdef CON
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    freopen("CONIN$", "r", stdin);
#endif

    Window* window = nullptr;
    Renderer* renderer = nullptr;
    Input* input = nullptr;
    PhysicsWorld* physics = nullptr;
    PhysDebugger* physicsDebugger = nullptr;
    try {
        window = new Window();
        renderer = new Renderer();
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

    Player* player = new Player();
    player->SetInput(input);
    player->SetPos(Vector(0, 2, 15));
    player->SetAngles(Vector(0, 90, 0));
    player->Spawn();
    renderer->SetCamera(&player->GetCamera());

    World* world = new World();
    world->Spawn();

    Evelator* evelator = new Evelator();
    evelator->SetPos(Vector(0, 0.2, -3));
    evelator->Spawn();

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
        }

        for (auto& entry : Entity::Entities) {
            Entity* entity = entry.second;
            if (entity->IsSpawned()) {
                entity->FrameUpdate(delta);
            }
        }

        last = time;

        renderer->Draw();
        window->SwapScreen();
    }

    delete physicsDebugger;
    delete physics;
    delete input;
    delete renderer;
    delete window;

#ifdef CON
    FreeConsole();
#endif

    SDL_Quit();
    return EXIT_SUCCESS;
}