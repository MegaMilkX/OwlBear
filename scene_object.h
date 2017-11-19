#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "typeindex.h"

class Component
{

};

class SceneObject
{
public:
    virtual void            SetName(const char* name) = 0;
    virtual char*           GetName() = 0;
    virtual SceneObject*    GetRoot() = 0;
    virtual bool            IsRoot() = 0;
    virtual SceneObject*    CreateObject() = 0;
    virtual SceneObject*    FindObject(const char* name) = 0;

    template<typename T>
    T* GetComponent();
    template<typename T>
    T* FindComponent();
private:
    virtual Component* GetComponent(typeindex type, Component* component) = 0;
    virtual Component* FindComponent(typeindex type) = 0;
};

template<typename T>
T* SceneObject::GetComponent()
{
    return (T*)GetComponent(type_index<T>(), new T());
}

template<typename T>
T* SceneObject::FindComponent()
{
    return (T*)FindComponent(type_index<T>());
}

#endif
