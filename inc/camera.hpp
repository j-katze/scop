#pragma once

#ifndef camera_hpp
#define camera_hpp

#include "../include/glad/glad.h"
#include "vec3.hpp"
#include "mat4.hpp"

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

    /* constructor with vectors */
    Camera(
        Vec3 position = {0.0f, 0.0f, 0.0f},
        Vec3 up = {0.0f, 1.0f, 0.0f},
        float yaw = YAW, float pitch = PITCH
    );

    /* constructor with scalar values */
    Camera(
        float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch
    );

    /* returns the view matrix calculated using Euler Angles and the LookAt Matrix */
    Mat4 GetViewMatrix() const;

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    void ProcessMouseScroll(float yoffset);

    /* switch between orbit and free cam */
    void ToggleFreeCam();

    void Update(float deltaTime);

    void printState(void);

private:
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
    float snapYawStart;
    float snapYawTarget;
    float snapYawDelta1;
    float snapYawDelta2;

    /* convert degrees to radians */
    static float radians(float degrees);

    /* convert radians to degrees */
    static float degrees(float radians);

    /* calculates the front vector from the Camera's (updated) Euler Angles */
    void updateCameraVectors();

    void updateOrbitCamera();

    static float smoothlerp(float a, float b, float decay, float dt);
};
#endif

