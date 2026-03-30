#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "Camera.h"
#include "Structs.cuh"

__global__ void cuda_InitTraceRay(unsigned char* surface, int width, int height, size_t pitch, CameraData camData, float t);

__global__ void cuda_kernel_texture_2d(unsigned char* surface, int width, int height, size_t pitch);

__global__ void cuda_GenerateCameraRays(Paths paths, CameraData camData, uint32_t maxPaths, uint32_t width, uint32_t height);