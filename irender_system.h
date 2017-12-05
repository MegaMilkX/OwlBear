#ifndef IRENDER_SYSTEM_H
#define IRENDER_SYSTEM_H

#include <Windows.h>

#include "ispatial_system.h"

class IMesh
{
public:

};

#pragma pack(push, 1)
struct MeshObject
{
    IMesh* mesh;
    ITransformNode* transform;
};
#pragma pack(pop)

class IRenderScene
{
public:
    virtual MeshObject* CreateMeshObject() = 0;
    virtual void DestroyMeshObject(MeshObject* mo) = 0;
};

#pragma pack(push, 1)
struct Viewpoint
{
    ITransformNode* view;
    float projection[16];
    IRenderScene* renderScene;
};
#pragma pack(pop)

class IRenderTarget
{
public:
    virtual Viewpoint* GetViewpoint() = 0;
};

class IRenderSystem
{
public:
    virtual bool Init(HWND window) = 0;
    virtual void Cleanup() = 0;

    virtual void ResizeCanvas(int width, int height) = 0;
    
    virtual void Update() = 0;

    virtual IMesh* CreateMesh() = 0;
    virtual void DestroyMesh(IMesh* mesh) = 0;

    virtual IRenderScene* CreateScene() = 0;
    virtual void DestroyScene(IRenderScene*) = 0;

    virtual IRenderTarget* DefaultRenderTarget() = 0;
};

#endif
