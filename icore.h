#ifndef ICORE_H
#define ICORE_H

#include "game_state.h"
#include "scene_object.h"

class ICore
{
public:
    virtual void Switch(GameState* state) = 0;
    virtual float DeltaTime() = 0;

    virtual SceneObject* CreateScene() = 0;
    virtual void DestroyScene(SceneObject* so) = 0;
};

#endif
