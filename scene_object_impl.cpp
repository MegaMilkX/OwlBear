#include "scene_object_impl.h"

#include "game_system_library.h"

void SceneObjectImpl::SetName(const char* name)
{
    this->name = std::string(name);
}

const char* SceneObjectImpl::GetName()
{
    return name.c_str();
}

SceneObject* SceneObjectImpl::GetRoot()
{
    return this;
}

bool SceneObjectImpl::IsRoot()
{
    return true;
}

SceneObject* SceneObjectImpl::CreateObject()
{
    return 0;
}

SceneObject* SceneObjectImpl::FindObject(const char* name)
{
    return 0;
}

// === Private =================

Component* SceneObjectImpl::GetComponent(const char* typeName)
{
    std::map<std::string, Component*>::iterator it =
        components.find(typeName);
    if (it == components.end())
    {
        Component* c = ModuleLibrary::CreateComponent(this, typeName);
        if (c)
        {
            components.insert(std::make_pair(std::string(typeName), c));
            return c;
        }
        return 0;
    }
    else
    {
        return it->second;
    }
}

Component* SceneObjectImpl::FindComponent(const char* typeName)
{
    std::map<std::string, Component*>::iterator it =
        components.find(typeName);
    if (it == components.end())
    {
        return 0;
    }
    else
    {
        return it->second;
    }
}
