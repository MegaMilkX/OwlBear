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

    frameBuffer = new FrameBuffer(640, 480);
    diffuseBuffer = new Texture2D();
    frameBuffer->SetTexture(0, diffuseBuffer);

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
    projection = gfxm::perspective(projection, 1.5f, 16.0f/9.0f, 0.01f, 100.0f);
    model.rotate(0.0001f, gfxm::vec3(0.0f, 1.0f, 0.0f));
    view.position(0.0f, 0.7f, 2.0f);

    frameBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (Mesh* m : meshes)
    {
        sp_model->Bind();
        //glUniformMatrix4fv(0, 1, GL_FALSE, &projection[0][0]);
        //glUniformMatrix4fv(1, 1, GL_FALSE, &gfxm::inverse(view.matrix())[0][0]);
        //glUniformMatrix4fv(2, 1, GL_FALSE, &model.matrix()[0][0]);
        tex->Bind(0);
        m->Bind();
        m->Render();
    }
    

    sp_model->Bind();
    glUniformMatrix4fv(sp_model->UniformLocation("projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(sp_model->UniformLocation("view"), 1, GL_FALSE, &gfxm::inverse(view.matrix())[0][0]);
    glUniformMatrix4fv(sp_model->UniformLocation("model"), 1, GL_FALSE, &model.matrix()[0][0]);
    tex->Bind(0);
    cube->Bind();
    cube->Render();

    RenderToScreen(diffuseBuffer);

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
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glViewport(0, 0, 1280, 720);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram->Bind();
    texture->Bind(0);
    screenQuad->Bind();
    screenQuad->Render();

    SwapBuffers(deviceContext);
}