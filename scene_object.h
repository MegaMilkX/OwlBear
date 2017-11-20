#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include "typeindex.h"

#include <string>

template <typename T>
inline const char* typeName(void) { return "unknown"; }

class Component
{

};

class SceneObject
{
public:
    virtual void            SetName(const char* name) = 0;
    void                    SetName(const std::string& name) { SetName(name.c_str()); }
    virtual const char*     GetName() = 0;
    virtual SceneObject*    GetRoot() = 0;
    virtual bool            IsRoot() = 0;
    virtual SceneObject*    CreateObject() = 0;
    virtual SceneObject*    FindObject(const char* name) = 0;
    SceneObject*            FindObject(const std::string& name) { return FindObject(name.c_str()); }

    template<typename T>
    T*                      GetComponent();
    template<typename T>
    T*                      FindComponent();
private:
    virtual Component*      GetComponent(const char* typeName) = 0;
    virtual Component*      FindComponent(const char* typeName) = 0;
};

template<typename T>
T* SceneObject::GetComponent()
{
    return (T*)GetComponent(typeName<T>());
}

template<typename T>
T* SceneObject::FindComponent()
{
    return (T*)FindComponent(typeName<T>());
}

#endif
