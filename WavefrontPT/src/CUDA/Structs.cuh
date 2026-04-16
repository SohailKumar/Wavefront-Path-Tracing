#pragma once
#include <exception>
#include <curand_kernel.h>

struct Ray {
	float3 ogn;
	float3 dir;
};

#define PINK float3(1.0f, 0.0f, 1.0f)
#define TYPES_BEFORE_BLINNPHONG 3

enum MaterialTypeID {
	NO_HIT,
	EXIT_SCENE,
	LIGHT,
	BLINNPHONG
};

struct Paths {
	//Ray* rays; // not sure tbh
	float3* rayOgn;
	float3* rayDir;
	
	float4* color;
	float3* throughput;
	uint32_t* rayCount; // number of rays in current path sample  (bounces)
	//uint32_t* pathCount; // number of samples	

	bool* sampled; 

	uint32_t* rayHitMat;
	uint32_t* rayHitMatID;

	float3* lightRayDir;

	float3* ExtBRDFColor;
	float* ExtBRDFColorPDF;
	float* ExtCosTheta;

	float3* LightBRDFColor;
	float* LightBRDFColorPDF;
	float* LightSelectPDF;
	float* LightCosTheta;
	float3* LightEmittance;

	curandState* randomNo;

	//intersection data
	float3* rayHitNormal;
	float3* rayHitPoint;

	Paths()
	{
		rayOgn = {};
		rayDir = {};
		color = {};
		throughput = {};
		rayCount = {};

		//pathCount = {};

		sampled = {};
		rayHitMat = {};
		rayHitMatID = {};
		//intersection normal
		//intersection input dir
		
		lightRayDir = {};
		ExtBRDFColor = {};
		ExtBRDFColorPDF = {};
		LightBRDFColor = {};
		LightBRDFColorPDF = {};
		LightSelectPDF = {};
		LightCosTheta = {};
		ExtCosTheta = {};
		LightEmittance = {};
		randomNo = {};

		rayHitNormal = {};
		rayHitPoint = {};
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

		err = cudaMallocManaged(&throughput, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayCount, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }



		err = cudaMallocManaged(&sampled, totalPixels * sizeof(bool));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayHitMat, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayHitMatID, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		//cudaMallocManaged(reinterpret_cast<void**>(&pathCount), width * height * sizeof(uint32_t));
		err = cudaMallocManaged(&lightRayDir, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&ExtBRDFColor, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&ExtBRDFColorPDF, totalPixels * sizeof(float));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&LightBRDFColor, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&LightBRDFColorPDF, totalPixels * sizeof(float));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&LightSelectPDF, totalPixels * sizeof(float));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&LightCosTheta, totalPixels * sizeof(float));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&LightEmittance, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }


		err = cudaMallocManaged(&ExtCosTheta, totalPixels * sizeof(float));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&randomNo, totalPixels * sizeof(curandState));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayHitNormal, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&rayHitPoint, totalPixels * sizeof(float3));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }
			
	}
};

#define AVAILABLE_MAT_TYPES 1

struct Queues 
{
	uint32_t* extensionRayQueue = {};
	uint32_t* extensionRayQueueCount = 0;
	uint32_t* shadowRayQueue = {};
	uint32_t* shadowRayQueueCount = 0;

	uint32_t* materialQueue[AVAILABLE_MAT_TYPES] = {};
	uint32_t* materialQueueCount = {};

	uint32_t* MATBlinnPhongQueue = {};

	Queues()
	{
		extensionRayQueue = {};
		shadowRayQueue = {};

		materialQueueCount = {};
		MATBlinnPhongQueue = {};
	};

	Queues(uint32_t width, uint32_t height)
	{
		size_t totalPixels = width * height;

		cudaError_t err = cudaSuccess;

		err = cudaMallocManaged(&extensionRayQueue, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&extensionRayQueueCount, sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&shadowRayQueue, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		err = cudaMallocManaged(&shadowRayQueueCount, sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }


		// material queues

		err = cudaMallocManaged(&MATBlinnPhongQueue, totalPixels * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

		materialQueue[0] = MATBlinnPhongQueue;

		err = cudaMallocManaged(&materialQueueCount, AVAILABLE_MAT_TYPES * sizeof(uint32_t));
		if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }
	};

	~Queues() {
	}
};

struct MaterialData {
	struct BlinnPhongData {
		float3 diffuseColor;
		float3 specularColor;
		float shininess;
	};

	
};