#include <iostream>
#include "../icore.h"

class Gameplay : public GameState
{
public:
    Gameplay(ICore* core)
        : core(core)
    {

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