#include "App.h"

namespace App{
    //anonymous namespace for variables only accessible in this file
    namespace {
        Camera _cam;
        Scene _scene;
        Renderer _renderer;
    }
}

void App::CreateApp(Renderer renderer, Scene scene){
    _cam = scene.camera;
    _scene = scene;
    _renderer = renderer;
}

Scene App::GetScene()
{
    return _scene;
}

Camera App::GetCamera()
{
    return _cam;
}

Renderer App::GetRenderer()
{
    return _renderer;
}
