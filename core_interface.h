#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include "game_state.h"

namespace Au {
    namespace GFX { class Device; }
}
namespace Au { class Window; }
class SceneObject;
class CoreInterface
{
public:
    CoreInterface();
    ~CoreInterface();

    virtual Au::GFX::Device* GetGfxDevice();

    virtual void Switch(GameState* state);
    virtual float DeltaTime();

    virtual bool Update();
private:
    Au::Window* window;
    Au::GFX::Device* gfxDevice;
    GameState* state;
    float dt;
};

#endif
