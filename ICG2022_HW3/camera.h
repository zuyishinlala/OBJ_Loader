#ifndef CAMERA_H
#define CAMERA_H

#include "headers.h"

// Camera Declarations.
class Camera {
public:
	// Camera Public Methods.
	Camera(const float aspectRatio);
	~Camera();

	glm::vec3& GetCameraPos() { return position; }
	glm::mat4x4& GetViewMatrix() { return viewMatrix; }
	glm::mat4x4& GetProjMatrix() { return projMatrix; }

	void UpdateView(const glm::vec3 newPos, const glm::vec3 newTarget, const glm::vec3 up);
	void UpdateProjection(const float fovyInDegree, const float aspectRatio, const float zNear, const float zFar);

private:
	// Camera Private Data.
	glm::vec3 position;
	glm::vec3 target;
	
	float fovy;	// in degree.
	float aspectRatio;
	float nearPlane;
	float farPlane;

	glm::mat4x4 viewMatrix;
	glm::mat4x4 projMatrix;
};

#endif
