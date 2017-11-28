#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "../icore.h"
#include "../irender_system.h"

#include <aurora/window.h>
#include <aurora/gfx.h>

extern "C" {
#include "stb_image.h"
}

class Texture2D
{
public:
    Texture2D()
    {
        glGenTextures(1, &texture);
        Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }

    ~Texture2D()
    {
        glDeleteTextures(1, &texture);
    }

    GLuint glIndex() { return texture; }

    void Resize(unsigned width, unsigned height)
    {
        Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }

    void Upload(unsigned char* data, unsigned width, unsigned height, unsigned bpp)
    {
        if (bpp < 1 || bpp > 4)
            return;

        GLenum fmt;
        if (bpp == 1)
            fmt = GL_RED;
        else if (bpp == 2)
            fmt = GL_RG;
        else if (bpp == 3)
            fmt = GL_RGB;
        else if (bpp == 4)
            fmt = GL_RGBA;

        Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
    }

    void Bind(int layer = 0)
    {
        glActiveTexture(GL_TEXTURE0 + layer);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
private:
    GLuint texture;
};

class FrameBuffer
{
public:
    FrameBuffer(unsigned width, unsigned height)
        : width(width), height(height)
    {
        glGenFramebuffers(1, &framebuffer);
        Bind();

        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "FrameBuffer creation failed!" << std::endl;
        }
    }

    ~FrameBuffer()
    {
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteRenderbuffers(1, &depthBuffer);
    }

    void Resize(unsigned width, unsigned height)
    {
        // TODO: 
        throw std::exception("FrameBuffer::Resize() not implemented");
    }

    void SetTexture(int layer, Texture2D* tex)
    {
        tex->Resize(width, height);

        Bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + layer, tex->glIndex(), 0);
        
        textures.push_back(tex);
        GLenum* drawBuffers = new GLenum[textures.size()];
        for (unsigned i = 0; i < textures.size(); ++i)
            drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        glDrawBuffers(textures.size(), drawBuffers);
        delete[] drawBuffers;
    }

    void Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    }
private:
    unsigned width, height;

    GLuint framebuffer;
    GLuint depthBuffer;

    std::vector<Texture2D*> textures;
};

class Mesh : public IMesh
{
public:
    Mesh()
        : indexCount(0), offset(0)
    {}

    void Render()
    {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)offset);
    }
private:
    int indexCount;
    int offset;
};

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
    Au::GFX::RenderState* renderState;
    Au::GFX::Mesh* mesh;

    Texture2D* tex;
    FrameBuffer* frameBuffer;
};

#endif
