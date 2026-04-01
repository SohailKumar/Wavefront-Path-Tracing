#include "Renderer.h"
#include <exception>
#include <iostream>

void Renderer::Initialize(Scene &scene)
{
	// TODO: Allocate memory for everything that needs to be in the VRAM. Camera, Paths (Triangles until Optix integrated)
	// Use cudaOccupancyMaxPotentialBlockSize to maximize blocksize and gridsize for the functions

    // Allocate Memory for all camera rays and their properties(color, throughput, bounces, etc.)
    paths.reallocatePaths(currWidth, currHeight);

    // Allocate memory for all objects for intersection kernel
    scene.CreateScene();

}

void Renderer::GenerateCameraRays(CameraData camData) {
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_GenerateCameraRays<<<gridSize, blockSize>>>(paths, camData, maxPaths, currWidth, currHeight);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_GenerateCameraRays() failed to launch error = %d\n", error);
    }
}

void Renderer::IntersectionKernel(float* sphereRadii, float3* sphereCenters, uint32_t sphereCount)
{
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_IntersectionSpheres<<<gridSize, blockSize>>> (paths, maxPaths, sphereRadii, sphereCenters, sphereCount);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_GenerateCameraRays() failed to launch error = %d\n", error);
    }

}

void Renderer::LogicKernel()
{
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_LogicKernel <<<gridSize, blockSize >>> (paths, maxPaths, queues);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
    }
}

void Renderer::RunMaterialShaders() {
    cudaError_t error = cudaSuccess;

    cuda_MATLambertian << <gridSize, blockSize >> > (paths, queues.materialQueueCount, queues.MATLambertianQueue);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_GenerateCameraRays() failed to launch error = %d\n", error);
    }
}

void Renderer::PostProcess(void*  surface, size_t pitch) {
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_PostProcessPathsAndWriteToSurface <<<imageGridSize, imageBlockSize >>> (paths, maxPaths, currWidth, currHeight,(unsigned char*)surface, pitch);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
    }
}

void Renderer::InitializeRays(void* surface, size_t pitch, CameraData camData, float t) {
    cudaError_t error = cudaSuccess;

    cuda_InitTraceRay << <imageGridSize, imageBlockSize >> > ((unsigned char*)surface, currWidth, currHeight, pitch, camData, t);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
    }
}

void Renderer::TextureTest(void* surface, uint32_t width, uint32_t height, size_t pitch) {
    cudaError_t error = cudaSuccess;

    cuda_kernel_texture_2d <<<imageGridSize, imageBlockSize>>> ((unsigned char*)surface, width, height, pitch);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
    }
}
