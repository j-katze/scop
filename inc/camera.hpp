#ifndef CAMERA_H
#define CAMERA_H

#include "../include/glad/glad.h"
#include "../inc/mat4.hpp"

/* Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods */
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

/* Default camera values */
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


/* An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL */
class Camera {
public:
    // camera Attributes
    Vec3 Position;
    Vec3 Front;
    Vec3 Up;
    Vec3 Right;
    Vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(
        Vec3 position = {0.0f, 0.0f, 0.0f},
        Vec3 up = {0.0f, 1.0f, 0.0f},
        float yaw = YAW,
        float pitch = PITCH
    ) : Position(position),
        Front{0.0f, 0.0f, -1.0f},
        Up{},
        Right{},
        WorldUp(up),
        Yaw(yaw),
        Pitch(pitch),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM)
    {
        // updateCameraVectors();
    }
    // constructor with scalar values
    Camera(
        float posX, float posY, float posZ,
        float upX, float upY, float upZ,
        float yaw, float pitch
    ) : Camera({posX, posY, posZ}, {upX, upY, upZ}, yaw, pitch)
    {
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    Mat4 GetViewMatrix() const {
        return lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    static float radians(float degrees) {
        return degrees * 3.14159265358979323846f / 180.0f;
    }
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        Vec3 front;
        front.x = cos(radians(Yaw)) * cos(radians(Pitch));
        front.y = sin(radians(Pitch));
        front.z = sin(radians(Yaw)) * cos(radians(Pitch));
        Front = normalize(front);
        // also re-calculate the Right and Up vector
        Right = normalize(cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = normalize(cross(Right, Front));
    }
};
#endif

