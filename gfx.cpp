#include "gfx.h"

#include <iostream>
#include <aurora/window.h>
#include "gl/glextutil.h"

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

bool Gfx::Init(Au::Window& window)
{
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    deviceContext = GetDC(window);

    int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);

    if (!pixelFormat)
        return false;

    if (!SetPixelFormat(deviceContext, pixelFormat, &pfd))
        return false;

    HGLRC renderingContext = wglCreateContext(deviceContext);
    wglMakeCurrent(deviceContext, renderingContext);

    bool core_profile = false;
    WGLEXTLoadFunctions();
    if (!wglCreateContextAttribsARB)
    {
        context = renderingContext;
        threadingContext = wglCreateContext(deviceContext);
        wglMakeCurrent(NULL, NULL);
        wglShareLists(threadingContext, context);
        wglMakeCurrent(deviceContext, context);
    }
    else
    {
        int attr[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 5,
            WGL_CONTEXT_FLAGS_ARB, 0,
            0
        };

        HGLRC renderingContext3plus = wglCreateContextAttribsARB(deviceContext, 0, attr);

        if (!renderingContext3plus)
        {
            context = renderingContext;
            threadingContext = wglCreateContext(deviceContext);
            wglMakeCurrent(NULL, NULL);
            wglShareLists(threadingContext, context);
            wglMakeCurrent(deviceContext, context);
        }
        else
        {
            context = renderingContext3plus;
            threadingContext = wglCreateContextAttribsARB(deviceContext, context, attr);
            wglMakeCurrent(NULL, NULL);
            wglDeleteContext(renderingContext);
            wglMakeCurrent(deviceContext, context);
            core_profile = true;
        }
    }

    GLEXTLoadFunctions();

    if (core_profile)
    {
        //For newer context at least one vao is required
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    int version[2];
    glGetIntegerv(GL_MAJOR_VERSION, &version[0]);
    glGetIntegerv(GL_MINOR_VERSION, &version[1]);
    contextVersion = version[0] * 100 + version[1] * 10;

    std::cout << "OpenGL v" << APIVersion() << " ready.\n";

    std::cout << "GLSL v" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

    //=======================================================
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    wglSwapIntervalEXT(0);

    return true;
}

void Gfx::Cleanup()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(context);
}

int Gfx::APIVersion()
{
    return contextVersion;
}

Camera* Gfx::CreateCamera()
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
        std::cout << c << std::endl;

    }
}