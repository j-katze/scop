#include "camera.hpp"

#include <cmath>
#include <iomanip>
#include <ios>
#include <iostream>

Camera::Camera(Vec3 position, Vec3 up, float yaw, float pitch)
    :   position_(position),
        front_{0.0f, 0.0f, -1.0f},
        worldUp_(up),
        yaw_(yaw),
        pitch_(pitch),
        movementSpeed_(SPEED),
        mouseSensitivity_(SENSITIVITY),
        fieldOfView_(FOV),
        cameraMode_(CameraMode::ORBITCAM),
        cameraActive_(false),
        target_{0.0f, 0.0f, 0.0f},
        radius_(10.0f),
        targetRadius_(10.0f),
        minRadius_(0.5f),
        maxRadius_(50.0f)
{
    updateCameraVectors();
}

CameraVectors Camera::getCameraVectors() const {
    return {front_, up_, right_};
}

bool Camera::getCameraActive() const {
    return cameraActive_;
}

Mat4 Camera::getViewMatrix() const {
    return lookAt(position_, position_ + front_, up_);
}

Mat4 Camera::getPerspectiveMatrix(float screenWidth, float screenHeight) const {
    return perspective(radians(fieldOfView_), screenWidth / screenHeight, 0.1f, 100.0f);
}

void Camera::toggleCameraActive() {
    cameraActive_ = !cameraActive_;
}

