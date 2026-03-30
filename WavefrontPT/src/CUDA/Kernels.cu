#include "Kernels.cuh"
#include "helper_math.h"
#include <stdio.h>

__device__ static bool sphereIntersect(float3 rayOgn, float3 rayDir, float3 sphereCenter, float sphereRadius) {
    float3 oc = sphereCenter - rayOgn;
    float a = dot(rayDir, rayDir);
    float b = -2.0f * dot(rayDir, oc);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4 * a * c;
    return (discriminant >= 0);
}

__global__ void cuda_InitTraceRay(unsigned char* surface, int width, int height, size_t pitch, CameraData camData, float t)
{
    int    x = blockIdx.x * blockDim.x + threadIdx.x;
    int    y = blockIdx.y * blockDim.y + threadIdx.y;
    float* pixel;

    // skip extra pixels because of pitch
    if (x >= width || y >= height)
        return;

    // get a pointer to the pixel at (x,y)
    pixel = (float*)(surface + y * pitch) + 4 * x;


    //Create a ray pointing from camera position out
    float3 ogn = camData.position;

    // Calculate forward vector
    //float3 forward = camData.forward;

    // Coordinates on film plane
    float u = (x + 0.5f) * camData.pixelWidth - camData.halfFilmPlaneWidth;
    float v = (y + 0.5f) * camData.pixelHeight - camData.halfFilmPlaneHeight;

    //float3 pointOnFilm = make_float3(u, v, 2); // CHANGE IF THE CAMERA MOVES AT ALL
    float3 pointOnFilm = ogn + (camData.forward * 8) + (camData.right * u) + (camData.up * v);
    float3 rayDir = normalize(pointOnFilm - ogn);

    //// Simple Sphere at (0, 0, 5) with radius 1.0
    bool sphere1 = sphereIntersect(ogn, rayDir, make_float3(0, 0, -5), 1.0f);
    bool sphere2 = sphereIntersect(ogn, rayDir, make_float3(-2, 3, -20), 1.0f);

    if (sphere1)
    {
        // Hit the sphere: Red
        pixel[0] = 0.56f;
        pixel[1] = 0.0f;
        pixel[2] = 0.26f;
        pixel[3] = 1.0f;
    }
    else if (sphere2) {
        pixel[0] = 0.26f;
        pixel[1] = 0.0f;
        pixel[2] = 0.56f;
        pixel[3] = 1.0f;
    }
    else
    {
        pixel[0] = 0.14f;
        pixel[1] = 0.14f;
        pixel[2] = 0.14f;
        pixel[3] = 1.0f;
    }
}

__global__ void cuda_kernel_texture_2d(unsigned char* surface, int width, int height, size_t pitch)
{
    int    x = blockIdx.x * blockDim.x + threadIdx.x;
    int    y = blockIdx.y * blockDim.y + threadIdx.y;
    float* pixel;

    // in the case where, due to quantization into grids, we have
    // more threads than pixels, skip the threads which don't
    // correspond to valid pixels
    if (x >= width || y >= height)
        return;

    // get a pointer to the pixel at (x,y)
    pixel = (float*)(surface + y * pitch) + 4 * x;

    // populate it
    pixel[0] = float(x + y) / (width + width);
    pixel[1] = 0.0;
    pixel[2] = (float)y / width;
    pixel[3] = 1;
}

__global__ void cuda_GenerateCameraRays(Paths paths, CameraData camData, uint32_t maxPaths, uint32_t width, uint32_t height)
{
    size_t idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx > maxPaths)
        return;

    uint32_t x = idx % width;
    uint32_t y = idx / width;

    //Create a ray pointing from camera position out
    float3 ogn = camData.position;
    paths.rayOgn[idx] = ogn;

    // Coordinates on film plane
    float u = (x + 0.5f) * camData.pixelWidth - camData.halfFilmPlaneWidth;
    float v = (y + 0.5f) * camData.pixelHeight - camData.halfFilmPlaneHeight;

    //float3 pointOnFilm = make_float3(u, v, 2); // CHANGE IF THE CAMERA MOVES AT ALL
    float3 pointOnFilm = ogn + (camData.forward * 8) + (camData.right * u) + (camData.up * v);
    float3 rayDir = normalize(pointOnFilm - ogn);
    paths.rayDir[idx] = rayDir;
}
