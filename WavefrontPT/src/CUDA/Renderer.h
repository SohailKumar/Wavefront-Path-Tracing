#pragma once
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include "Kernels.cuh"
#include "Scene.h"

class Renderer {
private:
	Paths paths; // all paths
	Queues queues;
	float4* accumulationBuffer;

public: 
	dim3 imageBlockSize;  // threads per block for writing to image buffer
	dim3 imageGridSize; // blocks per grid for writing to image buffer
	int blockSize;
	int gridSize;
	int currWidth;
	int currHeight;

	// Never used default constructor
	Renderer() {
		imageBlockSize = dim3(16, 16);
		imageGridSize = dim3(1024);
		blockSize = 256;
		gridSize = 1024;

		currWidth = 1280;
		currHeight = 720;
		paths = Paths();
		queues = Queues(currWidth, currHeight);
	}

	Renderer(int width, int height) {
		imageBlockSize = dim3(16, 16); // block dimensions are fixed to be 256 threads
		imageGridSize = dim3( ((width + imageBlockSize.x - 1) / imageBlockSize.x), ((height + imageBlockSize.y - 1) / imageBlockSize.y) );
		
		int total = width * height;
		blockSize = 256;
		gridSize = (total + blockSize - 1) / blockSize;
		
		currWidth = width;
		currHeight = height;

		paths = Paths();
		queues = Queues(currWidth, currHeight);
	}

	void Initialize(Scene& scene);

	void IterateOneFrame(Camera& cam, Scene& scene, void* surface, size_t pitch, int frameCount, int bounces);

	void GenerateCameraRays(CameraData camData, int frameCount);
	void ExtensionRayIntersectionKernel(float* sphereRadii, float3* sphereCenters, uint32_t sphereCount, float3* planeTriA, float3* planeTriB, float3* planeTriC, uint32_t planeTriCount, float3* lightTriA, float3* lightTriB, float3* lightTriC, uint32_t lightCount);
	void ShadowRayIntersectionKernel(float* sphereRadii, float3* sphereCenters, uint32_t sphereCount, float3* planeTriA, float3* planeTriB, float3* planeTriC, uint32_t planeTriCount, float3* lightTriA, float3* lightTriB, float3* lightTriC, uint32_t lightCount);
	void LogicKernel(float3* lightColors, float* lightIntensity, float3* lightTriA, float3* lightTriB, float3* lightTriC);
	void RunMaterialShaders(float3* albedoDiffuse, float3* albedoSpecular, float* shininess, uint32_t sphereCount, float3* lightTriA, float3* lightTriB, float3* lightTriC, uint32_t lightCount, float3* lightColors, float* lightIntensity);
	void PostProcess(void* surface, size_t pitch, int frameCount, float4* accumulationBuffer);

	void InitializeRays(void* surface, size_t pitch, CameraData camData, float t);
	void TextureTest(void* surface, uint32_t width, uint32_t height, size_t pitch);

};