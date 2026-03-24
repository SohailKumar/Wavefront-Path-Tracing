#include "App.h"

namespace App{
    //anonymous namespace for variables only accessible in this file
    namespace {
        Camera _cam;
        Scene _scene;
        Renderer _renderer;
    }
}

App::CreateApp(Scene scene, Renderer renderer){
    _cam = scene.camera;
    _scene = scene;
    _renderer = renderer;
}