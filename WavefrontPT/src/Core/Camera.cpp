#include <helper_math.h>
#include "Camera.h"

Camera::Camera(float3 position,
	float3 look_at,
	float filmPlaneWidth,
	float filmPlaneHeight,
	int windowWidth,
	int windowHeight)
{
	this->camDetails.position = position;
	this->camDetails.look_at = look_at;
	this->camDetails.halfFilmPlaneWidth = filmPlaneWidth / 2;
	this->camDetails.halfFilmPlaneHeight = filmPlaneHeight / 2;

	float3 forward = normalize(look_at - position);

	float3 worldUp = make_float3(0.0f, 1.0f, 0.0f);
	float3 right = normalize(cross(forward, worldUp));
	this->camDetails.up = normalize(cross(forward, right));

	this->camDetails.pixelWidth = filmPlaneWidth / windowWidth;
	this->camDetails.pixelHeight = filmPlaneHeight / windowHeight;
}
