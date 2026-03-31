#include "Scene.h"
#include <cuda_runtime_api.h>
#include <exception>
#include <vector_functions.h>

void Scene::CreateScene()
{
	// TODO: LOAD MODELS and get triangles and materials
	// - Put them into BVH for 

	cudaError_t err = cudaSuccess;

	sphereCount = 2;

	err = cudaMallocManaged(reinterpret_cast<void**>(&sphereRadii), sphereCount * sizeof(float));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&sphereCenters), sphereCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	sphereCenters[0] = make_float3(0, 0, -5);
	sphereRadii[0] = 1.0f;

	sphereCenters[1] = make_float3(-2, 3, -20);
	sphereRadii[1] = 1.0f;

	float3 testCenter1 = sphereCenters[0];
	float testRadius1 = sphereRadii[0];
	float3 testCenter2 = sphereCenters[1];
	float testRadius2 = sphereRadii[1];
	//bool sphere1 = sphereIntersect(ogn, rayDir, make_float3(0, 0, -5), 1.0f);
	//bool sphere2 = sphereIntersect(ogn, rayDir, make_float3(-2, 3, -20), 1.0f);

}
