#ifndef ICORE_H
#define ICORE_H

#include "game_state.h"
#include "igfx.h"

class ICore
{
public:
    virtual IGfx* GetGfx() = 0;

    virtual void Switch(GameState* state) = 0;
    virtual float DeltaTime() = 0;
};

#endif
