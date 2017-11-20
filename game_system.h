#ifndef GAME_SYSTEM_H
#define GAME_SYSTEM_H

class GameSystem
{
public:
    virtual ~GameSystem() {}
    virtual void Init() = 0;
    virtual void Cleanup() = 0;
    virtual bool Update() = 0;
};

#endif
