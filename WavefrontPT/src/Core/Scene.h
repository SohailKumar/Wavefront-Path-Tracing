#pragma once
#include "Camera.h"

class Scene {
public:
	Camera camera;

	Scene(Camera cam): camera(cam)
	{

	}

	void CreateScene();

};