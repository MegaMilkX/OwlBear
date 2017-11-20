#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include "../scene_object.h"

#include <iostream>

class Renderer : public Component
{
public:
    Renderer(SceneObject* so)
    {
        std::cout << "Renderer is ready!" << std::endl;
    }
private:
    
};

class Camera : public Component
{
public:
    Camera(SceneObject* so)
    {
        renderer = so->GetComponent<Renderer>();
        std::cout << "You have created a camera!" << std::endl;
    }
private:
    Renderer* renderer;
};

class Mesh : public Component
{
public:
    Mesh(SceneObject* so)
    {
        renderer = so->GetComponent<Renderer>();
        std::cout << "Mesh is created!" << std::endl;
    }
private:
    Renderer* renderer;
};

template <>
inline const char* typeName<Renderer>(void) { return "Renderer"; }
template <>
inline const char* typeName<Camera>(void) { return "Camera"; }
template <>
inline const char* typeName<Mesh>(void) { return "Mesh"; }

#endif

