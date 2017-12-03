#ifndef SCENE_OBJECT_H
#define SCENE_OBJECT_H

#include <vector>
#include <set>
#include <map>
#include "../icore.h"

#include "typeindex.h"

class SceneObject;
class Component
{
friend SceneObject;
public:
    virtual ~Component() {}
    SceneObject* Object() { return object; }
    template<typename T>
    T* GetComponent()
    {
        return object->GetComponent<T>();
    }
private:
    SceneObject* object;
};

class SceneObject
{
public:
    static void Init(ICore* core)
    {
        ::SceneObject::core = core;
    }
    static ICore* CoreInterface() { return core; }

    SceneObject() : parent(0) {}
    SceneObject(SceneObject* parent) : parent(parent) {}
    ~SceneObject()
    {
        for (SceneObject* so : objects)
        {
            delete so;
        }
    }

    SceneObject* GetRoot()
    {
        if (!parent)
            return this;
        else
            return parent->GetRoot();
    }

    bool IsRoot()
    {
        return parent == 0;
    }

    SceneObject* CreateObject()
    {
        SceneObject* o = new SceneObject(this);
        objects.insert(o);
        return o;
    }

    template<typename T>
    T* GetComponent()
    {
        T* c = FindComponent<T>();
        if (!c)
        {
            c = new T(this);
            c->object = this;
            components.insert(std::make_pair(type_index<T>(), c));
            return c;
        }
        else
            return c;
    }

    template<typename T>
    T* FindComponent()
    {
        if (components.empty())
            return 0;
        auto it = components.find(type_index<T>());
        if (it == components.end())
            return 0;
        else
            return (T*)it->second;
    }

    SceneObject* FindObject(const std::string& name)
    {
        SceneObject* o = 0;
        for (auto so : objects)
        {
            if (so->Name() == name)
            {
                o = so;
                break;
            }
            else if (o = so->FindObject(name))
            {
                break;
            }
        }

        return o;
    }

    void Name(const std::string& name) { this->name = name; }
    std::string Name() const { return name; }

private:
    std::string name;
    
    SceneObject* parent;
    std::set<SceneObject*> objects;
    std::map<typeindex, Component*> components;

    static ICore* core;
};

#endif
