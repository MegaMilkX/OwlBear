
#include <iostream>
#include <Windows.h>

#include "core_interface.h"

typedef GameState* (*PFN_START) (CoreInterface*);
typedef void(*PFN_CLEANUP) ();
PFN_START FuncStart = 0;
PFN_CLEANUP FuncCleanup = 0;

CoreInterface* coreInterface = 0;

int main()
{
    coreInterface = new CoreInterface();

    HMODULE game_module = LoadLibrary("game.dll");
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

    state = FuncStart(coreInterface);

    if (!state)
    {
        return 1;
    }

    coreInterface->Switch(state);
    while(coreInterface->Update());

    FuncCleanup();
    return 0;
}