#include <iostream>
#include "../core_interface.h"
#include "../scene_object.h"

class Gameplay : public GameState
{
public:
    Gameplay(CoreInterface* core)
        : core(core)
    {
        Camera* cam = core->GetGfx()->CreateCamera();
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
    CoreInterface* core;
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