#ifndef IRENDER_SYSTEM_H
#define IRENDER_SYSTEM_H

#include <Windows.h>

class IRenderSystem
{
public:
    virtual bool Init(HWND window) = 0;
    virtual void Cleanup() = 0;

    virtual void ResizeCanvas(int width, int height) = 0;
    
    virtual void Update() = 0;
};

#endif
