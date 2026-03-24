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
	this->camDetails.halfFilmPlaneWidth = filmPlaneWidth / 2;
	this->camDetails.halfFilmPlaneHeight = filmPlaneHeight / 2;
	this->camDetails.pixelWidth = filmPlaneWidth / windowWidth;
	this->camDetails.pixelHeight = filmPlaneHeight / windowHeight;


	this->camDetails.look_at = look_at;
	this->camDetails.forward = normalize(look_at - position);

	float3 worldUp = make_float3(0.0f, 1.0f, 0.0f);
	this->camDetails.right = normalize(cross(this->camDetails.forward, worldUp));
	this->camDetails.up = normalize(cross(this->camDetails.right, this->camDetails.forward));
}

void Camera::UpdateCameraPosition(float3 newPos) {
	float3 diff = newPos - this->camDetails.position;
	this->camDetails.position = newPos;
	this->camDetails.look_at += diff;
	this->camDetails.forward += diff;
	this->camDetails.right += diff;
	this->camDetails.up += diff;
}

void Camera::UpdateCameraRotation(float3 newLook_At) {
	this->camDetails.look_at = newLook_At;
	this->camDetails.forward = normalize(this->camDetails.look_at - this->camDetails.position);

	float3 worldUp = make_float3(0.0f, 1.0f, 0.0f);
	this->camDetails.right = normalize(cross(this->camDetails.forward, worldUp));
	this->camDetails.up = normalize(cross(this->camDetails.right, this->camDetails.forward));
}
