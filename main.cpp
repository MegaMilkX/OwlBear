
#include <iostream>
#include <Windows.h>

#include "core.h"

#include <aurora/lua.h>

typedef int (*PFN_INIT) (Core*);
typedef GameState* (*PFN_START) (Core*);
typedef void(*PFN_CLEANUP) ();
PFN_INIT FuncInit = 0;
PFN_START FuncStart = 0;
PFN_CLEANUP FuncCleanup = 0;

Core* core = 0;

int main()
{
    core = new Core();

    std::string moduleName = "";
    {
        Au::Lua lua;
        lua.Init();
        lua.DoFile("config.lua");
        moduleName = lua.GetGlobal<std::string>("Module");
    }

    HMODULE game_module = LoadLibraryA(moduleName.c_str());
    if (game_module == NULL)
    {
        std::cout << "Failed to load dll" << std::endl;
        std::getchar();
        return 1;
    }

    GameState* state = 0;

    FuncInit = (PFN_INIT)GetProcAddress(game_module, "GameInit");
    FuncStart = (PFN_START)GetProcAddress(game_module, "GameStart");
    FuncCleanup = (PFN_CLEANUP)GetProcAddress(game_module, "GameCleanup");
    if (!FuncStart || !FuncCleanup)
    {
        std::cout << "Failed to find required functions for gameplay module" << std::endl;
        std::getchar();
        return 1;
    }

    if (FuncInit)
    {
        int res = 0;
        if (res = FuncInit(core) != 0)
        {
            std::cout << "Game module init returned " << res << std::endl;
            return 1;
        }
    }
    if (!core->Init())
        return EXIT_FAILURE;
    state = FuncStart(core);
    if (!state)
    {
        return 1;
    }
    core->Switch(state);
    while(core->Update());

    FuncCleanup();
    return 0;
}