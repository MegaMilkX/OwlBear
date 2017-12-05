#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "../icore.h"
#include "../irender_system.h"

#include <aurora/window.h>

#include <gfxm.h>

extern "C" {
#include "stb_image.h"
}

#include "shader_program.h"
#include "deferred_framebuffer.h"
#include "render_target.h"
#include "mesh.h"

class IMaterial
{
public:

};

struct RenderInstance
{
    gfxm::transform transform;
    IMaterial* material;
    IMesh* mesh;
};

class RenderScene : public IRenderScene
{
public:
    RenderScene(ICore* core)
    {
        defaultTransform = 
            core->GetSpatialSystem()->CreateNode();
    }
    ~RenderScene()
    {
        core->GetSpatialSystem()->DestroyNode(defaultTransform);
    }
    virtual MeshObject* CreateMeshObject()
    {
        MeshObject* mo = new MeshObject();
        mo->transform = defaultTransform;
        meshObjects.insert(mo);
        return mo;
    }
    virtual void DestroyMeshObject(MeshObject* mo)
    {
        meshObjects.erase(mo);
    }

    std::set<MeshObject*> meshObjects;
private:
    ICore* core;
    ITransformNode* defaultTransform;
};

class RenderSystem : public IRenderSystem
{
public:
    RenderSystem(ICore* core)
        :core(core)
    {

    }
    bool Init(HWND window);

    void Cleanup();

    void ResizeCanvas(int width, int height);

    void Update();
    void UpdateRenderTarget(RenderTarget* rt);

    int APIVersion();

    IMesh* CreateMesh();
    void DestroyMesh(IMesh* mesh);

    IRenderScene* CreateScene();
    void DestroyScene(IRenderScene* scene);

    IRenderTarget* DefaultRenderTarget() { return windowRenderTarget; }

    // ==
    void RenderToScreen(Texture2D* texture);

private:
    HDC deviceContext;
    HGLRC context;
    HGLRC threadingContext;
    int contextVersion = 0;

    std::set<Mesh*> meshes;
    std::set<RenderScene*> scenes;

    // Temp
    //Au::GFX::Mesh* mesh;
    Mesh* screenQuad;
    Mesh* cube;

    ShaderProgram* shaderProgram;
    ShaderProgram* sp_model;
    Texture2D* tex;

    DeferredFrameBuffer* deferredFrameBuffer;
    std::set<RenderTarget*> renderTargets;
    RenderTarget* windowRenderTarget;

    gfxm::mat4 projection;
    gfxm::transform view;
    gfxm::transform model;

    ICore* core;
};

#endif
