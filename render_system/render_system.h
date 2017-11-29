#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "../icore.h"
#include "../irender_system.h"

#include <aurora/window.h>
#include <aurora/gfx.h>

#include <gfxm.h>

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
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    ~Texture2D()
    {
        glDeleteTextures(1, &texture);
    }

    GLuint glIndex() { return texture; }

    void Resize(unsigned width, unsigned height)
    {
        Bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, fmt, GL_UNSIGNED_BYTE, data);
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
        glViewport(0, 0, width, height);
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
        : indexCount(0), offset(0), vertexSize(20)
    {
        glGenVertexArrays(1, &vao);
        Bind();

        // =====
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
        
        glGenBuffers(1, &uvBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        std::vector<float> vertexData = {
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f
        };

        std::vector<float> uvData = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            0.0f, 0.0f,
            0.0f, 0.0f
        };

        std::vector<unsigned short> indexData = {
            0, 2, 1, 0, 3, 2,
            4, 5, 6, 4, 6, 7,
            0, 5, 4, 0, 1, 5,
            2, 6, 3, 2, 7, 6,
            0, 7, 2, 0, 4, 7,
            1, 6, 5, 1, 3, 6
        };

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), (void*)vertexData.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
        glBufferData(GL_ARRAY_BUFFER, uvData.size() * sizeof(float), (void*)uvData.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(unsigned short), (void*)indexData.data(), GL_STATIC_DRAW);

        indexCount = indexData.size();
        offset = 0;

        glBindVertexArray(0);
    }

    ~Mesh()
    {
        glDeleteBuffers(1, &indexBuffer);
        glDeleteBuffers(1, &uvBuffer);
        glDeleteBuffers(1, &vertexBuffer);

        glDeleteVertexArrays(1, &vao);
    }

    void Bind()
    {
        glBindVertexArray(vao);
    }

    void Render()
    {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)offset);
    }
private:
    GLuint vao;
    GLuint vertexBuffer;
    GLuint uvBuffer;
    GLuint indexBuffer;

    int indexCount;
    int offset;

    int vertexSize;
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
    Texture2D* diffuseBuffer;
    FrameBuffer* frameBuffer;
};

#endif
