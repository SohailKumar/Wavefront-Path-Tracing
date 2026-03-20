#pragma once
#include "helper_math.h"

struct Raya
{
	float3 origin;
	float3 direction;

	__host__ __device__  Raya(const float3 o, const float3 d) : origin(o), direction(d) {};
};

struct CameraDetails {
	float3 position;
	float3 look_at;
	float3 up;
	float focalLength;
	float filmPlaneWidth;
	float filmPlaneHeight;
	float pixelWidth;
	float pixelHeight;
};

class Camera {
public:
	CameraDetails camDetails;

	Camera(float3 position,
		float3 look_at,
		float3 focalLength,
		float filmPlaneWidth,
		float filmPlaneHeight,
		int windowWidth,
		int windowHeight)
	{
		this->camDetails.position = position;
		this->camDetails.look_at = look_at;
		this->camDetails.filmPlaneWidth = filmPlaneWidth;
		this->camDetails.filmPlaneHeight = filmPlaneHeight;

		float3 forward = normalize(look_at - position);

		float3 worldUp = make_float3(0.0f, 1.0f, 0.0f);
		float3 right = normalize(cross(forward, worldUp));

		this->camDetails.up = normalize(cross(forward, right));

		this->camDetails.pixelWidth = this->camDetails.filmPlaneWidth / windowWidth;
		this->camDetails.pixelHeight = this->camDetails.filmPlaneHeight / windowHeight;
	}
};

