#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

#include "scene_object.h"

class GameSystem
{
public:
    virtual ~GameSystem() {}
    virtual bool Init() { return true; };
    virtual void Cleanup() { };
    virtual Component* CreateComponent(SceneObject* so, const char* name) { return 0; }
    virtual void DestroyComponent(Component* comp) {}
    virtual bool Update() { return true; };
};

#endif
