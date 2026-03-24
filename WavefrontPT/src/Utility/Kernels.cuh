#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "Camera.h"
#include "helper_math.h"


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
    float u  = (x + 0.5f) * camData.pixelWidth - camData.halfFilmPlaneWidth;
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

__global__ void cuda_InitTraceRay4(unsigned char* surface, int width, int height, size_t pitch, CameraData camData, float t)
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
    float3 forward = camData.forward;
    float fLen = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
    forward.x /= fLen; forward.y /= fLen; forward.z /= fLen;

    // Calculate right vector
    float3 right = make_float3(forward.y * camData.up.z - forward.z * camData.up.y,
                               forward.z * camData.up.x - forward.x * camData.up.z,
                               forward.x * camData.up.y - forward.y * camData.up.x);
    float rLen = sqrtf(right.x * right.x + right.y * right.y + right.z * right.z);
    right.x /= rLen; right.y /= rLen; right.z /= rLen;

    // Calculate true up vector
    float3 true_up = make_float3(right.y * forward.z - right.z * forward.y,
                                 right.z * forward.x - right.x * forward.z,
                                 right.x * forward.y - right.y * forward.x);

    // Coordinates on film plane
    float u = (x + 0.5f) * camData.pixelWidth - camData.halfFilmPlaneWidth;
    float v = camData.halfFilmPlaneHeight - (y + 0.5f) * camData.pixelHeight;

    // Ray direction
    float3 dir = make_float3(
        forward.x + u * right.x + v * true_up.x,
        forward.y + u * right.y + v * true_up.y,
        forward.z + u * right.z + v * true_up.z
    );
    dir.y /= (16.0f / 9.0);
    
    float dLen = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    dir.x /= dLen; dir.y /= dLen; dir.z /= dLen;

    // Simple Sphere at (0, 0, 5) with radius 1.0
    float cx = 0.0f, cy = 0.0f, cz = 5.0f;
    float radius = 1.0f;

    float ocx = ogn.x - cx;
    float ocy = ogn.y - cy;
    float ocz = ogn.z - cz;

    float a = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    float b = 2.0f * (ocx * dir.x + ocy * dir.y + ocz * dir.z);
    float c = (ocx * ocx + ocy * ocy + ocz * ocz) - radius * radius;

    float discriminant = b * b - 4.0f * a * c;

    if (discriminant > 0.0f)
    {
        // Hit the sphere: Red
        pixel[0] = 0.56f;
        pixel[1] = 0.0f;
        pixel[2] = 0.26f;
        pixel[3] = 1.0f;
    }
    else
    {
        // Missed: Blue
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
    //pixel[0] = 0.5 * pixel[0] + 0.5 * pow(value_x, 3.0f); // red
    //pixel[1] = 0.5 * pixel[1] + 0.5 * pow(value_y, 3.0f); // green
    //pixel[2] = 0.5f + 0.5f * cos(t);                      // blue
    //pixel[3] = 1;                                         // alpha
    pixel[0] = float(x + y) / (width + width);
    pixel[1] = 0.0;
    pixel[2] = (float)y / width;
    pixel[3] = 1;
    //if (x > 1000 && x <= 1200 && y > 500 && y <= 700) {
    //    pixel[0] = float(x + y) / (width + width);
    //    pixel[1] = 0.0;
    //    pixel[2] = (float)y / width;
    //    pixel[3] = 1;
    //}
    //else {
    //    pixel[0] = 0.0f;
    //    pixel[1] = 1.0;
    //    pixel[2] = 0.0f;
    //    pixel[3] = 1;
    //}
}