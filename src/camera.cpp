#include "camera.hpp"

#include <cmath>
#include <iomanip>
#include <ios>
#include <iostream>

Camera::Camera(Vec3 position, Vec3 up, float yaw, float pitch)
    :   Position(position),
        Front{0.0f, 0.0f, -1.0f},
        WorldUp(up),
        Yaw(yaw),
        Pitch(pitch),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM)
{
    updateCameraVectors();
    // updateOrbitCamera();
}

Camera::Camera(
    float posX, float posY, float posZ,
    float upX, float upY, float upZ,
    float yaw, float pitch)
    :   Position{posX, posY, posZ},
        Front{0.0f, 0.0f, -1.0f},
        WorldUp{upX, upY, upZ},
        Yaw(yaw), Pitch(pitch),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM)
{
    updateCameraVectors();
    // updateOrbitCamera();
}

Mat4 Camera::GetViewMatrix() const {
    return lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    if (cameraMode == FREECAM) {
        float velocity = MovementSpeed * deltaTime;

        if (direction == FORWARD) {
            Position += Front * velocity;
        }
        if (direction == BACKWARD) {
            Position -= Front * velocity;
        }
        if (direction == LEFT) {
            Position -= Right * velocity;
        }
        if (direction == RIGHT) {
            Position += Right * velocity;
        }
        if (direction == UP) {
            Position += Up * velocity;
        }
        if (direction == DOWN) {
            Position -= Up * velocity;
        }
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    if (cameraMode == SWITCHING) return;
    Yaw   += xoffset * MouseSensitivity;
    Pitch += yoffset * MouseSensitivity;

    if (Yaw < 0) {
        Yaw += 360;
    }
    if (Yaw > 360) {
        Yaw -= 360;
    }

    /* make sure that when pitch is out of bounds screen doesnt get flipped */
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    if (cameraMode == ORBITCAM) {
        // updateOrbitCamera();
    }
    else {
        updateCameraVectors();
    }
}

void Camera::ProcessMouseScroll(float yoffset) {
    if (cameraMode == ORBITCAM) {
        targetRadius -= yoffset;
        if (targetRadius < minRadius) {
            targetRadius = minRadius;
        }
        if (targetRadius > maxRadius) {
            targetRadius = maxRadius;
        }
    }
    else {
        // Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }
}

void Camera::ToggleFreeCam() {
    if (cameraMode == FREECAM) {

        /* position relative to target (center) */
        Vec3 offset = Position - target;
        radius = targetRadius = offset.length();

        if (targetRadius < minRadius) {
            targetRadius = minRadius;
        }
        if (targetRadius > maxRadius) {
            targetRadius = maxRadius;
        }

        /* converting the offset vector to euler angles */
        targetYaw = degrees(atan2(-offset.x, offset.z));
        targetPitch = degrees(asin(-offset.y / offset.length()));

        snapYawStart  = Yaw;
        snapYawTarget = targetYaw;
        snapYawDelta1 = targetYaw - Yaw;
        snapYawDelta2 = targetYaw + 360 - Yaw;

        cameraMode = SWITCHING;
    }
    else {
        cameraMode = FREECAM;
    }
}

void Camera::Update(float deltaTime) {
    if (cameraMode == SWITCHING) {
        const float smoothing = 16.0f;

        /* dividend / divisor = quotient */
        /* remainder = dividend - quotient * divisor */
        /* targetYaw = 10, Yaw = 350 */
        /* 10-350=-340 */
        /* -340/360 = -0.944 remainder rounds to nearest integer -> -1 */
        /* remainder = -340 - -1 * 360 = 20 */
        /* 350 = 20 = 370 = 10 */
        /* going over 360 to 370 ends up at the same place as going down from 350 to 10 but is shorter */
        // float yawDelta = std::remainder(targetYaw - Yaw, 360.0f);

        float yawDelta;
        if (std::abs(targetYaw - Yaw) < std::abs(targetYaw + 360.0f - Yaw)) {
            yawDelta = targetYaw - Yaw;
        }
        else {
            yawDelta = targetYaw + 360.0f - Yaw;
        }

        // std::cout << yawDelta << std::endl;
        Yaw = smoothlerp(Yaw, Yaw + yawDelta, smoothing, deltaTime);
        Pitch = smoothlerp(Pitch, targetPitch, smoothing, deltaTime);

        updateCameraVectors();

        /* if difference between current and target small enough stop lerping */
        const float dif = 0.3f;
        if (std::abs(yawDelta) < dif && std::abs(targetPitch - Pitch) < dif) {
            Yaw = targetYaw;
            Pitch = targetPitch;
            cameraMode = ORBITCAM;
        }
    }
    if (cameraMode == ORBITCAM) {
        radius = smoothlerp(radius, targetRadius, 8.0f, deltaTime);
        updateOrbitCamera();
    }
}

void Camera::printState(void) {
    std::string orbitBool;
    if (cameraMode == ORBITCAM)
        orbitBool = "true";
    else
        orbitBool = "false";

    std::cout << std::fixed << std::setprecision(3)
        << "x:       " << Position.x    << "      \n"
        << "y:       " << Position.y    << "      \n"
        << "z:       " << Position.z    << "      \n"
        << "\n"
        << "orbit:   " << orbitBool     << "      \n"
        << "yaw:     " << Yaw           << "      \n"
        << "pitch:   " << Pitch         << "      \n"
        << "radius:  " << radius        << "      \n"
        << "\n"
        << "front x: " << Front.x       << "      \n"
        << "front y: " << Front.y       << "      \n"
        << "front z: " << Front.z       << "      \n"
        << "\n"
        << "start:   " << snapYawStart  << "      \n"
        << "target:  " << snapYawTarget << "      \n"
        << "delta:   " << snapYawDelta1 << "      \n"
        << "delta2:  " << snapYawDelta2 << "      \n"
        << "add:     " << std::abs(snapYawDelta1) + std::abs(snapYawDelta2) << "      \n"
        << "\033[18A"  << std::flush;
}

/* convert degrees to radians */
float Camera::radians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

/* convert radians to degrees */
float Camera::degrees(float radians) {
    return radians * 180.0f / 3.14159265358979323846f;
}

/* calculates the front vector from the Camera's (updated) Euler Angles */
void Camera::updateCameraVectors() {
    Vec3 front;
    const float yawRad = radians(Yaw);
    const float pitchRad = radians(Pitch);

    /* converting euler angles to 3d vector */
    front.x = sin(yawRad) * cos(pitchRad);
    front.y = sin(pitchRad);
    front.z = -cos(yawRad) * cos(pitchRad);

    Front = normalize(front);
    Right = normalize(cross(Front, WorldUp));
    Up = normalize(cross(Right, Front));
}

void Camera::updateOrbitCamera() {
    const float yawRad = radians(Yaw);
    const float pitchRad = radians(Pitch);

    /* placing the camera on a sphere defined by radius around the target  */
    Position.x = target.x - radius * sin(yawRad) * cos(pitchRad);
    Position.y = target.y - radius * sin(pitchRad);
    Position.z = target.z - radius * -cos(yawRad) * cos(pitchRad);

    Front = normalize(target - Position);
    Right = normalize(cross(Front, WorldUp));
    Up = normalize(cross(Right, Front));
}

float Camera::smoothlerp(float a, float b, float decay, float dt) {
    // exponential decay lerp
    // exponential decay constant useful range approx. 1 to 25, from slow to fast
    return b+(a-b)*exp(-decay * dt);
}
