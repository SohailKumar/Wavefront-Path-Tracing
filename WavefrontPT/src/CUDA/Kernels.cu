#include "Kernels.cuh"
#include "GPUUtil.cuh"
#include <stdio.h>
#include <curand_kernel.h>
#include <iostream>

#include <cooperative_groups.h>
namespace cg = cooperative_groups;

#define RANDOM_SEED 1

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
    float3 hitPoint = {};
    float3 normal = {};

    bool sphere1 = sphereIntersect(ogn, rayDir, make_float3(0, 0, -5), 1.0f, hitPoint, normal);
    bool sphere2 = sphereIntersect(ogn, rayDir, make_float3(-2, 3, -20), 1.0f, hitPoint, normal);

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
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
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


    // Initialize other path variables
    // MOVE TO ITS OWN KERNEL IF REGENERATING RAYS
    paths.rayCount[idx] = -1; // first time this ray hits an object, it gets updated to 0 bounces
    paths.sampled[idx] = false;
    paths.rayHitMat[idx] = NO_HIT;

    //paths.randomSeed = 
    //curand_init(1, id, )
    curand_init(RANDOM_SEED, idx, 0, &paths.randomNo[idx]);
}

__global__ void cuda_IntersectionSpheres(Paths paths, uint32_t maxPaths, float* sphereRadii, float3* sphereCenters, uint32_t sphereCount) {
    size_t idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx > maxPaths)
        return;
    
    bool intersect = 0;

    //Check for intersections
    for (uint32_t i = 0; i < sphereCount; ++i) {
        float3 hitPoint = {};
        float3 normal = make_float3(1.0, 1.0, 0.0);
        if (sphereIntersect(paths.rayOgn[idx], paths.rayDir[idx], sphereCenters[i], sphereRadii[i], hitPoint, normal)) {
            intersect = 1;
            paths.rayHitMatID[idx] = i;
            paths.rayHitMat[idx] = BLINNPHONG;
            paths.rayHitInDir[idx] = paths.rayDir[idx];
            paths.rayHitNormal[idx] = normal;
            break;
        }
    }


    if(!intersect)
    {
        paths.rayHitMat[idx] = NO_HIT;
    }
}


__global__ void cuda_LogicKernel(Paths paths, uint32_t maxPaths, Queues queues) {
    size_t idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx > maxPaths)
        return;

    // update throughput
    paths.rayCount += 1;

    // Ray Termination: x bounces, no hit, hit light
    if (paths.rayCount[idx] <= 1) // allow 1 bounce
    {
        // kill
        paths.color[idx] = make_float4(0.14f, 0.14f, 0.14f, 1.0f);
        paths.sampled[idx] = true;
        return;
    }
    
    // Ray lives!

    int matId = paths.rayHitMat[idx];
    if (matId < BLINNPHONG) { // matId = NO_HIT, EXIT_SCENE
        return;
    }

    int matQueueIdx = matId - 2;
    //queues.materialQueue[matQueueIdx];

    // find all threads in the same warp with the same matID
    auto g = cg::coalesced_threads();
    auto mat_group = cg::labeled_partition(g, matId);

    int num_matches = mat_group.size();
    int my_rank = mat_group.thread_rank();
    
    // leader atomically adds to the material count
    int global_offset;
    if (my_rank == 0) {
        global_offset = atomicAdd(&queues.materialQueueCount[matQueueIdx], num_matches);
    }

    // broadcast offset to the rest of the threads in the warp
    global_offset = mat_group.shfl(global_offset, 0);

    // write ray index to this material's queue
    queues.materialQueue[matQueueIdx][global_offset + my_rank] = idx;
}

__global__ void cuda_MATBlinnPhong(Paths paths, uint32_t* materialQueueCount, uint32_t* MATBlinnPhongQueue, float3* albedoDiffuse, float3* albedoSpecular, float* shininess, uint32_t sphereCount) {
    size_t idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx > materialQueueCount[BLINNPHONG-2])
        return;

    size_t currPathIdx = MATBlinnPhongQueue[idx];
    size_t matIdx = paths.rayHitMatID[currPathIdx];

    curandState localRandState = paths.randomNo[currPathIdx];

    //paths.color[MATBlinnPhongQueue[idx]] = make_float4(0.56f, 0.0f, 0.26f, 1.0f);
    float3 normal = paths.rayHitNormal[currPathIdx];
    float3 outDir = make_float3(0.0, 0.0f, 0.0f);
    float3 inDir = paths.rayDir[currPathIdx];

    paths.color[MATBlinnPhongQueue[idx]] = make_float4(evaluateBRDF<BLINNPHONG>(normal, outDir, inDir, albedoDiffuse[matIdx], albedoSpecular[matIdx], shininess[matIdx]), 1.0f);

    paths.randomNo[currPathIdx] = localRandState;
}

__global__ void cuda_PostProcessPathsAndWriteToSurface(Paths paths, uint32_t maxPaths, uint32_t width, uint32_t height, unsigned char* surface, size_t pitch) 
{
    //size_t idx = threadIdx.x + blockIdx.x * blockDim.x;
    int    x = blockIdx.x * blockDim.x + threadIdx.x;
    int    y = blockIdx.y * blockDim.y + threadIdx.y;
    float* pixel;

    if (x >= width || y >= height)
        return;

    int idx = (y * width) + x;

    pixel = (float*)(surface + y * pitch) + 4 * x;

    pixel[0] = paths.color[idx].x;
    pixel[1] = paths.color[idx].y;
    pixel[2] = paths.color[idx].z;
    pixel[3] = paths.color[idx].w;
}

