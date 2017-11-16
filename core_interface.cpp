#include "core_interface.h"

#include <aurora/timer.h>
#include <aurora/gfx.h>

#include "scene_object.h"

static Au::Timer timer;

CoreInterface::CoreInterface()
: state(0), dt(.0f)
{
    window = Au::Window::Create("OwlBear", 640, 480);
    window->Show();

    gfx = new Gfx();
    gfx->Init(*window);
}

CoreInterface::~CoreInterface()
{
    gfx->Cleanup();
    delete gfx;

    Au::Window::Destroy(window);
}

Gfx* CoreInterface::GetGfx()
{
    return gfx;
}

float CoreInterface::DeltaTime()
{
    return dt;
}

void CoreInterface::Switch(GameState* state)
{
    this->state = state;
    state->Switch();
}

bool CoreInterface::Update()
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