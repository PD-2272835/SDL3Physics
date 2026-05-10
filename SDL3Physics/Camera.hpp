#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_
#include "vec.hpp"
#include "matrix.hpp"
#include "Entity.hpp"

enum cameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};


class Camera
{
public:
	float movementSpeed;
	bool dirty = true;

	Entity* entity;
	mfg::vec3 front;
	mfg::vec3 right;
	mfg::vec3 up;
	mfg::vec3 worldUp;
	mfg::mat4 viewMat;

	void ProcessKeyboard(cameraMovement direction);
	void ProcessMouse(double xOffset, double yOffset);

	void UpdateVectors();
	mfg::mat4 GetMatrix();
};

#endif