#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_
#include <SDL3/SDL_keycode.h>
#include "vec.hpp"
#include "matrix.hpp"
#include "Entity.hpp"

enum cameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};


class Camera
{
public:
	mfg::mat4 viewMat;
	mfg::mat4 projMat;
	mfg::vec3 front = { 0, 0, 1 };
	mfg::vec3 right = { 1, 0, 0 };
	mfg::vec3 up = { 0, 1, 0 };
	mfg::vec3 worldUp;
	Entity* m_entity = nullptr;
	float movementSpeed = 10.f;
	bool dirty = true;

	Camera() {};

	Camera(Entity* entity) : worldUp(0, 1, 0)
	{
		m_entity = entity;
	}

	void ProcessKeyboard(cameraMovement direction, double timeDelta);
	void ProcessMouse(double xOffset, double yOffset);

	void UpdateVectors();
	mfg::mat4 GetMatrix();
};

#endif