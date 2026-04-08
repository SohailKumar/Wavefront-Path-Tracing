#pragma once
#include "Camera.h"
#include <cstdint>

class Scene {
public:
	Camera camera = {};

	float* sphereRadii = {};
	float3* sphereCenters = {};
	uint32_t* materialTypeID = {};

	float3* albedoDiffuse = {};
	float3* albedoSpecular = {};
	float* shininess = {};

	uint32_t sphereCount = 0;


	float3* lightCenters = {};
	float3* lightColors = {};
	float* lightIntensity = {};
	float* lightRadius = {};

	uint32_t lightCount = 0;
	
	// Never used default constructor
	Scene() {	}

	Scene(Camera cam): camera(cam)
	{
		
	}

	void CreateScene();

};