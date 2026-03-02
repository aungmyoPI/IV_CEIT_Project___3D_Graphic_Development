#include <coordinate_system.h>

glm::mat4 indentity(){
    return glm::mat4(1.0f);
}

glm::mat4 translate(const glm::vec3& t){
    glm::mat4 result = indentity();
    result[3] = glm::vec4(t.x, t.y, t.z, 1.0f);
    return result;
}

glm::mat4 scale(const glm::vec3& s){
    glm::mat4 result = indentity();
    result[0][0] = s.x;
    result[1][1] = s.y;
    result[2][2] = s.z;
    return result;
}

glm::mat4 rotateX(float angleRad){
    float c = cos(angleRad);
    float s = sin(angleRad);
    glm::mat4 result = indentity();
    result[1][1] =  c;
    result[1][2] = -s;
    result[2][1] =  s;
    result[2][2] =  c;
    return result;
}

glm::mat4 rotateY(float angleRad){
    float c = cos(angleRad);
    float s = sin(angleRad);
    glm::mat4 result = indentity();
    result[0][0] =  c;
    result[0][2] =  s;
    result[2][0] = -s;
    result[2][2] =  c;
    return result;
}

glm::mat4 rotateZ(float angleRad){
    float c = cos(angleRad);
    float s = sin(angleRad);
    glm::mat4 result = indentity();
    result[0][0] =  c;
    result[0][1] = -s;
    result[1][0] =  s;
    result[1][1] =  c;
    return result;
}

glm::mat4 rotate(float angleRad, const glm::vec3& axis){
    glm::vec3 a = glm::normalize(axis);
    float c = cos(angleRad);
    float s = sin(angleRad);
    float t = 1.0f - c;
    float x = a.x;
    float y = a.y;
    float z = a.z;
    glm::mat4 result = indentity();
    result[0][0] = t * x * x + c;
    result[0][1] = t * x * y - s * z;
    result[0][2] = t * x * z + s * y;
    result[1][0] = t * x * y + s * z;
    result[1][1] = t * y * y + c;
    result[1][2] = t * y * z - s * x;
    result[2][0] = t * x * z - s * y;
    result[2][1] = t * y * z + s * x;
    result[2][2] = t * z * z + c;
    return result;
}

glm::mat4 lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up){
    glm::vec3 f = glm::normalize(center - eye);
    glm::vec3 s = glm::normalize(glm::cross(f, up));
    glm::vec3 u = glm::cross(s, f);
    glm::mat4 result = indentity();
    // First column: s (right)
    result[0][0] = s.x;
    result[1][0] = s.y; // wait – careful: GLM is column-major. In memory, column 0 is first 4 floats.
    result[2][0] = s.z;
    result[3][0] = 0.0f;

    // Second column: u (up)
    result[0][1] = u.x;
    result[1][1] = u.y;
    result[2][1] = u.z;
    result[3][1] = 0.0f;

    // Third column: -f (forward, negated because we want to look towards -z)
    result[0][2] = -f.x;
    result[1][2] = -f.y;
    result[2][2] = -f.z;
    result[3][2] = 0.0f;

    // Fourth column: translation part ( -eye transformed by rotation )
    result[0][3] = -glm::dot(s, eye);
    result[1][3] = -glm::dot(u, eye);
    result[2][3] = glm::dot(f, eye);
    result[3][3] = 1.0f;
    return result;
}

glm::mat4 perspective(float fovy, float aspect, float zNear, float zFar){
    float tanHalfFovy = tan(fovy / 2.0f);
    glm::mat4 result = glm::mat4(0.0f);
    result[0][0] = 1.0f / (aspect * tanHalfFovy);
    result[1][1] = 1.0f / (tanHalfFovy);
    result[2][2] = -(zFar + zNear) / (zFar - zNear);
    result[2][3] = -1.0f; // this goes into the w component
    result[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
    return result;
}