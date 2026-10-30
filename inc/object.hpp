#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "vec3.hpp"
#include "mat4.hpp"
#include "obj_model.hpp"
#include "bmp_image.hpp"

enum class MovementDirections {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Object {
public:
    Object(Vec3 position = {0.0f, 0.0f, 0.0f}, float yaw = 0.0f, float pitch = 0.0f, float roll = 0.0f);

    const ObjModel& getObjModel() const;
    const BmpImage& getBmpTexture() const;
    Mat4 getModelMatrix() const;

    void cleanup();

    bool loadModel(const std::string& path);
    void loadDefaultCube();

    bool loadTexture(const std::string& path);

    void drawObject();

    void processKeyboard(MovementDirections direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset, Vec3 cameraUp, Vec3 cameraRight);

private:
    ObjModel objModel_;
    BmpImage texture_;

    Vec3     position_;
    Vec3     front_;
    Vec3     up_;
    Vec3     right_;

    Mat4     orientation_;

    Vec3     worldForward_;
    Vec3     worldUp_;
    Vec3     worldRight_;

    float    movementSpeed_;
    float    mouseSensitivity_;

    /* convert degrees to radians */
    static float radians(float degrees);

    /* convert radians to degrees */
    static float degrees(float radians);

    void updateModelVectors();
};

#endif
