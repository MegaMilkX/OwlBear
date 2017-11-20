#include <string>

#include "render_system.h"

#define DLLEXPORT __declspec(dllexport)

extern "C" 
{
    DLLEXPORT void SystemStart()
    {
        std::cout << "RenderSystem start" << std::endl;
    }

    DLLEXPORT Component* SystemGetComponent(SceneObject* so, const char* name)
    {
        std::string nm(name);

        if (nm == "Renderer")
            return new Renderer(so);
        else if (nm == "Camera")
            return new Camera(so);
        else if (nm == "Mesh")
            return new Mesh(so);

        return 0;
    }

    DLLEXPORT void SystemCleanup()
    {

    }
}