#ifndef ISPATIAL_SYSTEM_H
#define ISPATIAL_SYSTEM_H

class ITransformNode
{
public:
    virtual void Translate(float x, float y, float z) = 0;
    virtual void Rotate(float x, float y, float z, float w) = 0;
    virtual void AngleAxis(float a, float x, float y, float z) = 0;

    virtual void Position(float x, float y, float z) = 0;
    virtual void RotationEuler(float x, float y, float z) = 0;
    virtual void Rotation(float x, float y, float z, float w) = 0;
    virtual void Scale(float scale) = 0;
    virtual void Scale(float x, float y, float z) = 0;
    
    virtual void SetTransform(float* mat4) = 0;
    virtual void GetTransform(float* out_mat4) = 0;
};

class ICore;
class ISpatialSystem
{
public:
    virtual bool Init() = 0;
    virtual void Cleanup() = 0;

    virtual ITransformNode* CreateNode() = 0;
    virtual void DestroyNode(ITransformNode* o) = 0;
};

#define DLLEXPORT __declspec(dllexport)
extern "C"
{
    DLLEXPORT ISpatialSystem* SpatialInit(ICore* core);
}
#undef DLLEXPORT

#endif
