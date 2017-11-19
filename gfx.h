#ifndef GFX_H
#define GFX_H

#include <Windows.h>
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <aurora/window.h>
#include "gl/glextutil.h"
#include "gl/glbuffer.h"
#include <aurora/gfx.h>

#include "util.h"

#include "igfx.h"

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

namespace Au { class Window; }

struct VertexFormat
{
    VertexFormat()
        : attributes(0), attribCount(0), vertexSize(0)
    {

    }
    VertexFormat(const char* fmt, IGfx* gfx)
        : attributes(0), attribCount(0), vertexSize(0)
    {
        std::string fmtdesc(fmt);
        std::vector<std::string> tokens = split(fmtdesc, "/");
        std::vector<Attribute*> attribs;
        for (auto& tok : tokens)
        {
            std::cout << tok << std::endl;
            Attribute* attr = gfx->GetAttribute(tok.c_str());
            if (!attr)
                continue;
            attribs.push_back(attr);

            vertexSize += attr->elemCount * attr->elemSize;
        }

        attributes = new Attribute[attribs.size()];
        attribCount = attribs.size();
        for (int i = 0; i < attribCount; ++i)
        {
            attributes[i] = *attribs[i];
        }
    }
    ~VertexFormat()
    {
        if (attributes)
            delete[] attributes;
    }

    Attribute* attributes;
    int attribCount;
    int vertexSize;
};

class Gfx;
class Mesh : public IMesh
{
public:
    Mesh(Gfx* gfx);
    ~Mesh();
    virtual void SetFormat(const char* fmtdesc);
    virtual void SetAttribData(const char* attribName, const unsigned char* data, size_t sz);
    virtual void SetIndices(const unsigned short* data, int count);

    void Bind();

    void Render() { Render(indexCount, 0); }
    void Render(unsigned int indexCount, unsigned int offset);
private:
    void BlitAttribDataToBuffer(Attribute attrib, const unsigned char* attribData, int attribDataSize, const unsigned char* dest, int destSize);

    Gfx* gfx;

    VertexFormat vfmt;

    unsigned int vao;
    Au::GFX::GLBuffer vertexBuffer;
    Au::GFX::GLBuffer indexBuffer;
    unsigned int indexCount;
};

class Gfx : public IGfx
{
public:
    bool Init(Au::Window& window);
    void Cleanup();
    int APIVersion();

    virtual void SetAttribute(const char* name, ELEM_TYPE elemType, int elemCount);
    virtual Attribute* GetAttribute(const char* name);

    virtual IMesh* CreateMesh();
    virtual void DestroyMesh(IMesh* mesh);

    virtual Camera* CreateCamera();
    virtual void DestroyCamera(Camera* camera);

    void Render();

    GLenum ElemToGLenum(ELEM_TYPE elem) { return elemGL[elem]; }
private:
    std::map<std::string, Attribute> attributes;

    std::map<ELEM_TYPE, int> elemSizes;
    std::map<ELEM_TYPE, GLenum> elemGL;

    std::set<Mesh*> meshes;
    std::set<Camera*> cameras;

    HDC deviceContext;
    HGLRC context;
    HGLRC threadingContext;
    int contextVersion = 0;

    // Temp
    Au::GFX::RenderState* renderState;
    Au::GFX::Mesh* mesh;
};

#endif

