#ifndef GFX_H
#define GFX_H

#include <Windows.h>
#include <set>
#include <gfxm.h>

class RenderTarget
{
public:
    virtual bool Init();
    virtual void Cleanup();
private:
    unsigned frameBuffer;
    unsigned colorBuffer;
    unsigned depthBuffer;
};

struct Camera
{
    Camera()
        : transform(1.0f), projection(1.0f), renderTarget(0)
    {}
    gfxm::mat4 transform;
    gfxm::mat4 projection;
    int renderTarget;
};

namespace Au { class Window; }

class Gfx
{
public:
    virtual bool Init(Au::Window& window);
    virtual void Cleanup();
    virtual int APIVersion();

    virtual Camera* CreateCamera();
    virtual void DestroyCamera(Camera* camera);

    void Render();
private:
    std::set<Camera*> cameras;

    HDC deviceContext;
    HGLRC context;
    HGLRC threadingContext;
    int contextVersion = 0;
};

#endif

