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

    shaderProgram = new ShaderProgram();
    shaderProgram->VertexShader(
#include "vs_screen.glsl"
    );
    shaderProgram->FragmentShader(
#include "fs.glsl"
    );
    shaderProgram->Link();

    sp_model = new ShaderProgram();
    sp_model->VertexShader(
#include "vs.glsl"
    );
    sp_model->FragmentShader(
#include "fs.glsl"
    );
    sp_model->Link();

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

    vertexData = {
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f, -0.5f },
        { -0.5f,  0.5f, 0.5f },
        { 0.5f,  0.5f, 0.5f },
        { 0.5f,  0.5f, -0.5f },
        { -0.5f,  0.5f, -0.5f }
    };

    uvData = {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
        { 0.0f, 0.0f },
        { 1.0f, 0.0f }
    };

    indexData = {
        0, 2, 1, 0, 3, 2,
        4, 5, 6, 4, 6, 7,
        0, 5, 4, 0, 1, 5,
        2, 3, 7, 2, 7, 6,
        0, 7, 3, 0, 4, 7,
        1, 6, 5, 1, 2, 6
    };

    cube = new Mesh();
    cube->SetPositionData(vertexData);
    cube->SetUVData(uvData);
    cube->SetIndexData(indexData);

    tex = new Texture2D();
    stbi_set_flip_vertically_on_load(1);
    int bpp = 0;
    int width = 0;
    int height = 0;
    unsigned char* texData = 0;
    texData = stbi_load("test.jpg", &width, &height, &bpp, 3);
    tex->Upload(texData, width, height, bpp);
    stbi_image_free(texData);

    deferredFrameBuffer = new DeferredFrameBuffer();

    windowRenderTarget = new RenderTarget(1280, 720);

    std::cout << "RenderSystem start!" << std::endl;

    return true;
}

void RenderSystem::Cleanup()
{
    delete windowRenderTarget;
    delete deferredFrameBuffer;

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(context);
    std::cout << "RenderSystem cleanup!" << std::endl;

    delete this;
}

void RenderSystem::ResizeCanvas(int width, int height)
{
    glViewport(0, 0, width, height);
}

void RenderSystem::UpdateRenderTarget(RenderTarget* rt)
{
    deferredFrameBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sp_model->Bind();
    glUniformMatrix4fv(sp_model->UniformLocation("projection"), 1, GL_FALSE, rt->GetViewpoint()->projection);
    glUniformMatrix4fv(sp_model->UniformLocation("view"), 1, GL_FALSE, rt->GetViewpoint()->view);
    RenderScene* scene = (RenderScene*)rt->GetViewpoint()->renderScene;
    for (MeshObject* mo : scene->meshObjects)
    {
        glUniformMatrix4fv(sp_model->UniformLocation("model"), 1, GL_FALSE, mo->transform);
        tex->Bind(0);
        cube->Bind();
        cube->Render();
    }

    deferredFrameBuffer->Render(rt->GetFrameBuffer());
}

void RenderSystem::Update()
{    
    for (RenderTarget* rt : renderTargets)
        UpdateRenderTarget(rt);
    UpdateRenderTarget(windowRenderTarget);

    RenderToScreen(windowRenderTarget->GetColorBuffer());
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

IRenderScene* RenderSystem::CreateScene()
{
    RenderScene* s = new RenderScene();
    scenes.insert(s);
    return s;
}

void RenderSystem::DestroyScene(IRenderScene* scene)
{
    scenes.erase((RenderScene*)scene);
}

void RenderSystem::RenderToScreen(Texture2D* texture)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glViewport(0, 0, 1280, 720);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram->Bind();
    texture->Bind(0);
    screenQuad->Bind();
    screenQuad->Render();

    SwapBuffers(deviceContext);
}