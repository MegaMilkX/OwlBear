#include <iostream>
#include "../icore.h"
#include "../scene_object.h"
#include "../render_system/render_system.h"

class Gameplay : public GameState
{
public:
    Gameplay(ICore* core)
        : core(core)
    {
        scene = core->CreateScene();
        Camera* cam = scene->GetComponent<Camera>();
        Mesh* mesh = scene->GetComponent<Mesh>();
    }
    ~Gameplay() {}

    void Init() {}
    void Cleanup() {}
    void Switch() {}
    bool Update()
    {
        
        return true;
    }
private:
    ICore* core;
    SceneObject* scene;
};

Gameplay* gameplay = 0;


#define DLLEXPORT __declspec(dllexport)

extern "C" 
{

    DLLEXPORT GameState* OwlBearStart(ICore* core)
    {
        gameplay = new Gameplay(core);
        return gameplay;
    }

    DLLEXPORT void OwlBearCleanup()
    {
        delete gameplay;
    }

}