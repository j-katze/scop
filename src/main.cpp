#include "glad.h"
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"
#include "object.hpp"
#include "gl_errors.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <new>
#include <unordered_map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void error_callback(int error, const char* description);
void processInput(GLFWwindow *window);

void clearGlErrors();

bool glfwErrorOccured = false;

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

struct GlfwGuard {
    ~GlfwGuard() {
        glfwTerminate();
    }
};

struct WindowGuard {
    GLFWwindow* window = nullptr;
    explicit WindowGuard(GLFWwindow* w) : window(w) {}
    ~WindowGuard() {
        if (window) glfwDestroyWindow(window);
    }
};

bool renderWireframe = false;

int main(int argc, char* argv[]) {
    try {
        glfwSetErrorCallback(error_callback);
        /* initialize */
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return EXIT_FAILURE;
        }
        GlfwGuard glfwGuard;

        /* use OpenGl 3.3 core profile (core profile excludes older and deprecated features) */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        if (glfwErrorOccured) {
            return EXIT_FAILURE;
        }

        /* create window */
        // GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "scop", NULL, NULL);
        WindowGuard w{glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "scop", NULL, NULL)};
        if (w.window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            return EXIT_FAILURE;
        }
        glfwMakeContextCurrent(w.window);
        if (glfwErrorOccured) {
            return EXIT_FAILURE;
        }

        /* set up event callbacks */
        glfwSetFramebufferSizeCallback(w.window, framebuffer_size_callback);
        glfwSetCursorPosCallback(w.window, mouse_callback);
        glfwSetScrollCallback(w.window, scroll_callback);
        if (glfwErrorOccured) {
            return EXIT_FAILURE;
        }

        /* load OpenGL function pointers via GLAD */
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to load OpenGL functions with GLAD\n";
            return EXIT_FAILURE;
        }

        glfwGetFramebufferSize(w.window, &SCR_WIDTH, &SCR_HEIGHT);
        if (glfwErrorOccured) {
            return EXIT_FAILURE;
        }
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glfwSetInputMode(w.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwErrorOccured) {
            return EXIT_FAILURE;
        }

        glEnable(GL_DEPTH_TEST);

        if (reportGlErrors("initial setup")) {
            return EXIT_FAILURE;
        }

        /*loading shader */
        Shader shader("shaders/vertex.vert", "shaders/fragment.frag");
        if (reportGlErrors("shader creation")) {
            return EXIT_FAILURE;
        }

        /* loading model and texture */
        bool modelLoaded = argc > 1 && model.loadModel(argv[1]);
        if (!modelLoaded && !model.loadDefaultCube()) {
            return EXIT_FAILURE;
        }

        bool textureLoaded = argc > 2 && model.loadTexture(argv[2]);
        if (!textureLoaded) {
            std::cout << "Attempting to loading cat.bmp" << std::endl;
            textureLoaded = model.loadTexture("textures/cat.bmp");
        }
        if (!textureLoaded && !model.loadDefaultTexture()) {
            return EXIT_FAILURE;
        }

        /* render loop */
        while (!glfwWindowShouldClose(w.window)) {
            /* get deltatime */
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            /* process input */
            processInput(w.window);

            /* clear buffers */
            glClearColor(0.30f, 0.25f, 0.42f, 1.0f);
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

            camera.update(shader, deltaTime, (float)SCR_WIDTH, (float)SCR_HEIGHT);
            model.update(shader, deltaTime);

            glfwSwapBuffers(w.window);
            glfwPollEvents();
            if (glfwErrorOccured) {
                return EXIT_FAILURE;
            }
        }
        shader.destroy();
        model.destroy();
        return EXIT_SUCCESS;

    } catch (const std::bad_alloc&) {
        std::cerr << "Out of memory; shutting down." << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
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
        camera.toggleCameraActive();
        if (camera.getCameraActive()) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
    glfwErrorOccured = true;
    if (description) {
        std::cerr << "GLFW error 0x" << error << ": " << description << std::endl;
    }
    else {
        std::cerr << "GLFW error 0x" << error << ": unknown" << std::endl;
    }
}

void clearGlErrors() {
    while (glGetError() != GL_NO_ERROR) {}
}
