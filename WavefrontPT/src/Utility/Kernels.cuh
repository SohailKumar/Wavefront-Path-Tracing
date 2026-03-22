#include "cuda_runtime.h"
#include "device_launch_parameters.h"

__global__ void cuda_InitTraceRay(unsigned char* surface, int width, int height, size_t pitch)
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
    float3 ogn = make_float3(0, 0, 0);;

    // Note: Assuming dir calculation is adapted to generate a proper float3 directional vector
    float3 dir = make_float3(
        (float)x / width - 0.5f,
        (float)y / height - 0.5f,
        1.0f // Pointing into the screen
    );

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


__global__ void cuda_kernel_texture_2d(unsigned char* surface, int width, int height, size_t pitch, float t)
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
    float value_x = 0.5f + 0.5f * cos(t + 10.0f * ((2.0f * x) / width - 1.0f));
    float value_y = 0.5f + 0.5f * cos(t + 10.0f * ((2.0f * y) / height - 1.0f));
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