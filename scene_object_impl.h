#ifndef SCENE_OBJECT_IMPL_H
#define SCENE_OBJECT_IMPL_H

#include "scene_object.h"

#include <string>
#include <vector>
#include <map>

class SceneObjectImpl : public SceneObject
{
public:
    virtual void            SetName(const char* name);
    virtual char*           GetName();
    virtual SceneObject*    GetRoot();
    virtual bool            IsRoot();
    virtual SceneObject*    CreateObject();
    virtual SceneObject*    FindObject(const char* name);
private:
    virtual Component* GetComponent(typeindex type, Component* component) = 0;
    virtual Component* FindComponent(typeindex type) = 0;

    std::string name;
    SceneObject* parent;
    std::vector<SceneObject*> objects;
    std::map<typeindex, Component*> components;
};

#endif
