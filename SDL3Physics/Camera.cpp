#include "Camera.hpp"


void Camera::ProcessKeyboard(cameraMovement direction)
{
	dirty = true;
}

void Camera::ProcessMouse(double xOffset, double yOffset)
{

}

void Camera::UpdateVectors()
{
	dirty = true;
}

mfg::mat4 Camera::GetMatrix()
{
	if (dirty)
	{
		viewMat = mfg::View(right, up, front, entity->position);
		dirty = false;
	}

	return viewMat;
}