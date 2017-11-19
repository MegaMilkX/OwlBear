#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include "game_state.h"

#include "gfx.h"

namespace Au {
    namespace GFX { class Device; }
}

class SceneObject;
class Core
{
public:
    Core();
    ~Core();

    virtual IGfx* GetGfx();

    virtual void Switch(GameState* state);
    virtual float DeltaTime();

    bool Update();
private:
    Au::Window* window;
    Au::GFX::Device* gfxDevice;
    
    Gfx* gfx;
    GameState* state;
    float dt;
};

#endif
