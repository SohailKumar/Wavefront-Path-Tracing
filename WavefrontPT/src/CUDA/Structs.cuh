#include <exception>

struct Ray {
	float3 ogn;
	float3 dir;
};


enum MaterialID {
	NO_HIT,
	EXIT_SCENE,
	LAMBERTIAN
};

struct Paths {
	//Ray* rays; // not sure tbh
	float3* rayOgn;
	float3* rayDir;
	
	float4* color;
	//float3* throughput;
	uint32_t* rayCount; // number of rays in current path sample  (bounces)
	//uint32_t* pathCount; // number of samples	
	bool* sampled; 


	uint32_t* rayHitMat;

	Paths()
	{
		rayOgn = {};
		rayDir = {};
		color = {};
		//throughput = {};
		rayCount = {};

		//pathCount = {};

		sampled = {};
		rayHitMat = {};
	};
	~Paths() {
	}

	void reallocatePaths(int width, int height) {
		size_t totalPixels = width * height;

		cudaError_t err = cudaSuccess;

		err = cudaMallocManaged(&rayOgn, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayDir, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&color, totalPixels * sizeof(float4));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayCount, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&sampled, totalPixels * sizeof(bool));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayHitMat, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		//cudaMallocManaged(reinterpret_cast<void**>(&throughput), width * height * sizeof(float3));
		//cudaMallocManaged(reinterpret_cast<void**>(&pathCount), width * height * sizeof(uint32_t));

	}
};

#define AVAILABLE_MAT_TYPES 1

struct Queues 
{
	uint32_t* materialQueue[AVAILABLE_MAT_TYPES];
	uint32_t* materialQueueCount;

	uint32_t* MATLambertianQueue;

	Queues()
	{
		materialQueueCount = {};
		MATLambertianQueue = {};
	};

	Queues(uint32_t width, uint32_t height)
	{
		size_t totalPixels = width * height;

		cudaError_t err = cudaSuccess;

		err = cudaMallocManaged(&MATLambertianQueue, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		materialQueue[0] = MATLambertianQueue;

		err = cudaMallocManaged(&materialQueueCount, AVAILABLE_MAT_TYPES * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	};

	~Queues() {
	}
};

struct MaterialData {
	struct LambertianData {
		float3 albedo;
	};

	
};