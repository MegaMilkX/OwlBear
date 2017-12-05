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

    if (spatialSys.GetInterface() == 0)
    {
        ISpatialSystem* spatialInterface = spatialSys.Init(this, "SpatialInit");
        if (!spatialInterface)
        {
            // TODO: LOGS
            std::cout << "No spatial system module found!" << std::endl;
            return false;
        }
    }

    if (renderSys.GetInterface() == 0)
    {
        IRenderSystem* renderInterface = renderSys.Init(this, "RenderSystemInit");
        if (!renderInterface)
        {
            // TODO: LOGS
            std::cout << "No render module found" << std::endl;
            return false;
        }
    }

    if (!spatialSys.GetInterface()->Init())
    {
        // TODO: LOGS
        std::cout << "spatial system failed to initialize" << std::endl;
        return false;
    }
    if (!renderSys.GetInterface()->Init(*window))
    {
        // TODO: LOGS
        std::cout << "render system failed to initialize" << std::endl;
        return false;
    }

    return true;
}

void Core::Cleanup()
{
    if (renderSys.GetInterface())
        renderSys.GetInterface()->Cleanup();
    renderSys.Cleanup();
    if (spatialSys.GetInterface())
        spatialSys.GetInterface()->Cleanup();
    spatialSys.Cleanup();

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

IRenderSystem* Core::LoadRenderSystem(const char* filename)
{
    if (renderSys.GetInterface() != 0)
        return 0;
    IRenderSystem* renderInterface = renderSys.Init(this, filename, "RenderSystemInit");
    if (!renderInterface)
        return 0;
    return renderInterface;
}

IRenderSystem* Core::GetRenderSystem()
{
    return renderSys.GetInterface();
}

ISpatialSystem* Core::GetSpatialSystem()
{
    return spatialSys.GetInterface();
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
    
    renderSys.GetInterface()->Update();

    while (dt < 1.0f / 60.0f)
    {
        dt = timer.End() / 1000000.0f;
    }
    return true;
}