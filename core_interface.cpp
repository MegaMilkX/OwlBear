#include "core_interface.h"

#include "timer.h"

#include "scene_object.h"

static Au::Timer timer;

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
    while (dt < 1.0f / 60.0f)
    {
        dt = timer.End() / 1000000.0f;
    }
    return true;
}