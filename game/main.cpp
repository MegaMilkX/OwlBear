#include <iostream>
#include "../core_interface.h"
#include "../scene_object.h"

class Gameplay : public GameState
{
public:
    Gameplay(CoreInterface* core)
        : core(core)
    {
        SceneObject* o = scene.CreateObject();
    }
    ~Gameplay() {}

    void Init() {}
    void Cleanup() {}
    void Switch() {}
    bool Update()
    {
        std::cout << "dt: " << core->DeltaTime() << std::endl;
        return true;
    }
private:
    CoreInterface* core;
    SceneObject scene;
};

Gameplay* gameplay = 0;


#define DLLEXPORT __declspec(dllexport)

extern "C" 
{

    DLLEXPORT GameState* OwlBearStart(CoreInterface* core)
    {
        std::cout << "Hello, World!" << std::endl;
        gameplay = new Gameplay(core);
        return gameplay;
    }

    DLLEXPORT void OwlBearCleanup()
    {
        delete gameplay;
    }

}