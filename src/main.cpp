#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "mat4.hpp"

#include <cstdlib>
#include <iostream>
#include <unordered_map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void error_callback(int error, const char* description);
void processInput(GLFWwindow *window);

/* settings */
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

/* camera */
//   Y      +Y up    -Y down
//   |
//   +--X   +X right -X left
//  /
// Z        +Z back  -Z front

//             X     Y     Z
Camera camera({0.0f, 0.0f, 10.0f});
Object model;
Object model2;

float  lastX = SCR_WIDTH / 2.0f;
float  lastY = SCR_HEIGHT / 2.0f;
bool   firstMouse = true;

/* timing */
float deltaTime = 0.0f;
float lastFrame = 0.0f;

enum class RenderMode {
    SINGLECOLOUR,
    VERTEXCOLOUR,
    TEXTURE
};

RenderMode currentMode = RenderMode::VERTEXCOLOUR;
RenderMode nextMode = RenderMode::VERTEXCOLOUR;
float blendAmount = 1.0f;

bool renderWireframe = false;

int main(int argc, char* argv[]) {
    /* initialize */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return EXIT_FAILURE;
    }

    /* use OpenGl 3.3 core profile (core profile excludes older and deprecated features) */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* create window */
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "scop", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    /* set up event callbacks */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetErrorCallback(error_callback);

    /* load OpenGL function pointers via GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load OpenGL functions with GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);

    glEnable(GL_DEPTH_TEST);

    /*loading shader */
    Shader shader("shaders/vertex.vert", "shaders/fragment.frag");

    /* loading model and texture */
    const bool modelLoaded = argc > 1 && model.loadModel(argv[1]);
    const bool textureLoaded = argc > 2 && model.loadTexture(argv[2]);
    if (!modelLoaded) {
        model.loadDefaultCube();
    }
    if (!textureLoaded) {
        model.loadDefaultTexture();
    }
    model2.loadDefaultCube();

    /* render loop */
    while (!glfwWindowShouldClose(window)) {
        /* get deltatime */
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* process input */
        processInput(window);
        camera.update(deltaTime);
        // camera.printState();

        /* clear buffers */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* activate the shader */
        shader.use();

        /* blend between render modes if mode was switched */
        if (blendAmount < 1.0f) {
            blendAmount = smoothlerp(blendAmount, 1.0f, 6.0f, deltaTime);
            if (std::abs(blendAmount - 1.0f) < 0.02f) {
                blendAmount = 1.0f;
                currentMode = nextMode;
            }
        }

        shader.setInt("currentMode", static_cast<int>(currentMode));
        shader.setInt("nextMode", static_cast<int>(nextMode));
        shader.setFloat("blendAmount", blendAmount);

        /* pass projection matrix to shader */
        Mat4 projectionMat = camera.getPerspectiveMatrix((float)SCR_WIDTH, (float)SCR_HEIGHT);
        shader.setMat4("projection", projectionMat);

        /* camera / view transformations */
        Mat4 viewMat = camera.getViewMatrix();
        shader.setMat4("view", viewMat);

        model.update(shader, deltaTime);
        model2.update(shader, deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.destroy();
    model.destroy();
    glfwDestroyWindow(window);
    /* terminate program, clearing all allocated glfw resources */
    glfwTerminate();
    return EXIT_SUCCESS;
}

void switchRendermode(RenderMode newMode) {
    if (blendAmount >= 1.0f && newMode != currentMode) {
        nextMode = newMode;
        blendAmount = 0.0f;
    }
}

bool pressKeyOnce(GLFWwindow *window, int key) {
    static std::unordered_map<int, bool> wasPressed;
    const bool isPressed = glfwGetKey(window, key) == GLFW_PRESS;
    bool result = false;

    if (isPressed && !wasPressed[key]) {
        result = true;
    }
    wasPressed[key] = isPressed;
    return result;
}

/* process input */
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (camera.getCameraActive()) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            camera.processKeyboard(MovementDirections::FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            camera.processKeyboard(MovementDirections::BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            camera.processKeyboard(MovementDirections::LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            camera.processKeyboard(MovementDirections::RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            camera.processKeyboard(MovementDirections::UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            camera.processKeyboard(MovementDirections::DOWN, deltaTime);
        }
        if (pressKeyOnce(window, GLFW_KEY_R)) {
            camera.toggleFreeCam();
        }
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            model.processKeyboard(MovementDirections::FORWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            model.processKeyboard(MovementDirections::BACKWARD, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            model.processKeyboard(MovementDirections::LEFT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            model.processKeyboard(MovementDirections::RIGHT, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            model.processKeyboard(MovementDirections::UP, deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            model.processKeyboard(MovementDirections::DOWN, deltaTime);
        }
    }
    if (pressKeyOnce(window, GLFW_KEY_1)) {
        if (currentMode == RenderMode::VERTEXCOLOUR) {
            switchRendermode(RenderMode::TEXTURE);
        }
        else {
            switchRendermode(RenderMode::VERTEXCOLOUR);
        }
    }
    if (pressKeyOnce(window, GLFW_KEY_2)) {
        switchRendermode(RenderMode::SINGLECOLOUR);
    }
    if (pressKeyOnce(window, GLFW_KEY_3)) {
        renderWireframe = !renderWireframe;
        if (renderWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    if (pressKeyOnce(window, GLFW_KEY_T)) {
        // cameraMoveable = !cameraMoveable;
        camera.toggleCameraActive();
        if (camera.getCameraActive()) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        }
    }
}

/* make sure viewport matches window dimensions after rezising */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    SCR_WIDTH = width;
    SCR_HEIGHT = height;

    glViewport(0, 0, width, height);
}

/* whenever mouse moves this callback is called */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    /* prevents camera from jumping around when mouse enters the window from the side */
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (camera.getCameraActive()) {
        camera.processMouseMovement(xoffset, yoffset);
    }
    else {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            model.processMouseMovement(xoffset, yoffset, camera.getCameraVectors().up, camera.getCameraVectors().right);
        }
    }
}

/* whenever scroll wheel scrolls this callback is called */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;

    if (camera.getCameraActive()) {
        camera.processMouseScroll(yoffset);
    }
    else {
        model.processMouseScroll(yoffset);
    }
}

void error_callback(int error, const char* description) {
    if (description) {
        std::cerr << "GLFW error 0x" << error << ": " << description << std::endl;
    }
    else {
        std::cerr << "GLFW error 0x" << error << ": unknown" << std::endl;
    }
}

// void checkGlErrors(const char* where) {
//     for (GLenum error; (error = glGetError()) != GL_NO_ERROR; ) {
//         std::cerr << "OpenGl error 0x" << error << " at " << where << std::endl;
//     }
// }
