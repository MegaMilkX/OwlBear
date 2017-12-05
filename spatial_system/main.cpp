#include "../ispatial_system.h"

#include <gfxm.h>
#include <algorithm>
#include <set>
#include <iostream>

class Transform : public ITransformNode
{
public:
    Transform()
    {

    }

    virtual void Translate(float x, float y, float z)
    {
        t.translate(x, y, z);
    }
    virtual void Rotate(float x, float y, float z, float w)
    {
        t.rotate(gfxm::quat(x, y, z, w));
    }
    virtual void AngleAxis(float a, float x, float y, float z)
    {
        t.rotate(a, x, y, z);
    }

    virtual void Position(float x, float y, float z)
    {
        t.position(x, y, z);
    }
    virtual void RotationEuler(float x, float y, float z)
    {
        t.rotation(x, y, z);
    }
    virtual void Rotation(float x, float y, float z, float w)
    {
        t.rotation(x, y, z, w);
    }
    virtual void Scale(float scale)
    {
        t.scale(scale);
    }
    virtual void Scale(float x, float y, float z)
    {
        t.scale(x, y, z);
    }

    virtual void SetTransform(float* mat4)
    {
        t.set_transform(*(gfxm::mat4*)mat4);
    }
    virtual void GetTransform(float* out_mat4)
    {
        if (!out_mat4)
            return;
        gfxm::mat4 m = t.matrix();
        std::copy((float*)&m, (float*)&m + 16, out_mat4);
    }
private:
    gfxm::transform t;
};

class Spatial : public ISpatialSystem
{
public:
    bool Init()
    {
        std::cout << "Spatial system start!" << std::endl;
        return true;
    }

    void Cleanup()
    {
        delete this;
    }

    ITransformNode* CreateNode()
    {
        ITransformNode* n = new Transform();
        nodes.insert(n);
        return n;
    }
    void DestroyNode(ITransformNode* n)
    {
        nodes.erase(n);
        delete n;
    }
private:
    std::set<ITransformNode*> nodes;
};

Spatial* spatial = 0;
ISpatialSystem* SpatialInit(ICore* core)
{
    spatial = new Spatial();
    return spatial;
}