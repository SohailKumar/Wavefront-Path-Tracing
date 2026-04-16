#include "Renderer.h"
#include <exception>
#include <iostream>

void Renderer::Initialize(Scene &scene)
{
	// TODO: Allocate memory for everything that needs to be in the VRAM. Camera, Paths (Triangles until Optix integrated)
	// Use cudaOccupancyMaxPotentialBlockSize to maximize blocksize and gridsize for the functions

    // Allocate Memory for all camera rays and their properties(color, throughput, bounces, etc.)
    paths.reallocatePaths(currWidth, currHeight);
    
    size_t totalPixels = currWidth * currHeight;
    cudaError_t err = cudaMallocManaged(&accumulationBuffer, totalPixels * sizeof(float4));
    if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

    // Allocate memory for all objects for intersection kernel
    scene.CreateScene();

}

void Renderer::IterateOneFrame(Camera& cam, Scene& scene, void* surface, size_t pitch, int frameCount, int bounces)
{
    GenerateCameraRays(cam.camDetails, frameCount);
    ExtensionRayIntersectionKernel(scene.sphereRadii, scene.sphereCenters, scene.sphereCount, scene.planeTriA, scene.planeTriB, scene.planeTriC, scene.planeTriCount, scene.lightTriA, scene.lightTriB, scene.lightTriC, scene.lightCount);
    LogicKernel(scene.lightColors, scene.lightIntensity);

    for (int i = 0; i < bounces; i++) {
        RunMaterialShaders(scene.albedoDiffuse, scene.albedoSpecular, scene.shininess, scene.sphereCount, scene.lightTriA, scene.lightTriB, scene.lightTriC, scene.lightCount, scene.lightColors, scene.lightIntensity);
        ExtensionRayIntersectionKernel(scene.sphereRadii, scene.sphereCenters, scene.sphereCount, scene.planeTriA, scene.planeTriB, scene.planeTriC, scene.planeTriCount, scene.lightTriA, scene.lightTriB, scene.lightTriC, scene.lightCount);
        ShadowRayIntersectionKernel(scene.sphereRadii, scene.sphereCenters, scene.sphereCount, scene.planeTriA, scene.planeTriB, scene.planeTriC, scene.planeTriCount, scene.lightTriA, scene.lightTriB, scene.lightTriC, scene.lightCount);
        LogicKernel(scene.lightColors, scene.lightIntensity);
	}
    PostProcess(surface, pitch, frameCount, accumulationBuffer);

    cudaError_t error = cudaSuccess;
    error = cudaMemsetAsync(queues.extensionRayQueueCount, 0, sizeof(uint32_t));
    if (error != cudaSuccess) {
        throw std::exception("memsetAsync() failed to launch error = %d\n", error);
    }
    error = cudaMemsetAsync(queues.materialQueueCount, 0, AVAILABLE_MAT_TYPES * sizeof(uint32_t));
    if (error != cudaSuccess) {
        throw std::exception("memsetAsync() failed to launch error = %d\n", error);
    }
}

void Renderer::GenerateCameraRays(CameraData camData, int frameCount) {
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_GenerateCameraRays<<<gridSize, blockSize>>>(paths, queues, camData, maxPaths, currWidth, currHeight, frameCount);
    
  //  cudaDeviceSynchronize();
  //  std::cout << "ExtensionRay Count: " << *queues.extensionRayQueueCount << std::endl;
  //  for (int i = 0; i < 10; i++) {
		//std::cout << "ExtensionRayQueue[" << i << "]: " << queues.extensionRayQueue[i] << std::endl;
  //  }

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_GenerateCameraRays() failed to launch error = %d\n", error);
    }
}

void Renderer::ExtensionRayIntersectionKernel(float* sphereRadii, float3* sphereCenters, uint32_t sphereCount, float3* planeTriA, float3* planeTriB, float3* planeTriC, uint32_t planeTriCount, float3* lightTriA, float3* lightTriB, float3* lightTriC, uint32_t lightCount)
{
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_ExtensionRayIntersection<<<gridSize, blockSize>>> (paths, queues, maxPaths, sphereRadii, sphereCenters, sphereCount, planeTriA, planeTriB, planeTriC, planeTriCount, lightTriA, lightTriB, lightTriC, lightCount);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_Intersection() failed to launch error = %d\n", error);
    }

	// reset extension ray queue count for next set
    error = cudaMemsetAsync(queues.extensionRayQueueCount, 0, sizeof(uint32_t));
    if (error != cudaSuccess) {
        throw std::exception("memsetAsync() failed to launch error = %d\n", error);
    }
}

void Renderer::ShadowRayIntersectionKernel(float* sphereRadii, float3* sphereCenters, uint32_t sphereCount, float3* planeTriA, float3* planeTriB, float3* planeTriC, uint32_t planeTriCount, float3* lightTriA, float3* lightTriB, float3* lightTriC, uint32_t lightCount)
{
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_ShadowRayIntersection << <gridSize, blockSize >> > (paths, queues, maxPaths, sphereRadii, sphereCenters, sphereCount, planeTriA, planeTriB, planeTriC, planeTriCount, lightTriA, lightTriB, lightTriC, lightCount);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_Intersection() failed to launch error = %d\n", error);
    }

    // reset extension ray queue count for next set
    error = cudaMemsetAsync(queues.shadowRayQueueCount, 0, sizeof(uint32_t));
    if (error != cudaSuccess) {
        throw std::exception("memsetAsync() failed to launch error = %d\n", error);
    }
}

void Renderer::LogicKernel(float3* lightColors, float* lightIntensity)
{
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_LogicKernel <<<gridSize, blockSize >>> (paths, maxPaths, queues, lightColors, lightIntensity);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
    }
}

void Renderer::RunMaterialShaders(float3* albedoDiffuse, float3* albedoSpecular, float* shininess, uint32_t sphereCount, float3* lightTriA, float3* lightTriB, float3* lightTriC, uint32_t lightCount, float3* lightColors, float* lightIntensity) {
    cudaError_t error = cudaSuccess;

	//TODO change gridSize and blockSize based on material queue count for better occupancy
    cuda_MATBlinnPhong << <gridSize, blockSize >> > (paths, queues, queues.materialQueueCount, queues.MATBlinnPhongQueue, albedoDiffuse, albedoSpecular, shininess, sphereCount, lightTriA, lightTriB, lightTriC, lightCount, lightColors, lightIntensity);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_MATBlinnPhong() failed to launch error = %d\n", error);
    }

    // reset material queue count for next set
    error = cudaMemsetAsync(queues.materialQueueCount, 0, AVAILABLE_MAT_TYPES * sizeof(uint32_t));
    if (error != cudaSuccess) {
        throw std::exception("memsetAsync() failed to launch error = %d\n", error);
    }
}

void Renderer::PostProcess(void*  surface, size_t pitch, int frameCount, float4* accumulationBuffer) {
    uint32_t maxPaths = currWidth * currHeight;
    cudaError_t error = cudaSuccess;

    cuda_PostProcessPathsAndWriteToSurface <<<imageGridSize, imageBlockSize >>> (paths, maxPaths, currWidth, currHeight,(unsigned char*)surface, accumulationBuffer, pitch, frameCount);

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
