//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
//#include <exception>
////#include "../Core/Camera.h"
//
//// Create a kernel that creates rays from the camera's origin point and checks for intersection with a cube
//
//__global__ void cuda_TraceRay(unsigned char* surface, int width, int height, size_t pitch)
//{
//    int    x = blockIdx.x * blockDim.x + threadIdx.x;
//    int    y = blockIdx.y * blockDim.y + threadIdx.y;
//    float* pixel;
//
//    // skip extra pixels because of pitch
//    if (x >= width || y >= height)
//        return;
//
//    // get a pointer to the pixel at (x,y)
//    pixel = (float*)(surface  + y * pitch) + 4 * x;
//
//
//    //Create a ray pointing from camera position out
//    float3 ogn = make_float3(0, 0, 0);;
//    
//    // Note: Assuming dir calculation is adapted to generate a proper float3 directional vector
//    float3 dir = make_float3(
//        (float)x / width - 0.5f, 
//        (float)y / height - 0.5f, 
//        1.0f // Pointing into the screen
//    );
//
//    // Simple Sphere at (0, 0, 5) with radius 1.0
//    float cx = 0.0f, cy = 0.0f, cz = 5.0f;
//    float radius = 1.0f;
//
//    float ocx = ogn.x - cx;
//    float ocy = ogn.y - cy;
//    float ocz = ogn.z - cz;
//
//    float a = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
//    float b = 2.0f * (ocx * dir.x + ocy * dir.y + ocz * dir.z);
//    float c = (ocx * ocx + ocy * ocy + ocz * ocz) - radius * radius;
//
//    float discriminant = b * b - 4.0f * a * c;
//
//    if (discriminant > 0.0f)
//    {
//        // Hit the sphere: Red
//        pixel[0] = 0.56f;
//        pixel[1] = 0.0f;
//        pixel[2] = 0.26f;
//        pixel[3] = 1.0f;
//    }
//    else
//    {
//        // Missed: Blue
//        pixel[0] = 0.14f;
//        pixel[1] = 0.14f;
//        pixel[2] = 0.14f;
//        pixel[3] = 1.0f;
//    }
//}
//
//void cuda_SendRays(void* surface, size_t width, size_t height, size_t pitch)
//{
//    cudaError_t error = cudaSuccess;
//
//    cuda_TraceRay <<<gridSize, threadsPerBlockDim >>> ((unsigned char*)surface, width, height, pitch);
//
//    error = cudaGetLastError();
//    if (error != cudaSuccess) {
//        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
//    }
//}