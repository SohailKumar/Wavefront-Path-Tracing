#pragma once
#include "Camera.h"

class Scene {
public:
	Camera camera;

	// Never used default constructor
	Scene() {
		camera = {};
	}

	Scene(Camera cam): camera(cam)
	{

	}

	void CreateScene();

};