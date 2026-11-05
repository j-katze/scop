#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "vec3.hpp"
#include "mat4.hpp"
#include "object.hpp"

//   Y      +Y up    -Y down
//   |
//   +--X   +X right -X left
//  /
// Z        +Z back  -Z front

/* default camera values */
const float YAW         = 0.0f;
const float PITCH       = 0.0f;
const float SPEED       = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV        = 45.0f;

struct CameraVectors {
    Vec3 front;
    Vec3 up;
    Vec3 right;
};

class Camera{
public:
    Camera(
        Vec3 position = {0.0f, 0.0f, 0.0f},
        Vec3 up = {0.0f, 1.0f, 0.0f},
        float yaw = YAW, float pitch = PITCH
    );

    CameraVectors getCameraVectors() const;
    const bool& getCameraActive() const;

    /* returns the view matrix calculated using Euler Angles and the LookAt Matrix */
    Mat4 getViewMatrix() const;
    Mat4 getPerspectiveMatrix(float screenWidth, float screenHeight) const;

    void toggleCameraActive();

    void processKeyboard(MovementDirections direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void processMouseScroll(float yoffset);

    /* switch between orbit and free cam */
    void toggleFreeCam();

    void update(const Shader& shader, float deltaTime, float width, float height);

    void printState(void);

private:
    enum class CameraMode {
        ORBITCAM,
        FREECAM,
        SWITCHING
    };

    Vec3       position_;
    Vec3       front_;
    Vec3       up_;
    Vec3       right_;
    Vec3       worldUp_;

    /* euler angles */
    float      yaw_;
    float      pitch_;

    /* camera options */
    float      movementSpeed_;
    float      mouseSensitivity_;
    float      fieldOfView_;

    CameraMode cameraMode_;

    bool       cameraActive_;

    /* orbit camera */
    Vec3       target_;
    float      radius_;
    float      targetRadius_;
    float      minRadius_;
    float      maxRadius_;
    float      targetYaw_;
    float      targetPitch_;

    float      snapYawStart_;
    float      snapYawTarget_;
    float      snapYawDelta1_;
    float      snapYawDelta2_;

    /* calculates the front vector from the Camera's (updated) Euler Angles */
    void updateCameraVectors();
    void updateOrbitCameraVectors();
};
#endif
