#include "Scene.h"
#include <cuda_runtime_api.h>
#include <exception>
#include <vector_functions.h>
#include <owl.h>

void Scene::CreateScene()
{
	// TODO: LOAD MODELS and get triangles and materials
	// - Put them into BVH for 

	cudaError_t err = cudaSuccess;

	sphereCount = 2;
	planeTriCount = 8;

	err = cudaMallocManaged(reinterpret_cast<void**>(&sphereRadii), sphereCount * sizeof(float));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&sphereCenters), sphereCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&planeTriA), planeTriCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&planeTriB), planeTriCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&planeTriC), planeTriCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	objectCount = sphereCount + planeTriCount;

	err = cudaMallocManaged(reinterpret_cast<void**>(&albedoDiffuse), objectCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&albedoSpecular), objectCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&shininess), objectCount * sizeof(float));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }


	// INITIALIZATION:

	sphereCenters[0] = make_float3(1, 0.7, -6);
	sphereRadii[0] = 1.2f;
	albedoDiffuse[0] = make_float3(0.56f, 0.0f, 0.3f);
	albedoSpecular[0] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[0] = 2.0f;

	sphereCenters[1] = make_float3(-2, -0.2, -2);
	sphereRadii[1] = 0.6f;
	albedoDiffuse[1] = make_float3(0.56f, 0.0f, 0.3f);
	albedoSpecular[1] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[1] = 2.0f;

	//counterclockwise winding order for front face
	//floor
	planeTriA[0] = make_float3(-4, -1, 5);
	planeTriB[0] = make_float3(4, -1, 5);
	planeTriC[0] = make_float3(-4, -1, -10);
	albedoDiffuse[2] = make_float3(0.15f, 0.38f, 1.0f);
	albedoSpecular[2] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[2] = 1.0f;

	planeTriA[1] = make_float3(4, -1, 5);
	planeTriB[1] = make_float3(4, -1, -10);
	planeTriC[1] = make_float3(-4, -1, -10);
	albedoDiffuse[3] = make_float3(0.15f, 0.38f, 1.0f);
	albedoSpecular[3] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[3] = 1.0f;

	float3 wallColor = make_float3(0.76f, 0.76f, 0.76f);

	// back wall
	planeTriA[2] = make_float3(-4, -1, -10);
	planeTriB[2] = make_float3(4, -1, -10);
	planeTriC[2] = make_float3(-4, 3.2, -10);
	albedoDiffuse[4] = wallColor;
	albedoSpecular[4] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[4] = 1.0f;

	planeTriA[3] = make_float3(4, -1, -10);
	planeTriB[3] = make_float3(4, 3.2, -10);
	planeTriC[3] = make_float3(-4, 3.2, -10);
	albedoDiffuse[5] = wallColor;
	albedoSpecular[5] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[5] = 1.0f;


	// left wall
	planeTriA[4] = make_float3(-4, 3.2, 5);
	planeTriB[4] = make_float3(-4, -1, 5);
	planeTriC[4] = make_float3(-4, -1, -10);
	albedoDiffuse[6] = wallColor;
	albedoSpecular[6] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[6] = 1.0f;

	planeTriA[5] = make_float3(-4, 3.2, 5);
	planeTriB[5] = make_float3(-4, -1, -10);
	planeTriC[5] = make_float3(-4, 3.2, -10);
	albedoDiffuse[7] = wallColor;
	albedoSpecular[7] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[7] = 1.0f;

	// right wall
	planeTriA[6] = make_float3(4, 3.2, -10);
	planeTriB[6] = make_float3(4, -1, -10);
	planeTriC[6] = make_float3(4, -1, 5);
	albedoDiffuse[8] = wallColor;
	albedoSpecular[8] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[8] = 1.0f;

	planeTriA[7] = make_float3(4, 3.2, -10);
	planeTriB[7] = make_float3(4, -1, 5);
	planeTriC[7] = make_float3(4, 3.2, 5);
	albedoDiffuse[9] = wallColor;
	albedoSpecular[9] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[9] = 1.0f;

	// roof
	planeTriA[8] = make_float3(-4, 3.2, 5);
	planeTriB[8] = make_float3(-4, 3.2, -10);
	planeTriC[8] = make_float3(4, 3.2, 5);
	albedoDiffuse[10] = wallColor;
	albedoSpecular[10] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[10] = 1.0f;

	planeTriA[9] = make_float3(4, 3.2, 5);
	planeTriB[9] = make_float3(-4, 3.2, -10);
	planeTriC[9] = make_float3(4, 3.2, -10);
	albedoDiffuse[11] = wallColor;
	albedoSpecular[11] = make_float3(1.0f, 1.0f, 1.0f);
	shininess[11] = 1.0f;

	// LIGHTS
	lightCount = 2;

	err = cudaMallocManaged(reinterpret_cast<void**>(&lightTriA), lightCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }
	
	err = cudaMallocManaged(reinterpret_cast<void**>(&lightTriB), lightCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&lightTriC), lightCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	err = cudaMallocManaged(reinterpret_cast<void**>(&lightColors), lightCount * sizeof(float3));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }
	
	err = cudaMallocManaged(reinterpret_cast<void**>(&lightIntensity), lightCount * sizeof(float));
	if (err != cudaSuccess) { throw std::exception(cudaGetErrorString(err)); }

	float3 lightColor = make_float3(1.0, 1.0f, 1.0f);

	lightTriA[0] = make_float3(-3, 3.2, -1);
	lightTriB[0] = make_float3(-3, 3.2, -5);
	lightTriC[0] = make_float3(3, 3.2, -1);
	lightColors[0] = lightColor;
	//lightColors[0] = make_float3(1.0f, 0.925f, 0.3f);
	lightIntensity[0] = 24.0f;

	lightTriA[1] = make_float3(3, 3.2, -1);
	lightTriB[1] = make_float3(-3, 3.2, -5);
	lightTriC[1] = make_float3(3, 3.2, -5);
	lightColors[1] = lightColor;
	//lightColors[1] = make_float3(1.0f, 0.925f, 0.3f);
	lightIntensity[1] = 24.0f;

	//lightTriA[0] = make_float3(-2, 3, 0);
	//lightTriB[0] = make_float3(-2, 3, -15);
	//lightTriC[0] = make_float3(2, 3, 0);
	//lightColors[0] = make_float3(1.0f, 1.0f, 1.0f);
	//lightIntensity[0] = 10.0f;

	//lightTriA[1] = make_float3(2, 3, 0);
	//lightTriB[1] = make_float3(-2, 3, -15);
	//lightTriC[1] = make_float3(2, 3, -15);
	//lightColors[1] = make_float3(1.0f, 1.0f, 1.0f);
	//lightIntensity[1] = 10.0f;
}

void Scene::MoveSphere(float t)
{
	sphereCenters[0].x = 2.0f * cosf(t);
}
