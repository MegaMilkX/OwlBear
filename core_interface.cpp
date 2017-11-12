#include "core_interface.h"

#include "timer.h"

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
    if (!state)
        return false;

    state->Update();
    dt = timer.End() / 1000000.0f;
    return true;
}