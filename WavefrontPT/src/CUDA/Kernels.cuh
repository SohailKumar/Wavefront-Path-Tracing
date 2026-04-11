#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "Camera.h"
#include "Structs.cuh"

__global__ void cuda_InitTraceRay(unsigned char* surface, int width, int height, size_t pitch, CameraData camData, float t);

__global__ void cuda_kernel_texture_2d(unsigned char* surface, int width, int height, size_t pitch);

__global__ void cuda_GenerateCameraRays(Paths paths, CameraData camData, uint32_t maxPaths, uint32_t width, uint32_t height);

__global__ void cuda_Intersection(Paths paths, uint32_t maxPaths, float* sphereRadii, float3* sphereCenters, uint32_t sphereCount, float3* planeTriA, float3* planeTriB, float3* planeTriC, uint32_t planeTriCount);

__global__ void cuda_LogicKernel(Paths paths, uint32_t maxPaths, Queues queues);

__global__ void cuda_MATBlinnPhong(Paths paths, uint32_t* materialQueueCount, uint32_t* MATBlinnPhongQueue, float3* albedoDiffuse, float3* albedoSpecular, float* shininess, uint32_t sphereCount, float3* lightTriA, float3* lightTriB, float3* lightTriC, uint32_t lightCount);

__global__ void cuda_PostProcessPathsAndWriteToSurface(Paths paths, uint32_t maxPaths, uint32_t width, uint32_t height, unsigned char* surface, size_t pitch);