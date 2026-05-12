#include "Camera.hpp"


void Camera::ProcessKeyboard(cameraMovement direction, double timeDelta)
{
	float velocity = movementSpeed * timeDelta;

	switch (direction)
	{
	case FORWARD:
		m_entity->position -= front * velocity;
		break;
	case BACKWARD:
		m_entity->position += front * velocity;
		break;
	case LEFT:
		m_entity->position -= right * velocity;
		break;
	case RIGHT:
		m_entity->position += right * velocity;
		break;
	case UP:
		m_entity->position += up * velocity;
		break;
	case DOWN:
		m_entity->position -= up * velocity;
		break;
	default:
		return;
	}

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
		viewMat = mfg::View(right, up, front, m_entity->position);
		dirty = false;
	}

	return viewMat;
}