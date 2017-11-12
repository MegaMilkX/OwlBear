#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include "game_state.h"

class CoreInterface
{
public:
    CoreInterface()
        : state(0), dt(.0f)
    {

    }

    virtual void Switch(GameState* state);
    virtual float DeltaTime();

    virtual bool Update();
private:
    GameState* state;
    float dt;
};

#endif
