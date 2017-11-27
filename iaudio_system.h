#ifndef IAUDIO_SYSTEM_H
#define IAUDIO_SYSTEM_H

class IAudioSystem
{
public:
    virtual bool Init() = 0;
    virtual void Cleanup() = 0;
    
    virtual void Update() = 0;
};

#endif
