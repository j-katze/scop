#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"
#include "ObjModel.hpp"
#include "BmpImage.hpp"
#include "mat4.hpp"

#include <iostream>
#include <unordered_map>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

/* settings */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/* camera */
//   Y      +Y up   -Y down
//   |
//   +--X   +X right -X left
//  /
// Z        +Z back -Z front

//             X     Y     Z
Camera camera({0.0f, 0.0f, 10.0f});
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

/* timing */
float deltaTime = 0.0f;
float lastFrame = 0.0f;

unsigned int VAO, VBO;

enum RenderMode {
    SINGLECOLOUR,
    VERTEXCOLOUR,
    TEXTURE
};

RenderMode currentMode = VERTEXCOLOUR;
RenderMode nextMode = VERTEXCOLOUR;
float blendAmount = 1.0f;
const float transitionDuration = 1.0f;

bool renderWireframe = false;

int main(int argc, char* argv[]) {
    /* initialize */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* create window */
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "scop", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* load OpenGL function pointers via GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load OpenGL functions with GLAD\n";
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    /*loading shader */
    Shader customShader("shaders/vertex.vert", "shaders/fragment.frag");

    /* loading model */
    ObjModel model;
    const bool modelLoaded = (argc > 1) && model.load(argv[1]);

    if (!modelLoaded) {
        model.defaultCube();
    }

    /* loading texture */
    BmpImage texture;
    const bool textureLoaded = (argc > 2) && texture.loadTexture(argv[2]);

    if (!textureLoaded) {
        return 1;
    }

    /* render loop */
    while (!glfwWindowShouldClose(window)) {
        /* get deltatime */
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* process input */
        processInput(window);
        camera.printState();
        camera.Update(deltaTime);

        /* clear buffers */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* bind texture */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.textureID);

        /* activate the shader */
        customShader.use();

        /* blend between render modes if mode was switched */
        if (blendAmount < 1.0f) {
            blendAmount += deltaTime * transitionDuration;
            if (blendAmount >= 1.0f) {
                blendAmount = 1.0f;
                currentMode = nextMode;
            }
        }

        customShader.setInt("currentMode", static_cast<int>(currentMode));
        customShader.setInt("nextMode", static_cast<int>(nextMode));
        customShader.setFloat("blendAmount", blendAmount);

        if (renderWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        /* pass projection matrix to shader */
        Mat4 projectionMat = Mat4::identityMatrix();
        projectionMat = perspective(camera.Zoom * 3.14159265358979323846f / 180.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        customShader.setMat4("projection", projectionMat);

        /* camera / view transformations */
        Mat4 viewMat = Mat4::identityMatrix();
        viewMat = lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        customShader.setMat4("view", viewMat);

        Mat4 modelMat = Mat4::identityMatrix();
        modelMat = translate(modelMat, model.centerVector);
        customShader.setMat4("model", modelMat);

        model.draw();

        /* swap buffers */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    /* optional: de allocate all resources once theyve outlived their purpose: */
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    /* terminate program, clearing all allocated glfw resources */
    glfwTerminate();
    return 0;
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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (pressKeyOnce(window, GLFW_KEY_R)) {
        camera.ToggleFreeCam();
    }
    if (pressKeyOnce(window, GLFW_KEY_1)) {
        if (currentMode == VERTEXCOLOUR) {
            switchRendermode(TEXTURE);
        }
        else {
            switchRendermode(VERTEXCOLOUR);
        }
    }
    if (pressKeyOnce(window, GLFW_KEY_2)) {
        switchRendermode(SINGLECOLOUR);
    }
    if (pressKeyOnce(window, GLFW_KEY_3)) {
        renderWireframe = !renderWireframe;
    }
}

/* make sure viewport matches window dimensions after rezising */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;

    glViewport(0, 0, width, height);
}

/* whenever mouse moves this callback is called */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    (void)window;
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

/* whenever scroll wheel scrolls this callback is called */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)window;
    (void)xoffset;

    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

