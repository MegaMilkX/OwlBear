#ifndef GAME_STATE_H
#define GAME_STATE_H

class GameState
{
public:
    virtual ~GameState() {}
    virtual void Init() = 0;
    virtual void Switch() = 0;
    virtual void Cleanup() = 0;
    virtual bool Update() = 0;
};

#endif

