#include "Renderer.h"
#include "Kernels.cuh"
#include <exception>


void Renderer::Initialize(const Scene& scene)
{
	// TODO: Allocate memory for everything that needs to be in the VRAM. Camera, Paths (Triangles until Optix integrated)
	// Use cudaOccupancyMaxPotentialBlockSize to maximize blocksize and gridsize for the functions
}

void Renderer::InitializeRays(void* surface, size_t width, size_t height, size_t pitch, CameraData camData, float t) {
    cudaError_t error = cudaSuccess;

    cuda_InitTraceRay <<<gridSize, blockSize>>> ((unsigned char*)surface, width, height, pitch, camData, t);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
    }
}

void Renderer::TextureTest(void* surface, size_t width, size_t height, size_t pitch) {
    cudaError_t error = cudaSuccess;

    cuda_kernel_texture_2d <<<gridSize, blockSize>>> ((unsigned char*)surface, width, height, pitch);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
    }
}
