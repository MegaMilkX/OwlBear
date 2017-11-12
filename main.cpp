
#include <iostream>
#include <Windows.h>

#include "core_interface.h"

typedef GameState* (*PFN_START) (CoreInterface*);
PFN_START FuncStart = 0;

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
    if (FuncStart)
        state = FuncStart(coreInterface);
    else
    {
        std::cout << "Failed to find OwlBearStart()" << std::endl;
        std::getchar();
    }

    if (!state)
    {
        return 1;
    }

    coreInterface->Switch(state);
    while(coreInterface->Update());

    return 0;
}