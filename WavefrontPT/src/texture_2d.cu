#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

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
    pixel[0] = float(x + y)/(width + width);
    pixel[1] = 0.0;
    pixel[2] = (float)y/width;
    pixel[3] = 1;
    if (x > 1000 && x <= 1200 && y > 500 && y <= 700) {
        pixel[0] = float(x + y) / (width + width);
        pixel[1] = 0.0;
        pixel[2] = (float)y / width;
        pixel[3] = 1;
    }
    else {
        pixel[0] = 0.0f;
        pixel[1] = 1.0;
        pixel[2] = 0.0f;
        pixel[3] = 1;
    }
}

extern  "C" void cuda_texture_2d(void* surface, size_t width, size_t height, size_t pitch, float t)
{
    cudaError_t error = cudaSuccess;

    dim3 threadsPBlockDim = dim3(16, 16); // block dimensions are fixed to be 256 threads
    dim3 numberOfBlocksDim = dim3((width + threadsPBlockDim.x - 1) / threadsPBlockDim.x, (height + threadsPBlockDim.y - 1) / threadsPBlockDim.y);

    cuda_kernel_texture_2d <<<numberOfBlocksDim, threadsPBlockDim>>> ((unsigned char*)surface, width, height, pitch, t);

    error = cudaGetLastError();
    if (error != cudaSuccess) {
        throw std::exception("cuda_kernel_texture_2d() failed to launch error = %d\n", error);
    }
}