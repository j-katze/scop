#include "object.hpp"

Object::Object(Vec3 position, float yaw, float pitch, float roll)
    :   position_(position),
        front_{0.0f, 0.0f, -1.0f},
        up_{0.0f, 1.0f, 0.0f},
        right_{1.0f, 0.0f, 0.0f},
        orientation_(Mat4::identityMatrix()),
        worldForward_{0.0f, 0.0f, -1.0f},
        worldUp_{0.0f, 1.0f, 0.0f},
        worldRight_{1.0f, 0.0f, 0.0f},
        movementSpeed_(2.5f),
        mouseSensitivity_(0.5f)
{
    orientation_ = rotate(orientation_, radians(yaw), worldUp_);
    orientation_ = rotate(orientation_, radians(pitch), right_);
    orientation_ = rotate(orientation_, radians(roll), {0.0f, 0.0f, 1.0f});
}

const ObjModel& Object::getObjModel() const {
    return objModel_;
}

const BmpImage& Object::getBmpTexture() const {
    return texture_;
}

Mat4 Object::getModelMatrix() const {
    Mat4 matrix = Mat4::identityMatrix();

    matrix = translate(matrix, position_);
    return multiply(matrix, orientation_);
}

void Object::cleanup() {
    objModel_.cleanup();
    texture_.cleanup();
}

bool Object::loadModel(const std::string &path) {
    return objModel_.load(path);
}

void Object::loadDefaultCube() {
    objModel_.defaultCube();
}

bool Object::loadTexture(const std::string &path) {
    return texture_.loadTexture(path);
}

void Object::drawObject() {
    if (objModel_.getVAO() == 0 || objModel_.getVBO() == 0) {
        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_.getTextureId());

    glBindVertexArray(objModel_.getVAO());
    glDrawArrays(GL_TRIANGLES, 0, objModel_.getVertexCount());
    glBindVertexArray(0);
}

void Object::processKeyboard(MovementDirections direction, float deltaTime) {
    float velocity = movementSpeed_ * deltaTime;

    if (direction == MovementDirections::FORWARD) {
        position_ += worldForward_ * velocity;
    }
    if (direction == MovementDirections::BACKWARD) {
        position_ -= worldForward_ * velocity;
    }
    if (direction == MovementDirections::LEFT) {
        position_ -= worldRight_ * velocity;
    }
    if (direction == MovementDirections::RIGHT) {
        position_ += worldRight_ * velocity;
    }
    if (direction == MovementDirections::UP) {
        position_ += worldUp_ * velocity;
    }
    if (direction == MovementDirections::DOWN) {
        position_ -= worldUp_ * velocity;
    }
}

void Object::processMouseMovement(float xoffset, float yoffset, Vec3 cameraUp, Vec3 cameraRight) {
    const float yawDelta   = radians(xoffset * mouseSensitivity_);
    const float pitchDelta = radians(-yoffset * mouseSensitivity_);

    const Mat4 screenYaw   = rotate(Mat4::identityMatrix(), yawDelta, cameraUp);
    const Mat4 screenPitch = rotate(Mat4::identityMatrix(), pitchDelta, cameraRight);

    orientation_ = multiply(screenPitch, orientation_);
    orientation_ = multiply(screenYaw, orientation_);

    updateModelVectors();
}

/* convert degrees to radians */
float Object::radians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

/* convert radians to degrees */
float Object::degrees(float radians) {
    return radians * 180.0f / 3.14159265358979323846f;
}

void Object::updateModelVectors() {
    right_ = normalize({orientation_[0], orientation_[1], orientation_[2]});
    up_    = normalize({orientation_[4], orientation_[5], orientation_[6]});
    front_ = normalize({orientation_[8], orientation_[9], orientation_[10]});
}
