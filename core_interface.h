#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include <set>

#include "game_state.h"

#include "gfx.h"

#include "scene_object_impl.h"

#include "game_system_library.h"

namespace Au {
    namespace GFX { class Device; }
}

class SceneObject;
class Core : public ICore
{
public:
    Core();
    ~Core();

    virtual void Switch(GameState* state);
    virtual float DeltaTime();

    virtual SceneObject* CreateScene();
    virtual void DestroyScene(SceneObject* so);

    bool Update();
private:
    std::set<SceneObject*> scenes;

    Au::Window* window;
    Au::GFX::Device* gfxDevice;
    
    Gfx* gfx;
    GameState* state;
    float dt;
};

#endif
