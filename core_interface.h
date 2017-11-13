#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include "game_state.h"
#include "window.h"

class SceneObject;
class CoreInterface
{
public:
    CoreInterface()
        : state(0), dt(.0f)
    {
        window = Au::Window::Create("OwlBear", 1280, 720);
        window->Show();
    }

    virtual void Switch(GameState* state);
    virtual float DeltaTime();

    virtual bool Update();
private:
    Au::Window* window;
    GameState* state;
    float dt;
};

#endif
