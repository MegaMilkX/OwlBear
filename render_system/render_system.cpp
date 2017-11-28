#include "render_system.h"

bool RenderSystem::Init(HWND window)
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

    renderState = new Au::GFX::RenderState();
    Au::GFX::Shader* vshader = new Au::GFX::Shader(Au::GFX::Shader::VERTEX);
    Au::GFX::Shader* fshader = new Au::GFX::Shader(Au::GFX::Shader::PIXEL);
    vshader->Source(R"(#version 330
            in vec3 Position;
            in vec3 ColorRGB;
            in vec2 UV;
            out vec3 fColorRGB;
            out vec2 fUV;
            void main()
            {
                fColorRGB = ColorRGB;
                fUV = UV;
                gl_Position = vec4(Position, 1.0);
            }
        )");
    fshader->Source(R"(#version 330
            uniform sampler2D Diffuse;
            in vec3 fColorRGB;
            in vec2 fUV;
            out vec4 color;
            void main()
            {
                color = vec4(texture2D(Diffuse, fUV).xyz, 1.0);
            }
        )");
    std::cout << vshader->StatusString() << std::endl;
    std::cout << fshader->StatusString() << std::endl;

    renderState->SetShader(vshader);
    renderState->SetShader(fshader);
    std::cout << renderState->StatusString() << std::endl;
    renderState->AttribFormat(Au::Position() << Au::ColorRGB());

    mesh = new Au::GFX::Mesh();
    mesh->Format(Au::Position() << Au::ColorRGB() << Au::UV());
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
    std::vector<float> uvData = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    std::vector<unsigned short> indexData = {
        0, 1, 2, 0, 2, 3
    };
    mesh->VertexAttribByInfo(Au::Position(), (unsigned char*)vertexData.data(), vertexData.size() * sizeof(float));
    mesh->VertexAttribByInfo(Au::ColorRGB(), (unsigned char*)colorData.data(), colorData.size() * sizeof(float));
    mesh->VertexAttribByInfo(Au::UV(), (unsigned char*)uvData.data(), uvData.size() * sizeof(float));
    mesh->IndexData(indexData);   

    renderState->AddSampler2D("Diffuse", 0);

    tex = new Texture2D();
    stbi_set_flip_vertically_on_load(1);
    int bpp = 0;
    int width = 0;
    int height = 0;
    unsigned char* texData = 0;
    texData = stbi_load("test.jpg", &width, &height, &bpp, 3);
    tex->Upload(texData, width, height, bpp);
    stbi_image_free(texData);

    frameBuffer = new FrameBuffer(800, 600);
    //frameBuffer->SetTexture(0, tex);

    std::cout << "RenderSystem start!" << std::endl;

    return true;
}

void RenderSystem::Cleanup()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(context);
    std::cout << "RenderSystem cleanup!" << std::endl;

    delete this;
}

void RenderSystem::ResizeCanvas(int width, int height)
{
    glViewport(0, 0, width, height);
}

void RenderSystem::Update()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (Mesh* m : meshes)
    {
        m->Render();
    }
    
    RenderToScreen(tex);

    // For each camera
    // Get camera's renderTarget
    // Get camera's renderSpace
    // For each renderable in renderSpace
    // 
}

int RenderSystem::APIVersion()
{
    return contextVersion;
}

IMesh* RenderSystem::CreateMesh()
{
    Mesh* m = new Mesh();
    meshes.insert(m);
    return m;
}

void RenderSystem::DestroyMesh(IMesh* mesh)
{
    meshes.erase((Mesh*)mesh);
}

void RenderSystem::RenderToScreen(Texture2D* texture)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    renderState->Bind();
    texture->Bind(0);
    mesh->Bind();
    mesh->Render(mesh->IndexCount(), 0);

    SwapBuffers(deviceContext);
}