    #ifndef ICORE_H
#define ICORE_H

#include "game_state.h"

#include "irender_system.h"
#include "iaudio_system.h"

class ICore
{
public:
    virtual void Switch(GameState* state) = 0;
    virtual float DeltaTime() = 0;
    
    virtual IRenderSystem* GetRenderSystem() = 0;
    virtual IAudioSystem* GetAudioSystem() = 0;
};

#endif
