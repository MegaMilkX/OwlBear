#ifndef MODULE_LIBRARY_H
#define MODULE_LIBRARY_H

#include "scene_object.h"

#include <windows.h>
#include <string>
#include <vector>

#include <iostream>

inline std::vector<std::string> find_files(const std::string& filename)
{
    std::vector<std::string> result;

    WIN32_FIND_DATAA find_data;
    HANDLE h;
    long long time = 0;

    h = FindFirstFileA(filename.c_str(), &find_data);
    if (h == NULL)
    {
        return result;
    }

    result.push_back(std::string(find_data.cFileName));

    while (FindNextFileA(h, &find_data))
    {
        result.push_back(std::string(find_data.cFileName));
    }

    return result;
}

typedef void (*PFN_SYSTEMSTART) (void);
typedef Component*(*PFN_SYSTEMGETCOMPONENT) (SceneObject*, const char*);
typedef void(*PFN_SYSTEMCLEANUP) (void);

class ComponentModule
{
public:
    bool Init(const std::string& name)
    {
        module = LoadLibraryA(name.c_str());
        if (module == NULL)
        {
            std::cout << "Failed to load system module " << name << std::endl;
            return false;
        }

        FuncStart = (PFN_SYSTEMSTART)GetProcAddress(module, "SystemStart");
        FuncGetComponent = (PFN_SYSTEMGETCOMPONENT)GetProcAddress(module, "SystemGetComponent");
        FuncCleanup = (PFN_SYSTEMCLEANUP)GetProcAddress(module, "SystemCleanup");
        if (!FuncStart || !FuncGetComponent || !FuncCleanup)
        {
            std::cout << "Failed to find required functions for " << name << std::endl;
            return false;
        }

        FuncStart();

        return true;
    }

    Component* GetComponent(SceneObject* so, const char* name)
    {
        return FuncGetComponent(so, name);
    }
    
    void Cleanup()
    {
        FuncCleanup();
        FreeLibrary(module);
    }
private:
    HMODULE module;
    PFN_SYSTEMSTART FuncStart;
    PFN_SYSTEMGETCOMPONENT FuncGetComponent;
    PFN_SYSTEMCLEANUP FuncCleanup;
};

class ModuleLibrary
{
public:
    static void Init()
    {
        std::vector<std::string> files = find_files("*.dll");
        for (std::string& f : files)
        {
            ComponentModule module;
            if (!module.Init(f))
                continue;
            modules.push_back(module);
        }
    }

    static Component* GetComponent(SceneObject* so, const char* name)
    {
        for (auto& m : modules)
        {
            Component* c = m.GetComponent(so, name);
            if (c)
                return c;
        }
        return 0;
    }

    static void Cleanup()
    {
        for (auto& m : modules)
        {
            m.Cleanup();
        }
    }
private:
    static std::vector<ComponentModule> modules;
};

#endif
