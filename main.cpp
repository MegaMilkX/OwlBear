
#include <iostream>
#include <Windows.h>

#include "core_interface.h"

#include <aurora/lua.h>

typedef GameState* (*PFN_START) (Core*);
typedef void(*PFN_CLEANUP) ();
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

    FuncStart = (PFN_START)GetProcAddress(game_module, "OwlBearStart");
    FuncCleanup = (PFN_CLEANUP)GetProcAddress(game_module, "OwlBearCleanup");
    if (!FuncStart || !FuncCleanup)
    {
        std::cout << "Failed to find required functions" << std::endl;
        std::getchar();
        return 1;
    }

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