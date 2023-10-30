#include "camera.h"

Camera::Camera(const float aspectRatio)
{
	// Default camera pose and parameters.
	position = glm::vec3(0.0f, 0.0f, 3.0f);
	target = glm::vec3(0.0f, 0.0f, 0.0f);
	fovy = 45.0f;
	nearPlane = 0.1f;
	farPlane = 1000.0f;
	UpdateView(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
	UpdateProjection(fovy, aspectRatio, nearPlane, farPlane);
}

Camera::~Camera() 
{}

void Camera::UpdateView(const glm::vec3 newPos, const glm::vec3 newTarget, const glm::vec3 up)
{
	position = newPos;
	target = newTarget;
	viewMatrix = glm::lookAt(position, target, up);
}

void Camera::UpdateProjection(const float fovyInDegree, const float aspectRatio, const float zNear, const float zFar)
{
	fovy = fovyInDegree;
	nearPlane = zNear;
	farPlane = zFar;
	projMatrix = glm::perspective(glm::radians(fovyInDegree), aspectRatio, nearPlane, farPlane);
}