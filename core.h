#ifndef CORE_INTERFACE_H
#define CORE_INTERFACE_H

#include <Windows.h>

#include <set>
#include <vector>
#include <string>

#include <aurora/window.h>

#include "game_state.h"

#include "icore.h"
#include "irender_system.h"
#include "iaudio_system.h"

#include "util.h"

template<typename T>
class SystemModule
{
public:
    SystemModule()
        : sys(0)
    {}

    T* Init(ICore* core, const std::string& initFunc)
    {
        std::vector<std::string> files = find_files("*.dll");
        for (std::string& f : files)
        {
            T* sys = 0;
            if (sys = Init(core, f, initFunc))
                return sys;
        }

        return 0;
    }

    T* Init(ICore* core, const std::string& name, const std::string& initFunc)
    {
        module = LoadLibraryA(name.c_str());
        if (module == NULL)
        {
            return 0;
        }

        FN_SYSTEMSTART = (T*(*) (ICore*))GetProcAddress(module, initFunc.c_str());

        if (!FN_SYSTEMSTART)
        {
            return 0;
        }

        sys = FN_SYSTEMSTART(core);
        if (!sys)
        {
            return 0;
        }

        return sys;
    }

    void Cleanup()
    {
        if (!sys)
            return;
        FreeLibrary(module);
        sys = 0;
    }

    T* GetInterface()
    {
        return sys;
    }
private:
    HMODULE module;
    T* sys;

    T*(*FN_SYSTEMSTART) (ICore*);
};

class SceneObject;
class Core : public ICore
{
public:
    Core();
    ~Core();

    bool Init();
    void Cleanup();

    virtual void Switch(GameState* state);
    virtual float DeltaTime();
    
    virtual IRenderSystem* LoadRenderSystem(const char* filename);
    virtual IRenderSystem* GetRenderSystem();

    bool Update();
private:
    Au::Window* window;

    SystemModule<IRenderSystem> renderSys;

    std::set<SceneObject*> scenes;

    GameState* state;
    float dt;
};

#endif
