#include <string>

#include "render_system.h"

#define STB_IMAGE_IMPLEMENTATION
extern "C" {
#include "stb_image.h"
}

RenderSystem* renderSystem = 0;

#define DLLEXPORT __declspec(dllexport)

extern "C" 
{
    DLLEXPORT IRenderSystem* RenderSystemInit(ICore* core)
    {
        return new RenderSystem(core);
    }
}