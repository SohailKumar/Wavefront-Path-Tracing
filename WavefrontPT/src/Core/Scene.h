#pragma once
#include "Camera.h"
#include <cstdint>

class Scene {
public:
	Camera camera;
	float* sphereRadii = {};
	float3* sphereCenters = {};
	uint32_t sphereCount;
	
	// Never used default constructor
	Scene() {
		camera = {};
		sphereCount = 0;
	}

	Scene(Camera cam): camera(cam)
	{
		sphereCount = 0;
	}

	void CreateScene();

};