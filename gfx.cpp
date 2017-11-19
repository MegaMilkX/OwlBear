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

    // ============

    elemSizes = {
        { BYTE, 1 },
        { UBYTE, 1 },
        { SHORT, 2 },
        { USHORT, 2 },
        { INT, 4 },
        { UINT, 4 },
        //  GL_HALF_FLOAT
        { FLOAT, 4 },
        { DOUBLE, 8 }
        // GL_FIXED
        // GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and GL_UNSIGNED_INT_10F_11F_11F_REV
    };

    elemGL = {
        { BYTE, GL_BYTE },
        { UBYTE, GL_UNSIGNED_BYTE },
        { SHORT, GL_SHORT },
        { USHORT, GL_UNSIGNED_SHORT },
        { INT, GL_INT },
        { UINT, GL_UNSIGNED_INT },
        { FLOAT, GL_FLOAT },
        { DOUBLE, GL_DOUBLE }
    };

    // ============

    SetAttribute("Vertex", IGfx::FLOAT, 3);
    SetAttribute("Position", IGfx::FLOAT, 3);
    SetAttribute("ColorRGBA", IGfx::UBYTE, 4);
    SetAttribute("ColorRGB", IGfx::UBYTE, 3);
    SetAttribute("ColorGRAYA", IGfx::UBYTE, 2);
    SetAttribute("ColorGRAY", IGfx::UBYTE, 1);
    SetAttribute("Normal", IGfx::BYTE, 3);
    SetAttribute("Tangent", IGfx::BYTE, 3);
    SetAttribute("Bitangent", IGfx::BYTE, 3);
    SetAttribute("UV", IGfx::FLOAT, 2);

    renderState = new Au::GFX::RenderState();
    Au::GFX::Shader* vshader = new Au::GFX::Shader(Au::GFX::Shader::VERTEX);
    Au::GFX::Shader* fshader = new Au::GFX::Shader(Au::GFX::Shader::PIXEL);
    vshader->Source(R"(#version 330
        in vec3 Position;
        in vec3 ColorRGB;
        out vec3 fColorRGB;
        void main()
        {
            fColorRGB = ColorRGB;
            gl_Position = vec4(Position, 1.0);
        }
    )");
    fshader->Source(R"(#version 330
        in vec3 fColorRGB;
        out vec4 color;
        void main()
        {
            color = vec4(fColorRGB.xyz, 1.0);
        }
    )");
    std::cout << vshader->StatusString() << std::endl;
    std::cout << fshader->StatusString() << std::endl;

    renderState->SetShader(vshader);
    renderState->SetShader(fshader);
    std::cout << renderState->StatusString() << std::endl;
    renderState->AttribFormat(Au::Position() << Au::ColorRGB());

    mesh = new Au::GFX::Mesh();
    mesh->Format(Au::Position() << Au::ColorRGB());
    std::vector<float> vertexData = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f
    };
    std::vector<float> colorData = {
        0.5f, 0.8f, 0.5f,
        0.5f, 0.8f, 0.5f,
        0.0f, 0.5f, 0.5f,
        0.0f, 0.5f, 0.5f
    };
    std::vector<unsigned short> indexData = {
        0, 1, 2, 0, 2, 3
    };
    mesh->VertexAttribByInfo(Au::Position(), (unsigned char*)vertexData.data(), vertexData.size() * sizeof(float));
    mesh->VertexAttribByInfo(Au::ColorRGB(), (unsigned char*)colorData.data(), colorData.size() * sizeof(float));
    mesh->IndexData(indexData);

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

    

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderState->Bind();
    mesh->Bind();
    mesh->Render(mesh->IndexCount(), 0);

    SwapBuffers(this->deviceContext);

    // For each camera
        // Get camera's renderTarget
        // Get camera's renderSpace
        // For each renderable in renderSpace
            // 
}