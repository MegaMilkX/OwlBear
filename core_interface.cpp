#include "core_interface.h"

#include <aurora/timer.h>
#include <aurora/gfx.h>

static Au::Timer timer;

Core::Core()
: state(0), dt(.0f)
{
    ModuleLibrary::Init(this);

    window = Au::Window::Create("OwlBear", 640, 480);
    window->Show();

    gfx = new Gfx();
    gfx->Init(*window);
}

Core::~Core()
{
    gfx->Cleanup();
    delete gfx;

    Au::Window::Destroy(window);

    ModuleLibrary::Cleanup();
}

void Core::Switch(GameState* state)
{
    this->state = state;
    state->Switch();
}

float Core::DeltaTime()
{
    return dt;
}

SceneObject* Core::CreateScene()
{
    SceneObject* s = new SceneObjectImpl(this);
    scenes.insert(s);
    return s;
}

void Core::DestroyScene(SceneObject* so)
{
    scenes.erase(so);
}

bool Core::Update()
{
    timer.Start();
    if (window->Destroyed())
        return false;
    Au::Window::PollMessages();
    if (!state)
        return false;

    state->Update();
    
    gfx->Render();

    while (dt < 1.0f / 60.0f)
    {
        dt = timer.End() / 1000000.0f;
    }
    return true;
}