void Camera::processKeyboard(MovementDirections direction, float deltaTime) {
    if (cameraMode_ == CameraMode::FREECAM && cameraActive_) {
        float velocity = movementSpeed_ * deltaTime;

        if (direction == MovementDirections::FORWARD) {
            position_ += front_ * velocity;
        }
        if (direction == MovementDirections::BACKWARD) {
            position_ -= front_ * velocity;
        }
        if (direction == MovementDirections::LEFT) {
            position_ -= right_ * velocity;
        }
        if (direction == MovementDirections::RIGHT) {
            position_ += right_ * velocity;
        }
        if (direction == MovementDirections::UP) {
            position_ += up_ * velocity;
        }
        if (direction == MovementDirections::DOWN) {
            position_ -= up_ * velocity;
        }
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset) {
    if (cameraMode_ == CameraMode::SWITCHING || !cameraActive_) {
        return;
    }

    yaw_   += xoffset * mouseSensitivity_;
    pitch_ += yoffset * mouseSensitivity_;

    if (yaw_ < 0) {
        yaw_ += 360;
    }
    if (yaw_ > 360) {
        yaw_ -= 360;
    }

    /* make sure that when pitch is out of bounds screen doesnt get flipped */
    if (pitch_ > 89.0f) {
        pitch_ = 89.0f;
    }
    if (pitch_ < -89.0f) {
        pitch_ = -89.0f;
    }

    if (cameraMode_ == CameraMode::ORBITCAM) {
        updateOrbitCameraVectors();
    }
    else {
        updateCameraVectors();
    }
}

void Camera::processMouseScroll(float yoffset) {
    if (cameraMode_ == CameraMode::ORBITCAM && cameraActive_) {
        targetRadius_ -= yoffset;
        if (targetRadius_ < minRadius_) {
            targetRadius_ = minRadius_;
        }
        if (targetRadius_ > maxRadius_) {
            targetRadius_ = maxRadius_;
        }
    }
    // else if (!cameraActive_) {
    //     FieldOfView -= (float)yoffset;
    //     if (FieldOfView < 1.0f)
    //         FieldOfView = 1.0f;
    //     if (FieldOfView > 45.0f)
    //         FieldOfView = 45.0f;
    // }
}

void Camera::toggleFreeCam() {
    if (cameraMode_ == CameraMode::FREECAM) {

        /* position relative to target (center) */
        Vec3 offset = position_ - target_;
        radius_ = targetRadius_ = offset.length();

        if (targetRadius_ < minRadius_) {
            targetRadius_ = minRadius_;
        }
        if (targetRadius_ > maxRadius_) {
            targetRadius_ = maxRadius_;
        }

        /* converting the offset vector to euler angles */
        targetYaw_ = degrees(atan2(-offset.x, offset.z));
        targetPitch_ = degrees(asin(-offset.y / offset.length()));

        snapYawStart_  = yaw_;
        snapYawTarget_ = targetYaw_;
        snapYawDelta1_ = targetYaw_ - yaw_;
        snapYawDelta2_ = targetYaw_ + 360 - yaw_;

        cameraMode_ = CameraMode::SWITCHING;
    }
    else {
        cameraMode_ = CameraMode::FREECAM;
    }
}

void Camera::update(const Shader& shader, float deltaTime, float width, float height) {
    if (cameraMode_ == CameraMode::SWITCHING) {
        const float smoothing = 16.0f;

        float yawDelta;
        if (std::abs(targetYaw_ - yaw_) < std::abs(targetYaw_ + 360.0f - yaw_)) {
            yawDelta = targetYaw_ - yaw_;
        }
        else {
            yawDelta = targetYaw_ + 360.0f - yaw_;
        }

        yaw_ = smoothlerp(yaw_, yaw_ + yawDelta, smoothing, deltaTime);
        pitch_ = smoothlerp(pitch_, targetPitch_, smoothing, deltaTime);

        updateCameraVectors();

        /* if difference between current and target small enough stop lerping */
        const float dif = 0.3f;
        if (std::abs(yawDelta) < dif && std::abs(targetPitch_ - pitch_) < dif) {
            yaw_ = targetYaw_;
            pitch_ = targetPitch_;
            cameraMode_ = CameraMode::ORBITCAM;
        }
    }
    if (cameraMode_ == CameraMode::ORBITCAM && cameraActive_) {
        radius_ = smoothlerp(radius_, targetRadius_, 8.0f, deltaTime);
        updateOrbitCameraVectors();
    }
    // printState();

    shader.setMat4("projection", getPerspectiveMatrix(width, height));
    shader.setMat4("view", getViewMatrix());
}

void Camera::printState(void) {
    std::string modeStr;
    if (cameraMode_ == CameraMode::ORBITCAM) {
        modeStr = "orbit";
    }
    else if (cameraMode_ == CameraMode::FREECAM) {
        modeStr = "free";
    }
    else if (cameraMode_ == CameraMode::SWITCHING) {
        modeStr = "switch";
    }

    std::string activeStr;
    if (cameraActive_ == true) {
        activeStr = "true";
    }
    else {
        activeStr = "false";
    }

    std::cout << std::fixed << std::setprecision(3)
        << "x:       " << position_.x    << "      \n"
        << "y:       " << position_.y    << "      \n"
        << "z:       " << position_.z    << "      \n"
        << "\n"
        << "active:  " << activeStr      << "      \n"
        << "mode:    " << modeStr        << "      \n"
        << "yaw:     " << yaw_           << "      \n"
        << "pitch:   " << pitch_         << "      \n"
        << "radius:  " << radius_        << "      \n"
        << "\n"
        << "front x: " << front_.x       << "      \n"
        << "front y: " << front_.y       << "      \n"
        << "front z: " << front_.z       << "      \n"
        << "\n"
        << "start:   " << snapYawStart_  << "      \n"
        << "target:  " << snapYawTarget_ << "      \n"
        << "delta:   " << snapYawDelta1_ << "      \n"
        << "delta2:  " << snapYawDelta2_ << "      \n"
        << "add:     " << std::abs(snapYawDelta1_) + std::abs(snapYawDelta2_) << "      \n"
        << "\033[19A"  << std::flush;
}

/* calculates the front vector from the Camera's (updated) Euler Angles */
void Camera::updateCameraVectors() {
    Vec3 front;
    const float yawRad = radians(yaw_);
    const float pitchRad = radians(pitch_);

    /* converting euler angles to 3d vector */
    front.x = sin(yawRad) * cos(pitchRad);
    front.y = sin(pitchRad);
    front.z = -cos(yawRad) * cos(pitchRad);

    front_ = normalize(front);
    right_ = normalize(cross(front_, worldUp_));
    up_ = normalize(cross(right_, front_));
}

void Camera::updateOrbitCameraVectors() {
    const float yawRad = radians(yaw_);
    const float pitchRad = radians(pitch_);

    /* placing the camera on a sphere defined by radius around the target  */
    position_.x = target_.x - radius_ * sin(yawRad) * cos(pitchRad);
    position_.y = target_.y - radius_ * sin(pitchRad);
    position_.z = target_.z - radius_ * -cos(yawRad) * cos(pitchRad);

    front_ = normalize(target_ - position_);
    right_ = normalize(cross(front_, worldUp_));
    up_ = normalize(cross(right_, front_));
}
