#pragma once
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include "Kernels.cuh"
#include "Scene.h"

class Renderer {
private:
	Paths paths; // all paths

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
	}

	void Initialize();
	void InitializeRays(void* surface, size_t pitch, CameraData camData, float t);
	void GenerateCameraRays(CameraData camData);
	void TextureTest(void* surface, uint32_t width, uint32_t height, size_t pitch);

};