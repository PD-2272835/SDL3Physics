#include "Camera.hpp"


void Camera::ProcessMovement(cameraMovement direction, double timeDelta)
{
	float velocity = movementSpeed * 10.f * timeDelta;

	//camera moves counter-intuatively, as it is translating the scene, not itself
	//eg-camera should look like it moves right, so the scene should be translated left
	//this is because the projection matrix is used to translate the objects in the scene, not just the camera
	switch (direction)
	{
	case FORWARD:
		m_entity->position += front * velocity;
		break;
	case BACKWARD:
		m_entity->position -= front * velocity;
		break;
	case LEFT:
		m_entity->position += right * velocity;
		break;
	case RIGHT:
		m_entity->position -= right * velocity;
		break;
	case UP:
		m_entity->position -= up * velocity;
		break;
	case DOWN:
		m_entity->position += up * velocity;
		break;
	default:
		return;
	}

	dirty = true;
}

void Camera::ProcessLook(double xOffset, double yOffset)
{

}

void Camera::UpdateVectors()
{
	dirty = true;
}

sgm::mat4 Camera::GetMatrix()
{
	if (dirty)
	{
		//std::cout << "updated view mat\n";
		viewMat = sgm::mul(projMat, sgm::View(right, up, front, m_entity->position));
		dirty = false;
	}

	return viewMat;
}