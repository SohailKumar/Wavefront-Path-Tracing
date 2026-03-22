#pragma once
#include <vector_types.h>

struct CameraData {
	float3 position;
	float3 look_at;
	float3 up;
	float halfFilmPlaneWidth;
	float halfFilmPlaneHeight;
	float pixelWidth;
	float pixelHeight;
};

class Camera {
public:
	CameraData camDetails;

	// Constructor
	Camera(float3 position,
		float3 look_at,
		float filmPlaneWidth,
		float filmPlaneHeight,
		int windowWidth,
		int windowHeight);
};

