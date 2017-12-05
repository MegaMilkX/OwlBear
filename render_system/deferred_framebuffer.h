#ifndef DEFERRED_FRAMEBUFFER_H
#define DEFERRED_FRAMEBUFFER_H

#include "framebuffer.h"
#include "render_target.h"
#include "mesh.h"

class DeferredFrameBuffer
{
public:
    DeferredFrameBuffer(unsigned width, unsigned height)
        : width(width), height(height)
    {
        fb = new FrameBuffer(width, height);
        diffuse = new Texture2D();
        normal = new Texture2D();
        fb->SetTexture(0, diffuse);
        fb->SetTexture(1, normal);

        shaderProgram.VertexShader(
#include "deferred_vs.glsl"
        );
        shaderProgram.FragmentShader(
#include "deferred_fs.glsl"
        );
        shaderProgram.Link();

        std::vector<gfxm::vec3> vertexData = {
            { -1.0f, -1.0f, 0.0f },
            { 1.0f, -1.0f, 0.0f },
            { 1.0f, 1.0f, 0.0f },
            { -1.0f, 1.0f, 0.0f }
        };
        std::vector<gfxm::vec2> uvData = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };
        std::vector<unsigned short> indexData = {
            0, 1, 2, 0, 2, 3
        };

        screenQuad = new Mesh();
        screenQuad->SetPositionData(vertexData);
        screenQuad->SetUVData(uvData);
        screenQuad->SetIndexData(indexData);
    }
    ~DeferredFrameBuffer()
    {
        delete fb;
        delete diffuse;
    }

    void Resize(unsigned width, unsigned height)
    {
        fb->Resize(width, height);
    }

    void Bind()
    {
        fb->Bind();
    }

    void Render(FrameBuffer* fb)
    {
        fb->Bind();
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Bind();
        diffuse->Bind(0);
        normal->Bind(1);
        screenQuad->Bind();
        screenQuad->Render();
    }

    void RenderToWindow()
    {

    }

    Texture2D* GetColorBuffer() { return diffuse; }
private:
    Texture2D* diffuse;
    Texture2D* normal;
    FrameBuffer* fb;

    ShaderProgram shaderProgram;

    Mesh* screenQuad;

    unsigned width, height;
};

#endif
