#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "camera.hpp"
#include "ObjModel.hpp"
#include "mat4.hpp"
#include "stb_image.h"

#include <iostream>
#include <unordered_map>
// #include <cmath>

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
unsigned int texture1;

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

    /*  SHADER SECTION  */
    Shader customShader("shaders/vertex.vert", "shaders/fragment.frag");

    ObjModel model;
    const bool modelLoaded = (argc > 1) && model.load(argv[1]);

    if (!modelLoaded) {
        std::cout << "No model provided, using default cube." << std::endl;
        /* default cube in case no object was loaded */
        float vertices[] = {
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     0.0f, 1.0f, 1.0f,

             0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f, 
             0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     1.0f, 0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,

            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 0.0f, 0.0f,

            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,    1.0f, 1.0f,     1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,    1.0f, 0.0f,     1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,     0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,     0.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        /* copy vertex array to buffer for opengl to use */
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        /* set the vertex attribute pointer */
        /* position */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        /* texture coordinate */
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);

        /* colour */
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    /* texture */
    // unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    /* set texture wrapping parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* set texture filtering parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* load image, create texture and generate mipmaps */
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("textures/flowers.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    /* uncomment to draw in wireframe mode */
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /*  SHADER SECTION END  */

    /* render loop */
    while (!glfwWindowShouldClose(window)) {
        camera.printState();
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* input */
        processInput(window);

        /* render */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* bind texture */
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);

        /* activate the shader */
        customShader.use();

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
        Mat4 projection = Mat4::identityMatrix();
        projection = perspective(camera.Zoom * 3.14159265358979323846f / 180.0f, (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        customShader.setMat4("projection", projection);

        /* camera / view transformations */
        Mat4 view = Mat4::identityMatrix();
        view = lookAt(camera.Position, camera.Position + camera.Front, camera.Up);
        customShader.setMat4("view", view);

        Mat4 modelM = Mat4::identityMatrix();
        modelM = translate(modelM, {0.0f, 0.0f, 0.0f});
        customShader.setMat4("model", modelM);

        if (modelLoaded) {
            model.draw();
        }
        else {
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        /* swap buffers */
        glfwSwapBuffers(window);
        glfwPollEvents();

        camera.Update(deltaTime);
    }

    /* optional: de allocate all resources once theyve outlived their purpose: */
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);

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
    // if (pressKeyOnce(window, GLFW_KEY_Q)) {
    //     camera.printState();
    // }
}

/* make sure viewport matches window dimensions after rezising */
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window; //why does function ask for window but not use it?
    glViewport(0, 0, width, height);
}

/* whenever mouse moves this callback is called */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    (void)window; //why does function ask for window but not use it?
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
    (void)window; //why does function ask for window but not use it?
    (void)xoffset;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

