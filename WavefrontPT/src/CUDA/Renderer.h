#pragma once
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>
#include "Scene.h"

class Renderer {
public: 
	dim3 blockSize;  // threads per block
	dim3 gridSize; // blocks per grid

	Renderer(int width, int height) {
		blockSize = dim3(16, 16); // block dimensions are fixed to be 256 threads
		gridSize = dim3((width + blockSize.x - 1) / blockSize.x, (height + blockSize.y - 1) / blockSize.y);

	}

	void Initialize(const Scene& scene);
	void InitializeRays(void* surface, size_t width, size_t height, size_t pitch);

};