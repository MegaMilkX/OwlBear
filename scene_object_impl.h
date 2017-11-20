#ifndef SCENE_OBJECT_IMPL_H
#define SCENE_OBJECT_IMPL_H

#include "scene_object.h"
#include "icore.h"

#include <string>
#include <vector>
#include <map>

class SceneObjectImpl : public SceneObject
{
public:
    SceneObjectImpl(ICore* core) 
        : core(core)
    {}

    virtual void            SetName(const char* name);
    virtual const char*     GetName();
    virtual SceneObject*    GetRoot();
    virtual bool            IsRoot();
    virtual SceneObject*    CreateObject();
    virtual SceneObject*    FindObject(const char* name);
private:
    virtual Component*      GetComponent(const char* typeName);
    virtual Component*      FindComponent(const char* typeName);

    ICore* core;

    std::string name;
    SceneObject* parent;
    std::vector<SceneObject*> objects;
    std::map<std::string, Component*> components;
};

#endif
