# Nami: Wavefront Path Tracer

Developed for RIT Masters of Computer Science capstone project.

🚀 Some Features
Wavefront Architecture: Minimizes SIMT divergence on the GPU by partitioning the ray-tracing pipeline into discrete kernels (RayGen, Intersection, Shading).

CUDA-DX11 Interop: Renders the path-traced accumulation buffer directly to a DX11 texture for high-performance windowed display.

Efficient Memory Management: Uses SOA (Structure of Arrays) layout to optimize global memory access patterns on the GPU.

🛠 Prerequisites
Before building the project, ensure you have the following installed:

- Operating System: Windows 10/11

- IDE: Visual Studio 2022

- SDK: CUDA Toolkit 13.2 

- Drivers: NVIDIA GPU Drivers compatible with CUDA 13.2 or higher.

- Hardware: An NVIDIA GPU with Compute Capability 7.0+ (recommended).

📖 Architecture Overview (subject to change) #TODO include diagram
Unlike a "Megakernel" approach where one kernel handles the entire life of a ray, this project uses a Wavefront approach:

Ray Generation: Generates primary rays and stores them in a buffer.

Intersection: Traverses the scene (BVH) to find hit points.

Sorting: (Optional/Included) Rays are sorted by material type to ensure warp efficiency during shading.

Shading: Computes lighting, evaluates BSDFs, and generates secondary rays.

DirectX Interop: The final radiance buffer is copied to a ID3D11Texture2D using cudaGraphicsD3D11RegisterResource for immediate on-screen display.
