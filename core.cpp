#include "core.h"

#include <aurora/timer.h>
#include <aurora/gfx.h>

static Au::Timer timer;

Core::Core()
: state(0), dt(.0f)
{
    
}

Core::~Core()
{
    
}

bool Core::Init()
{
    window = Au::Window::Create("AuroraX", 1280, 720);
    window->Show();

    IRenderSystem* renderInterface = renderSys.Init(this, "RenderSystemInit");
    if (!renderInterface)
        return false;
    renderInterface->Init(*window);

    IAudioSystem* audioInterface = audioSys.Init(this, "AudioSystemInit");

    return true;
}

void Core::Cleanup()
{
    if (renderSys.GetInterface())
        renderSys.GetInterface()->Cleanup();
    renderSys.Cleanup();
    
    if (audioSys.GetInterface())
        audioSys.GetInterface()->Cleanup();
    audioSys.Cleanup();

    Au::Window::Destroy(window);
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

IRenderSystem* Core::GetRenderSystem()
{
    return renderSys.GetInterface();
}

IAudioSystem* Core::GetAudioSystem()
{
    return audioSys.GetInterface();
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
    
    if(audioSys.GetInterface())
        audioSys.GetInterface()->Update();
    renderSys.GetInterface()->Update();

    while (dt < 1.0f / 60.0f)
    {
        dt = timer.End() / 1000000.0f;
    }
    return true;
}