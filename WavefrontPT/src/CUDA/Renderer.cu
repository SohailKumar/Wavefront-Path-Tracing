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

void Renderer::IntersectionKernel(float* sphereRadii, float3* sphereCenters, uint32_t sphereCount, float3* planeTriA, float3* planeTriB, float3* planeTriC, uint32_t planeTriCount)
{
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_Intersection<<<gridSize, blockSize>>> (paths, maxPaths, sphereRadii, sphereCenters, sphereCount, planeTriA, planeTriB, planeTriC, planeTriCount);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_Intersection() failed to launch error = %d\n", error);
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

void Renderer::RunMaterialShaders(float3* albedoDiffuse, float3* albedoSpecular, float* shininess, uint32_t sphereCount, float3* lightTriA, float3* lightTriB, float3* lightTriC, uint32_t lightCount) {
    cudaError_t error = cudaSuccess;

    cuda_MATBlinnPhong << <gridSize, blockSize >> > (paths, queues.materialQueueCount, queues.MATBlinnPhongQueue, albedoDiffuse, albedoSpecular, shininess, sphereCount, lightTriA, lightTriB, lightTriC, lightCount);

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
