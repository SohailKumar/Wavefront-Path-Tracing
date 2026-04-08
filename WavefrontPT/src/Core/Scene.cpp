#include "Scene.h"
#include <cuda_runtime_api.h>
#include <exception>
#include <vector_functions.h>

void Scene::CreateScene()
{
	// TODO: LOAD MODELS and get triangles and materials
	// - Put them into BVH for 

	cudaError_t err = cudaSuccess;

	// SPHERES

	sphereCount = 2;

	err = cudaMallocManaged(reinterpret_cast<void**>(&sphereRadii), sphereCount * sizeof(float));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&sphereCenters), sphereCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&albedoDiffuse), sphereCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&albedoSpecular), sphereCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&shininess), sphereCount * sizeof(float));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	sphereCenters[0] = make_float3(0, 0, -5);
	sphereRadii[0] = 1.0f;
	albedoDiffuse[0] = make_float3(0.56f, 0.0f, 0.26f);
	albedoSpecular[0] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[0] = 1.0f;

	sphereCenters[1] = make_float3(-2, 3, -20);
	sphereRadii[1] = 1.0f;
	albedoDiffuse[1] = make_float3(0.56f, 0.0f, 0.56f);
	albedoSpecular[1] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[1] = 1.0f;


	// LIGHTS
	lightCount = 1;

	err = cudaMallocManaged(reinterpret_cast<void**>(&lightCenters), lightCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&lightColors), lightCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }
	
	err = cudaMallocManaged(reinterpret_cast<void**>(&lightIntensity), lightCount * sizeof(float));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&lightRadius), lightCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }


	lightCenters[0] = make_float3(7, 7, -2);
	lightColors[0] = make_float3(1.0f, 1.0f, 1.0f);
	lightIntensity[0] = 100.0f;
	lightRadius[0] = 10.0f;
}
