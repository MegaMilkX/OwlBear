#ifndef MODULE_LIBRARY_H
#define MODULE_LIBRARY_H

#include "icore.h"
#include "game_system.h"
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

typedef GameSystem*(*PFN_SYSTEMSTART) (ICore*);
typedef Component*(*PFN_SYSTEMGETCOMPONENT) (SceneObject*, const char*);
typedef void(*PFN_SYSTEMCLEANUP) (void);

class ComponentModule
{
public:
    ComponentModule()
        : FuncStart(0), FuncCleanup(0)
    {

    }
    GameSystem* Init(const std::string& name, ICore* core)
    {
        GameSystem* sys = 0;
        module = LoadLibraryA(name.c_str());
        if (module == NULL)
        {
            std::cout << "Failed to load system module " << name << std::endl;
            return 0;
        }

        FuncStart = (PFN_SYSTEMSTART)GetProcAddress(module, "SystemStart");
        FuncCleanup = (PFN_SYSTEMCLEANUP)GetProcAddress(module, "SystemCleanup");
        if (!FuncStart || !FuncCleanup)
        {
            std::cout << "Failed to find required functions for " << name << std::endl;
            return 0;
        }

        sys = FuncStart(core);
        if (sys)
            if (!sys->Init())
            {
                sys->Cleanup();
                Cleanup();
                return 0;
            }
        return sys;
    }
    
    void Cleanup()
    {
        FuncCleanup();
        FreeLibrary(module);
    }
private:
    HMODULE module;
    PFN_SYSTEMSTART FuncStart;
    PFN_SYSTEMCLEANUP FuncCleanup;
};

class ModuleLibrary
{
public:
    static void Init(ICore* core)
    {
        std::vector<std::string> files = find_files("*.dll");
        for (std::string& f : files)
        {
            ComponentModule module;
            GameSystem* sys = module.Init(f, core);
            if (!sys)
                continue;
            modules.push_back(module);
            systems.push_back(sys);
        }
    }

    static Component* CreateComponent(SceneObject* so, const char* name)
    {
        for (auto& s : systems)
        {
            Component* c = s->CreateComponent(so, name);
            if (c)
                return c;
        }

        return 0;
    }

    static bool Update(float dt)
    {
        for (auto& s : systems)
        {
            if (!s->Update())
                return false;
        }
        return true;
    }

    static void Cleanup()
    {
        for (auto& s : systems)
        {
            s->Cleanup();
        }

        for (auto& m : modules)
        {
            m.Cleanup();
        }
    }
private:
    static std::vector<ComponentModule> modules;
    static std::vector<GameSystem*> systems;
};

#endif
