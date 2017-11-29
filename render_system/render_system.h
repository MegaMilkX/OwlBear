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
#include "framebuffer.h"
#include "mesh.h"

class RenderSystem : public IRenderSystem
{
public:
    bool Init(HWND window);

    void Cleanup();

    void ResizeCanvas(int width, int height);

    void Update();

    int APIVersion();

    IMesh* CreateMesh();
    void DestroyMesh(IMesh* mesh);

    // ==
    void RenderToScreen(Texture2D* texture);

private:
    HDC deviceContext;
    HGLRC context;
    HGLRC threadingContext;
    int contextVersion = 0;

    std::set<Mesh*> meshes;

    // Temp
    //Au::GFX::Mesh* mesh;
    Mesh* screenQuad;
    Mesh* cube;

    ShaderProgram* shaderProgram;
    ShaderProgram* sp_model;
    Texture2D* tex;
    Texture2D* diffuseBuffer;
    FrameBuffer* frameBuffer;

    gfxm::mat4 projection;
    gfxm::transform view;
    gfxm::transform model;
};

#endif
