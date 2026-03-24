#pragma once
#include <vector_types.h>

struct CameraData { 
	float3 position;
	float3 forward;
	float3 up;
	float3 right;
	float3 look_at;
	float halfFilmPlaneWidth;
	float halfFilmPlaneHeight;
	float pixelWidth;
	float pixelHeight;
};

class Camera {
public:
	CameraData camDetails;

	//Never used default constructor
	Camera() {
		camDetails.position = {};
		camDetails.forward = {};
		camDetails.up = {};
		camDetails.right = {};
		camDetails.look_at = {};
		camDetails.halfFilmPlaneWidth = {};
		camDetails.halfFilmPlaneHeight = {};
		camDetails.pixelWidth = {};
		camDetails.pixelHeight = {};
	}

	// Constructor
	Camera(float3 position,
		float3 forward,
		float filmPlaneWidth,
		float filmPlaneHeight,
		int windowWidth,
		int windowHeight);

	void UpdateCameraPosition(float3 newPos);
	void UpdateCameraRotation(float3 rotation);
};

