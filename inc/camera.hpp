#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "../include/glad/glad.h"
#include "vec3.hpp"
#include "mat4.hpp"
#include <cmath>
#include <iomanip>
#include <ios>
#include <iostream>

//   Y      +Y up       -Y down
//   |
//   +--X   +X right    -X left
//  /
// Z        +Z back     -Z front

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

/* default camera values */
const float YAW         = 0.0f;
const float PITCH       = 0.0f;
const float SPEED       = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM        = 45.0f;

class Camera {
public:
    /* camera attributes */
    Vec3        Position;
    Vec3        Front;
    Vec3        Up;
    Vec3        Right;
    Vec3        WorldUp;

    /* euler angles */
    float       Yaw;
    float       Pitch;

    /* camera options */
    float       MovementSpeed;
    float       MouseSensitivity;
    float       Zoom;

    /* orbit camera */
    enum Camera_Mode {
        ORBITCAM,
        FREECAM,
        SWITCHING
    };
    Camera_Mode cameraMode      = ORBITCAM;
    Vec3        target          {0.0f, 0.0f, 0.0f};
    float       radius          = 10.0f;
    float       targetRadius    = 10.0f;
    float       minRadius       = 0.5f;
    float       maxRadius       = 50.0f;
    float       targetYaw;
    float       targetPitch;

    float snapyaw;
    float snaptyaw;
    float snapdyaw;
    float snapdpyaw;

    /* constructor with vectors */
    Camera(Vec3 position = {0.0f, 0.0f, 0.0f}, Vec3 up = {0.0f, 1.0f, 0.0f}, float yaw = YAW, float pitch = PITCH) :
        Position(position), Front{0.0f, 0.0f, -1.0f}, WorldUp(up), Yaw(yaw), Pitch(pitch), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        updateCameraVectors();
        // updateOrbitCamera();
    }
    /* constructor with scalar values */
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
        Position{posX, posY, posZ}, Front{0.0f, 0.0f, -1.0f}, WorldUp{upX, upY, upZ}, Yaw(yaw), Pitch(pitch), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM) {
        updateCameraVectors();
        // updateOrbitCamera();
    }

    /* returns the view matrix calculated using Euler Angles and the LookAt Matrix */
    Mat4 GetViewMatrix() const {
        return lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        if (cameraMode == FREECAM) {
            float velocity = MovementSpeed * deltaTime;
            if (direction == FORWARD)
                Position += Front * velocity;
            if (direction == BACKWARD)
                Position -= Front * velocity;
            if (direction == LEFT)
                Position -= Right * velocity;
            if (direction == RIGHT)
                Position += Right * velocity;
            if (direction == UP)
                Position += Up * velocity;
            if (direction == DOWN)
                Position -= Up * velocity;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        if (cameraMode == SWITCHING) return;
        Yaw   += xoffset * MouseSensitivity;
        Pitch += yoffset * MouseSensitivity;

        if (Yaw < 0) {
            Yaw += 360;
        }
        if (Yaw > 360) {
            Yaw -= 360;
        }

        /* make sure that when pitch is out of bou nds screen doesnt get flipped */
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

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset) {
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

    /* switch between orbit and free cam */
    void ToggleFreeCam() {
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

            snapyaw = Yaw;
            snaptyaw = targetYaw;
            snapdyaw = targetYaw - Yaw;
            snapdpyaw = targetYaw + 360 - Yaw;

            cameraMode = SWITCHING;
        }
        else {
            cameraMode = FREECAM;
        }
    }

    void Update(float deltaTime) {
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

    void printState(void) {
        std::string orbitBool;
        if (cameraMode == ORBITCAM)
            orbitBool = "true";
        else
            orbitBool = "false";

        std::cout << std::fixed << std::setprecision(3)
            << "x:       " << Position.x   << "      \n"
            << "y:       " << Position.y   << "      \n"
            << "z:       " << Position.z   << "      \n"
            << "\n"
            << "orbit:   " << orbitBool    << "      \n"
            << "yaw:     " << Yaw          << "      \n"
            << "pitch:   " << Pitch        << "      \n"
            << "radius:  " << radius       << "      \n"
            << "\n"
            << "front x: " << Front.x      << "      \n"
            << "front y: " << Front.y      << "      \n"
            << "front z: " << Front.z      << "      \n"
            << "\n"
            << "start:   " << snapyaw          << "      \n"
            << "target:  " << snaptyaw          << "      \n"
            << "delta:   " << snapdyaw          << "      \n"
            << "delta2:  " << snapdpyaw          << "      \n"
            << "add:     " << std::abs(snapdpyaw) + std::abs(snapdyaw)          << "      \n"
            << "\033[18A"  << std::flush;
    }

private:
    /* convert degrees to radians */
    static float radians(float degrees) {
        return degrees * 3.14159265358979323846f / 180.0f;
    }

    /* convert radians to degrees */
    static float degrees(float radians) {
        return radians * 180.0f / 3.14159265358979323846f;
    }

    /* calculates the front vector from the Camera's (updated) Euler Angles */
    void updateCameraVectors() {
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

    void updateOrbitCamera() {
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

    static float smoothlerp(float a, float b, float decay, float dt) {
        // exponential decay lerp
        // exponential decay constant useful range approx. 1 to 25, from slow to fast
        return b+(a-b)*exp(-decay * dt);
    }
};
#endif

