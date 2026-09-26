#include "accept.h"
#include "clock.h"
#include "entities/evelator.h"
#include "entity.h"
#include "input.h"
#include "physics.h"
#include "renderer.h"
#include "rubik.h"
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
    try {
        window = new Window();
        renderer = new Renderer();
        input = new Input();
        physics = new PhysicsWorld();
    } catch (const std::exception& e) {
        reportException(e);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    window->SetRenderer(renderer);
    window->SetInput(input);

    Camera cam;
    cam.SetPos(Vector(0, 0, 5));
    cam.SetAng(Vector(0, 90, 0));
    renderer->SetCamera(&cam);

    // Rubik* rubik = new Rubik();
    Evelator* evelator = new Evelator();
    evelator->Spawn();

    Clock clock;
    float accumulator = 0.0f, last = 0.0f;
    constexpr float logicPeriod = 1.0f / 60.0f;
    while (window->Update()) {
        float time = clock.GetElapsedTime();
        float delta = time - last;
        accumulator += delta;

        while (accumulator >= logicPeriod) {
            physics->Update(logicPeriod);
            for (auto& entry : Entity::Entities) {
                Entity* entity = entry.second;
                entity->FixedUpdate(logicPeriod);
            }

            accumulator -= logicPeriod;
        }

        for (auto& entry : Entity::Entities) {
            Entity* entity = entry.second;
            entity->FrameUpdate(delta);
        }

        last = time;

        renderer->Draw();
        window->SwapScreen();
        input->Update();
    }

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