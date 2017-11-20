#include "core_interface.h"

#include <aurora/timer.h>
#include <aurora/gfx.h>

static Au::Timer timer;

Core::Core()
: state(0), dt(.0f)
{
    ModuleLibrary::Init(this);
}

Core::~Core()
{
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
    
    if (!state)
        return false;
    state->Update();
    
    if (!ModuleLibrary::Update(dt))
        return false;

    while (dt < 1.0f / 60.0f)
    {
        dt = timer.End() / 1000000.0f;
    }
    return true;
}