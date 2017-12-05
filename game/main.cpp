#include <iostream>
#include <gfxm.h>

#include "scene_object.h"

class Transform : public Component
{
public:
    Transform(SceneObject* so)
    {
        ISpatialSystem* spatial = SceneObject::CoreInterface()->GetSpatialSystem();
        tnode = spatial->CreateNode();
    }
    ~Transform()
    {
        SceneObject::CoreInterface()->GetSpatialSystem()->DestroyNode(tnode);
    }

    void Translate(float x, float y, float z) {
        tnode->Translate(x, y, z);
    }
    void Translate(const gfxm::vec3& vec) {
        tnode->Translate(vec.x, vec.y, vec.z);
    }
    void Rotate(float angle, float axisX, float axisY, float axisZ) { 
        tnode->AngleAxis(angle, axisX, axisY, axisZ);
    }
    void Rotate(float angle, const gfxm::vec3& axis) { 
        tnode->AngleAxis(angle, axis.x, axis.y, axis.z);
    }
    void Rotate(const gfxm::quat& q) { 
        tnode->Rotate(q.x, q.y, q.z, q.w); 
    }

    void Position(float x, float y, float z) { 
        tnode->Position(x, y, z);
    }
    void Position(const gfxm::vec3& position) { 
        tnode->Position(position.x, position.y, position.z);
    }
    void Rotation(float x, float y, float z) { 
        tnode->RotationEuler(x, y, z); 
    }
    void Rotation(const gfxm::quat& rotation) { 
        tnode->Rotation(rotation.x, rotation.y, rotation.z, rotation.w);
    }
    void Rotation(float x, float y, float z, float w) { 
        tnode->Rotation(x, y, z, w);
    }
    void Scale(float scale) { 
        tnode->Scale(scale); 
    }
    void Scale(float x, float y, float z) { 
        tnode->Scale(x, y, z);
    }
    void Scale(const gfxm::vec3& scale) { 
        tnode->Scale(scale.x, scale.y, scale.z);
    }

    //gfxm::vec3 WorldPosition();
    //gfxm::vec3 Position() { return transform.position(); }
    //gfxm::quat Rotation() { return transform.rotation(); }
    //gfxm::vec3 Scale() { return transform.scale(); }
    /*
    gfxm::vec3 Right() { return transform.right(); }
    gfxm::vec3 Up() { return transform.up(); }
    gfxm::vec3 Back() { return transform.back(); }
    gfxm::vec3 Left() { return transform.left(); }
    gfxm::vec3 Down() { return transform.down(); }
    gfxm::vec3 Forward() { return transform.forward(); }
    */
    void SetTransform(gfxm::mat4& t) { 
        tnode->SetTransform((float*)&t);
    }
    gfxm::mat4 GetTransform() {
        gfxm::mat4 m;
        tnode->GetTransform((float*)&m);
        return m;
    }

    ITransformNode* GetTransformNode() { return tnode; }
private:
    ITransformNode* tnode;
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
        t = so->GetComponent<Transform>();
        GfxRoot* mr = so->GetRoot()->GetComponent<GfxRoot>();
        mo = mr->GetRenderScene()->CreateMeshObject();
        mo->mesh = g_mesh;
        mo->transform = t->GetTransformNode();
        
        mr->AddMesh(this);
    }

    ~Mesh()
    {
        Object()->GetRoot()->GetComponent<GfxRoot>()->GetRenderScene()->DestroyMeshObject(mo);
        Object()->GetRoot()->GetComponent<GfxRoot>()->RemoveMesh(this);
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
        gfxScene->SetCurrentCamera(this);
        t = so->GetComponent<Transform>();

        SetRenderTarget(SceneObject::CoreInterface()->GetRenderSystem()->DefaultRenderTarget());
        Perspective(1.5f, 16.0f / 9.0f, 0.01f, 100.0f);
    }

    void Perspective(float fov, float aspect, float znear, float zfar)
    {
        gfxm::perspective(projection, fov, aspect, znear, zfar);
        std::copy((float*)&projection, (float*)&projection + 16, vp->projection);
    }

    void SetRenderTarget(IRenderTarget* rt)
    {
        vp = rt->GetViewpoint();
        vp->renderScene = gfxScene->GetRenderScene();
        vp->view = t->GetTransformNode();
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

