#include "object.hpp"
#include "gl_functions.hpp"

#include <cmath>

Object::Object(Vec3 position, float yaw, float pitch, float roll)
    :   position_(position),
        orientation_(Mat4::identityMatrix()),
        scale_{1.0f, 1.0f, 1.0f},
        front_{0.0f, 0.0f, -1.0f},
        up_{0.0f, 1.0f, 0.0f},
        right_{1.0f, 0.0f, 0.0f},
        worldForward_{0.0f, 0.0f, -1.0f},
        worldUp_{0.0f, 1.0f, 0.0f},
        worldRight_{1.0f, 0.0f, 0.0f},
        movementSpeed_(2.5f),
        mouseSensitivity_(0.5f)
{
    orientation_ = rotate(orientation_, radians(yaw), worldUp_);
    orientation_ = rotate(orientation_, radians(pitch), right_);
    orientation_ = rotate(orientation_, radians(roll), {0.0f, 0.0f, 1.0f});

    targetScale_ = scale_;
}

Object::~Object() {
    destroy();
}

const ObjModel& Object::getObjModel() const {
    return objModel_;
}

const BmpImage& Object::getBmpTexture() const {
    return texture_;
}

Mat4 Object::getModelMatrix() const {
    /* multiplication order: T * R * S */
    Mat4 matrix = Mat4::identityMatrix();

    matrix = translate(matrix, position_);
    matrix = multiply(matrix, orientation_);
    matrix = multiply(matrix, scale(Mat4::identityMatrix(), scale_));
    return matrix;
}

void Object::destroy() {
    objModel_.destroy();
    texture_.destroy();
}

bool Object::loadModel(const std::string &path) {
    return objModel_.load(path);
}

bool Object::loadDefaultCube() {
    if (!objModel_.defaultCube()) {
        return false;
    }
    if (texture_.isImageDataEmpty() && !texture_.loadDefaultTexture()) {
        return false;
    }
    return true;
}

bool Object::loadTexture(const std::string &path) {
    return texture_.loadTexture(path);
}

bool Object::loadDefaultTexture() {
    return texture_.loadDefaultTexture();
}

void Object::update(const Shader& shader, float deltaTime) {
    const float smoothing = 16.0f;

    scale_.x = smoothlerp(scale_.x, targetScale_.x, smoothing, deltaTime);
    scale_.y = smoothlerp(scale_.y, targetScale_.y, smoothing, deltaTime);
    scale_.z = smoothlerp(scale_.z, targetScale_.z, smoothing, deltaTime);

    shader.setMat4("model", getModelMatrix());

    if (objModel_.getVAO() == 0 || objModel_.getVBO() == 0) {
        return;
    }

    /* bind texture to texture onit 0 */
    gl.ActiveTexture(GL_TEXTURE0);
    gl.BindTexture(GL_TEXTURE_2D, texture_.getTextureId());

    /* bind model */
    gl.BindVertexArray(objModel_.getVAO());

    /* draw model */
    gl.DrawArrays(GL_TRIANGLES, 0, objModel_.getVertexCount());

    /* unbind texture and model */
    gl.BindTexture(GL_TEXTURE_2D, 0);
    gl.BindVertexArray(0);
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
    if (xoffset == 0.0f && yoffset == 0.0f) {
        return;
    }

    const float yawDelta   = radians(xoffset * mouseSensitivity_);
    const float pitchDelta = radians(-yoffset * mouseSensitivity_);

    const Mat4 screenYaw   = rotate(Mat4::identityMatrix(), yawDelta, cameraUp);
    const Mat4 screenPitch = rotate(Mat4::identityMatrix(), pitchDelta, cameraRight);

    orientation_ = multiply(screenPitch, orientation_);
    orientation_ = multiply(screenYaw, orientation_);

    updateModelVectors();
}

void Object::processMouseScroll(float yoffset) {
    float scrollSpeed = 0.5;
    float scrollVelocity = yoffset * scrollSpeed;
    float minScale = 0.1f;

    targetScale_ = {targetScale_.x + scrollVelocity, targetScale_.y + scrollVelocity, targetScale_.z + scrollVelocity};
    if (targetScale_.x < minScale || scale_.y < minScale || scale_.z < minScale) {
        targetScale_.x = minScale;
        targetScale_.y = minScale;
        targetScale_.z = minScale;
    }
}

void Object::updateModelVectors() {
    right_ = normalize({orientation_[0], orientation_[1], orientation_[2]});
    up_    = normalize({orientation_[4], orientation_[5], orientation_[6]});
    front_ = normalize({orientation_[8], orientation_[9], orientation_[10]});
}

float radians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

float degrees(float radians) {
    return radians * 180.0f / 3.14159265358979323846f;
}

float smoothlerp(float a, float b, float decay, float dt) {
    /* exponential decay lerp */
    /* exponential decay constant useful range approx. 1 to 25, from slow to fast */
    return b+(a-b)*exp(-decay * dt);
}
