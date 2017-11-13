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

    gfxDevice = new Au::GFX::Device();
    gfxDevice->Init(*window);
}

CoreInterface::~CoreInterface()
{
    gfxDevice->Cleanup();
    delete gfxDevice;

    Au::Window::Destroy(window);
}

Au::GFX::Device* CoreInterface::GetGfxDevice()
{
    return gfxDevice;
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
    gfxDevice->Clear();
    gfxDevice->SwapBuffers();
    while (dt < 1.0f / 60.0f)
    {
        dt = timer.End() / 1000000.0f;
    }
    return true;
}