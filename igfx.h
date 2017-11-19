#ifndef IGFX_H
#define IGFX_H

#include <gfxm.h>

struct Attribute;
class IMesh
{
public:
    virtual void SetFormat(const char* fmtdesc) = 0;
    virtual void SetAttribData(const char* attribName, const unsigned char* data, size_t sz) = 0;
    virtual void SetIndices(const unsigned short* data, int count) = 0;
};

class IGfx
{
public:
    enum ELEM_TYPE
    {
        BYTE,
        UBYTE,
        SHORT,
        USHORT,
        INT,
        UINT,
        //  GL_HALF_FLOAT
        FLOAT,
        DOUBLE
        // GL_FIXED
        // GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and GL_UNSIGNED_INT_10F_11F_11F_REV
    };

    struct Camera
    {
        Camera()
            : transform(1.0f), projection(1.0f), renderTarget(0)
        {}
        gfxm::mat4 transform;
        gfxm::mat4 projection;
        int renderTarget;
    };

    virtual void SetAttribute(const char* name, ELEM_TYPE elemType, int elemCount) = 0;
    virtual Attribute* GetAttribute(const char* name) = 0;

    virtual IMesh* CreateMesh() = 0;
    virtual void DestroyMesh(IMesh* mesh) = 0;

    virtual Camera* CreateCamera() = 0;
    virtual void DestroyCamera(Camera* camera) = 0;
};

struct Attribute
{
    char name[32];
    IGfx::ELEM_TYPE elemType;
    int elemSize;
    int elemCount;

    bool operator==(const Attribute& other)
    {
        if (strncmp(other.name, name, 32) != 0)
            return false;
        if (other.elemType != elemType)
            return false;
        if (other.elemSize != elemSize)
            return false;
        if (other.elemCount != elemCount)
            return false;
        return true;
    }
};

#endif
