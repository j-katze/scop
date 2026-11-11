#include "gl_functions.hpp"
#include "gl_errors.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "object.hpp"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <unordered_map>
#include <cstring>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void error_callback(int error, const char* description);

void processInput(GLFWwindow *window);

enum class RenderMode {
    SINGLECOLOUR,
    VERTEXCOLOUR,
    TEXTURE
};

struct AppContext {
    Object*     model        = nullptr;
    Camera*     camera       = nullptr;

    /* initial window dimensions */
    int         screenWidth  = 800;
    int         screenHeight = 600;

    /* prevent sudden camera movement when first entering window */
    float       lastX        = screenWidth / 2.0f;
    float       lastY        = screenHeight / 2.0f;
    bool        firstMouse   = true;

    /* timing */
    float       deltaTime    = 0.0f;
    float       lastFrame    = 0.0f;

    /* model display */
    RenderMode  currentMode  = RenderMode::VERTEXCOLOUR;
    RenderMode  nextMode     = RenderMode::VERTEXCOLOUR;
    float       blendAmount  = 1.0f;

    bool renderWireframe     = false;
};

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

/* glfw error handling */
bool glfwErrorOccured = false;

int main(int argc, char* argv[]) {
    try {
        glfwSetErrorCallback(error_callback);
        // glfwInitHint(GLFW_WAYLAND_LIBDECOR, GLFW_WAYLAND_DISABLE_LIBDECOR);
        /* initialize */
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return EXIT_FAILURE;
        }
        GlfwGuard glfwGuard;

        AppContext context;

        /* use OpenGl 3.3 core profile (core profile excludes older and deprecated features) */
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        if (glfwErrorOccured) {
            return EXIT_FAILURE;
        }

        /* create window */
        WindowGuard w{glfwCreateWindow(context.screenWidth, context.screenHeight, "scop", NULL, NULL)};
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

        /* load OpenGL function pointers */
        if (!gl.load()) {
            std::cerr << "Failed to load required OpenGL functions" << std::endl;
            return EXIT_FAILURE;
        }

        glfwGetFramebufferSize(w.window, &context.screenWidth, &context.screenHeight);
        if (glfwErrorOccured) {
            return EXIT_FAILURE;
        }
        gl.Viewport(0, 0, context.screenWidth, context.screenHeight);

        glfwSetInputMode(w.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        if (glfwErrorOccured) {
            return EXIT_FAILURE;
        }

        gl.Enable(GL_DEPTH_TEST);

        if (reportGlErrors("initial setup")) {
            return EXIT_FAILURE;
        }

        /*loading shader */
        Shader shader("shaders/vertex.vert", "shaders/fragment.frag");
        if (reportGlErrors("shader creation")) {
            return EXIT_FAILURE;
        }

        /* loading model and texture */
        Object model;

        bool modelLoaded = argc > 1 && model.loadModel(argv[1]);
        if (modelLoaded && std::strcmp(argv[1], "models/42.obj") == 0) {
            model.toggleAutoRotate();
        }
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

        Camera camera({0.0f, 0.0f, 10.0f});
        context.model = &model;
        context.camera = &camera;
        glfwSetWindowUserPointer(w.window, &context);

        /* render loop */
        while (!glfwWindowShouldClose(w.window)) {
            /* get deltatime */
            float currentFrame = static_cast<float>(glfwGetTime());
            context.deltaTime = currentFrame - context.lastFrame;
            context.lastFrame = currentFrame;

            /* process input */
            glfwPollEvents();
            processInput(w.window);
            if (reportGlErrors("input and callbacks")) {
                return EXIT_FAILURE;
            }
            if (glfwWindowShouldClose(w.window)) {
                break;
            }

            /* clear buffers */
            gl.ClearColor(0.30f, 0.25f, 0.42f, 1.0f);
            gl.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            /* activate the shader */
            shader.use();

            /* blend between render modes if mode was switched */
            if (context.blendAmount < 1.0f) {
                context.blendAmount = smoothlerp(context.blendAmount, 1.0f, 6.0f, context.deltaTime);
                if (std::abs(context.blendAmount - 1.0f) < 0.02f) {
                    context.blendAmount = 1.0f;
                    context.currentMode = context.nextMode;
                }
            }

            shader.setInt("currentMode", static_cast<int>(context.currentMode));
            shader.setInt("nextMode", static_cast<int>(context.nextMode));
            shader.setFloat("blendAmount", context.blendAmount);

            camera.update(shader, context.deltaTime, (float)context.screenWidth, (float)context.screenHeight);
            model.update(shader, context.deltaTime);

            glfwSwapBuffers(w.window);
            if (glfwErrorOccured || reportGlErrors("rendering")) {
                return EXIT_FAILURE;
            }
        }
        return EXIT_SUCCESS;
    }
    catch (const std::bad_alloc&) {
        std::cerr << "Out of memory; shutting down." << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

void switchRendermode(AppContext* context, RenderMode newMode) {

    if (context->blendAmount >= 1.0f && newMode != context->currentMode) {
        context->nextMode = newMode;
        context->blendAmount = 0.0f;
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
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (context->camera->getCameraActive()) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            context->camera->processKeyboard(MovementDirections::FORWARD, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            context->camera->processKeyboard(MovementDirections::BACKWARD, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            context->camera->processKeyboard(MovementDirections::LEFT, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            context->camera->processKeyboard(MovementDirections::RIGHT, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            context->camera->processKeyboard(MovementDirections::UP, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            context->camera->processKeyboard(MovementDirections::DOWN, context->deltaTime);
        }
        if (pressKeyOnce(window, GLFW_KEY_R)) {
            context->camera->toggleFreeCam();
        }
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            context->model->processKeyboard(MovementDirections::FORWARD, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            context->model->processKeyboard(MovementDirections::BACKWARD, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            context->model->processKeyboard(MovementDirections::LEFT, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            context->model->processKeyboard(MovementDirections::RIGHT, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            context->model->processKeyboard(MovementDirections::UP, context->deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            context->model->processKeyboard(MovementDirections::DOWN, context->deltaTime);
        }
    }
    if (pressKeyOnce(window, GLFW_KEY_1)) {
        if (context->currentMode == RenderMode::VERTEXCOLOUR) {
            switchRendermode(context, RenderMode::TEXTURE);
        }
        else {
            switchRendermode(context, RenderMode::VERTEXCOLOUR);
        }
    }
    if (pressKeyOnce(window, GLFW_KEY_2)) {
        switchRendermode(context, RenderMode::SINGLECOLOUR);
    }
    if (pressKeyOnce(window, GLFW_KEY_3)) {
        context->renderWireframe = !context->renderWireframe;
        if (context->renderWireframe) {
            gl.PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            gl.PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    if (pressKeyOnce(window, GLFW_KEY_4)) {
        context->model->toggleAutoRotate();
    }

    if (pressKeyOnce(window, GLFW_KEY_T)) {
        context->camera->toggleCameraActive();
        if (context->camera->getCameraActive()) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

/* make sure viewport matches window dimensions after rezising */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));

    context->screenWidth = width;
    context->screenHeight = height;

    gl.Viewport(0, 0, width, height);
}

/* whenever mouse moves this callback is called */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    /* prevents camera from jumping around when mouse enters the window from the side */
    if (context->firstMouse) {
        context->lastX = xpos;
        context->lastY = ypos;
        context->firstMouse = false;
    }

    float xoffset = xpos - context->lastX;
    float yoffset = context->lastY - ypos;

    context->lastX = xpos;
    context->lastY = ypos;

    if (context->camera->getCameraActive()) {
        context->camera->processMouseMovement(xoffset, yoffset);
    }
    else {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            context->model->processMouseMovement(xoffset, yoffset, context->camera->getCameraVectors().up,
                                                 context->camera->getCameraVectors().right);
        }
    }
}

/* whenever scroll wheel scrolls this callback is called */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset;

    AppContext* context = static_cast<AppContext*>(glfwGetWindowUserPointer(window));

    if (context->camera->getCameraActive()) {
        context->camera->processMouseScroll(yoffset);
    }
    else {
        context->model->processMouseScroll(yoffset);
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
