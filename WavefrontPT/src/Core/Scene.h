#pragma once
#include "Camera.h"
#include <cstdint>

class Scene {
public:
	Camera camera = {};

	float* sphereRadii = {};
	float3* sphereCenters = {};

	float3* planeTriA  = {};
	float3* planeTriB  = {};
	float3* planeTriC  = {};

	uint32_t* materialTypeID = {};

	float3* albedoDiffuse = {};
	float3* albedoSpecular = {};
	float* shininess = {};

	uint32_t sphereCount = 0;
	uint32_t planeTriCount = 0;
	uint32_t objectCount = 0;

	float3* lightTriA = {};
	float3* lightTriB = {};
	float3* lightTriC = {};
	float3* lightColors = {};
	float* lightIntensity = {};

	uint32_t lightCount = 0;
	
	// Never used default constructor
	Scene() {	}

	Scene(Camera cam): camera(cam)
	{
		
	}

	void CreateScene();

};