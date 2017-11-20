#include "gfx.h"

bool RenderTarget::Init()
{
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &colorBuffer);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, colorBuffer);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 768);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    return true;
}

void RenderTarget::Cleanup()
{
    glDeleteRenderbuffers(1, &depthBuffer);
    glDeleteTextures(1, &colorBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
}
static int shaderProgram;
bool Gfx::Init(Au::Window& window)
{
    
}

void Gfx::Cleanup()
{
    
}

int Gfx::APIVersion()
{
    return 0;
}

void Gfx::SetAttribute(const char* name, ELEM_TYPE elemType, int elemCount)
{
    Attribute attrib;
    strncpy_s(attrib.name, name, 31);
    attrib.elemType = elemType;
    attrib.elemCount = elemCount;
    attrib.elemSize = elemSizes[elemType];

    attributes.insert(std::make_pair(std::string(name), attrib));
}

Attribute* Gfx::GetAttribute(const char* name)
{
    std::map<std::string, Attribute>::iterator it =
        attributes.find(std::string(name));
    if (it == attributes.end())
    {
        std::cout << "Attribute '" << std::string(name) << "' doesn't exist" << std::endl;
        return 0;
    }
    
    return &it->second;
}

IMesh* Gfx::CreateMesh()
{
    Mesh* m = new Mesh(this);
    meshes.insert(m);
    return m;
}

void Gfx::DestroyMesh(IMesh* mesh)
{
    meshes.erase((Mesh*)mesh);
}

Gfx::Camera* Gfx::CreateCamera()
{
    Camera* cam = new Camera();
    cameras.insert(cam);
    return cam;
}

void Gfx::DestroyCamera(Camera* camera)
{
    cameras.erase(camera);
}

void Gfx::Render()
{
    std::set<Camera*>::iterator it = cameras.begin();
    for (it; it != cameras.end(); ++it)
    {
        Camera* c = (*it);

    }

    

    
}