#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 indentity();

glm::mat4 translate(const glm::vec3& t);

glm::mat4 scale(const glm::vec3& t);

glm::mat4 rotateX(float angleRad);
glm::mat4 rotateY(float angleRad);
glm::mat4 rotateZ(float angleRad);
glm::mat4 rotate(float angleRad, const glm::vec3& axis);

glm::mat4 lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

glm::mat4 perspective(float fovy, float aspect, float zNear, float zFar);

glm::mat4 ortho(float left, float rightr, float bottom, float top, float zNear, float zFar);