#pragma once
#include "Renderer.h"

namespace App {
	void CreateApp(Renderer renderer, Scene scene);
	Scene& GetScene();
	Camera& GetCamera();
	Renderer& GetRenderer();
}