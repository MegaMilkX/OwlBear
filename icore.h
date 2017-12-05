    #ifndef ICORE_H
#define ICORE_H

#include "game_state.h"

#include "irender_system.h"
#include "ispatial_system.h"

class ICore
{
public:
    virtual void Switch(GameState* state) = 0;
    virtual float DeltaTime() = 0;
    
    virtual IRenderSystem* LoadRenderSystem(const char* filename) = 0;
    virtual IRenderSystem* GetRenderSystem() = 0;
    virtual ISpatialSystem* GetSpatialSystem() = 0;
};

#define DLLEXPORT __declspec(dllexport)
extern "C"
{
    DLLEXPORT int GameInit(ICore* core);
    DLLEXPORT GameState* GameStart(ICore* core);
    DLLEXPORT void GameCleanup();

}
#undef DLLEXPORT

#endif
