#include <iostream>
#include <gfxm.h>

#include "scene_object.h"

class Transform : public Component
{
public:
    Transform(SceneObject* so)
    {

    }

    void Translate(float x, float y, float z) { transform.translate(x, y, z); }
    void Translate(const gfxm::vec3& vec) { transform.translate(vec); }
    void Rotate(float angle, float axisX, float axisY, float axisZ) { transform.rotate(angle, axisX, axisY, axisZ); }
    void Rotate(float angle, const gfxm::vec3& axis) { transform.rotate(angle, axis); }
    void Rotate(const gfxm::quat& q) { transform.rotate(q); }

    void LookAt(const gfxm::vec3& target, const gfxm::vec3& forward, const gfxm::vec3& up = gfxm::vec3(0.0f, 1.0f, 0.0f), float f = 1.0f)
    {
        transform.look_at(target, forward, up, f);
    }

    void Position(float x, float y, float z) { transform.position(x, y, z); }
    void Position(const gfxm::vec3& position) { transform.position(position); }
    void Rotation(float x, float y, float z) { transform.rotation(x, y, z); }
    void Rotation(const gfxm::quat& rotation) { transform.rotation(rotation); }
    void Rotation(float x, float y, float z, float w) { transform.rotation(x, y, z, w); }
    void Scale(float scale) { transform.scale(scale); }
    void Scale(float x, float y, float z) { transform.scale(x, y, z); }
    void Scale(const gfxm::vec3& scale) { transform.scale(scale); }

    //gfxm::vec3 WorldPosition();
    gfxm::vec3 Position() { return transform.position(); }
    gfxm::quat Rotation() { return transform.rotation(); }
    gfxm::vec3 Scale() { return transform.scale(); }

    gfxm::vec3 Right() { return transform.right(); }
    gfxm::vec3 Up() { return transform.up(); }
    gfxm::vec3 Back() { return transform.back(); }
    gfxm::vec3 Left() { return transform.left(); }
    gfxm::vec3 Down() { return transform.down(); }
    gfxm::vec3 Forward() { return transform.forward(); }

    void SetTransform(gfxm::mat4& t) { transform.set_transform(t); }
    //Au::Math::Mat4f GetLocalTransform();
    gfxm::mat4 GetTransform() { return transform.matrix(); }
private:
    gfxm::transform transform;
};

IMesh* g_mesh = 0;
class Mesh;
class Camera;
class GfxRoot : public Component
{
public:
    GfxRoot(SceneObject* so)
        :camera(0)
    {
        IRenderSystem* rs = SceneObject::CoreInterface()->GetRenderSystem();
        renderScene = rs->CreateScene();
        g_mesh = rs->CreateMesh();
    }

    IRenderScene* GetRenderScene() { return renderScene; }
    void AddMesh(Mesh*);
    void RemoveMesh(Mesh*);
    void SetCurrentCamera(Camera*);
    void Update();
private:
    std::set<Mesh*> meshes;
    Camera* camera;
    IRenderScene* renderScene;
};

class Mesh : public Component
{
public:
    Mesh(SceneObject* so)
    {
        GfxRoot* mr = so->GetRoot()->GetComponent<GfxRoot>();
        mo = mr->GetRenderScene()->CreateMeshObject();
        mo->mesh = g_mesh;
        t = so->GetComponent<Transform>();
        mr->AddMesh(this);
        UpdateTransform();
    }

    ~Mesh()
    {
        Object()->GetRoot()->GetComponent<GfxRoot>()->GetRenderScene()->DestroyMeshObject(mo);
        Object()->GetRoot()->GetComponent<GfxRoot>()->RemoveMesh(this);
    }

    void UpdateTransform()
    {
        gfxm::mat4& mat = t->GetTransform();
        std::copy((float*)&mat, (float*)&mat + 16, mo->transform);
    }
private:
    MeshObject* mo;
    Transform* t;
};

class Camera : public Component
{
public:
    Camera(SceneObject* so)
        :vp(0)
    {
        gfxScene = so->GetRoot()->GetComponent<GfxRoot>();
        projection = gfxm::perspective(projection, 1.5f, 16.0f / 9.0f, 0.01f, 100.0f);
        t = so->GetComponent<Transform>();

        gfxScene->SetCurrentCamera(this);

        SetRenderTarget(SceneObject::CoreInterface()->GetRenderSystem()->DefaultRenderTarget());
    }

    void SetRenderTarget(IRenderTarget* rt)
    {
        vp = rt->GetViewpoint();
        vp->renderScene = gfxScene->GetRenderScene();
    }

    void Update()
    {
        if (!vp)
            return;
        gfxm::mat4& mat = gfxm::inverse(t->GetTransform());
        std::copy((float*)&mat, (float*)&mat + 16, vp->view);
        std::copy((float*)&projection, (float*)&projection + 16, vp->projection);
    }
private:
    Transform* t;
    GfxRoot* gfxScene;
    Viewpoint* vp;
    gfxm::mat4 projection;
};

void GfxRoot::AddMesh(Mesh* m)
{
    meshes.insert(m);
}
void GfxRoot::RemoveMesh(Mesh* m)
{
    meshes.erase(m);
}

void GfxRoot::SetCurrentCamera(Camera* c)
{
    camera = c;
}

void GfxRoot::Update()
{
    for (auto m : meshes)
    {
        m->UpdateTransform();
    }
    camera->Update();
}


class Gameplay : public GameState
{
public:
    Gameplay(ICore* core)
        : core(core)
    {
        SceneObject::Init(core);

        scene.CreateObject();
        mesh = scene.CreateObject()->GetComponent<Mesh>();
        auto m2 = scene.CreateObject()->GetComponent<Mesh>();
        m2->GetComponent<Transform>()->Translate(1.0f, 0.7f, -0.5f);
        m2 = scene.CreateObject()->GetComponent<Mesh>();
        m2->GetComponent<Transform>()->Translate(-1.0f, 0.5f, 0.7f);

        m2 = scene.CreateObject()->GetComponent<Mesh>();
        m2->GetComponent<Transform>()->Translate(0.0f, 0.0f, -3.0f);

        gfxRoot = scene.GetComponent<GfxRoot>();

        cam = scene.CreateObject()->GetComponent<Camera>();
        cam->GetComponent<Transform>()->Translate(0.0f, 1.0f, 2.0f);
    }
    ~Gameplay() {}

    void Init() {}
    void Cleanup() {}
    void Switch() {}

    bool Update()
    {
        mesh->GetComponent<Transform>()->Rotate(0.0001f, gfxm::vec3(0.0f, 1.0f, 0.0f));

        gfxRoot->Update();        
        return true;
    }
private:
    ICore* core;

    Mesh* mesh;
    Camera* cam;
    GfxRoot* gfxRoot;
    SceneObject scene;
};

Gameplay* gameplay = 0;



int GameInit(ICore* core)
{/*
    renderSystem = (RenderSystem*)core->LoadRenderSystem("render_system.dll");
    if (!renderSystem)
        return 1;
        */
    return 0;
}

GameState* GameStart(ICore* core)
{
    gameplay = new Gameplay(core);
    return gameplay;
}

void GameCleanup()
{
    delete gameplay;
}

