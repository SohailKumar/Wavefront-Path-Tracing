#include <exception>

struct Ray {
	float3 ogn;
	float3 dir;
};


struct Paths {
	//Ray* rays; // not sure tbh
	float3* rayOgn;
	float3* rayDir;
	
	float4* color;
	//float3* throughput;
	uint32_t* rayCount; // number of rays in current path sample  (bounces)
	//uint32_t* pathCount; // number of samples

	Paths()
	{
		rayOgn = {};
		rayDir = {};
		color = {};
		//throughput = {};
		rayCount = {};
		//pathCount = {};
	
	};
	~Paths() {
	}

	void reallocatePaths(int width, int height) {
		size_t totalPixels = static_cast<size_t>(width) * static_cast<size_t>(height);

		cudaError_t err = cudaSuccess;

		err = cudaMallocManaged(&rayOgn, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayDir, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&color, totalPixels * sizeof(float4));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayCount, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		//cudaMallocManaged(reinterpret_cast<void**>(&throughput), width * height * sizeof(float3));
		//cudaMallocManaged(reinterpret_cast<void**>(&pathCount), width * height * sizeof(uint32_t));
	}
};
