#include <GL/glew.h>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Model.h"
#include <filesystem>

const GLuint WIDTH = 1920, HEIGHT = 1080;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void scrool_callback(GLFWwindow* window, double xoffset, double yoffset);
void pos_update();

using namespace glm;

Camera camera(vec3(0.0f, 0.0f, 3.0f));

bool keys[1024];
GLfloat currentFrame = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat deltaFrame = 0.0f;

GLfloat lastX = WIDTH/2.0;
GLfloat lastY = HEIGHT/2.0;

bool initMouse = true;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Hengine", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scrool_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glViewport(0, 0, WIDTH, HEIGHT);

    Shader myShader("/home/herain/Documents/opengl/model/model.vertex", "/home/herain/Documents/opengl/model/model.fragment");
    Shader singleShader("/home/herain/Documents/opengl/model/singleColor.vertex", "/home/herain/Documents/opengl/model/singleColor.fragment");;

    Model ourModel("/home/herain/Documents/opengl/model/obj/nanosuit.obj");

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    vec3 lightPos(1.2f, 1.0f, 2.0f);

    while (!glfwWindowShouldClose(window)){
        currentFrame = glfwGetTime();
        deltaFrame = currentFrame - lastFrame;
        lastFrame = currentFrame;

        pos_update();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        myShader.use();

        myShader.setVec3("viewPos", camera.Position);
        myShader.setVec3("light.position", lightPos);
        myShader.setVec3("light.ambient", vec3(0.2f));
        myShader.setVec3("light.diffuse", vec3(0.5f));
        myShader.setVec3("light.specular", vec3(1.0f));
        myShader.setFloat("shininess", 32);

        mat4 projection = perspective(radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        mat4 view = camera.GetViewMatrix();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);

        mat4 model = mat4(1.0f);
        model = translate(model, vec3(0.0, 0.0, 0.0));
        model = scale(model, vec3(1.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);
        ourModel.Draw(myShader);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);

        singleShader.use();

        mat4 model1 = mat4(1.0f);
        model1 = translate(model, vec3(0.0, 0.0, 0.0));
        model1 = scale(model, vec3(1.01f, 1.01f, 1.01f));
        singleShader.setMat4("model", model1);
        singleShader.setMat4("projection", projection);
        singleShader.setMat4("view", view);
        ourModel.Draw(singleShader);

        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilMask(0xFF);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode){

    if (key == GLFW_KEY_ESCAPE && mode == GLFW_MOD_SHIFT && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if(action == GLFW_PRESS){
        keys[key] = true;
    }  else if (action == GLFW_RELEASE){
        keys[key] = false;
    }
}

void pos_update(){
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaFrame);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaFrame);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaFrame);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaFrame);
    if(keys[GLFW_KEY_SPACE])
        camera.ProcessKeyboard(UP, deltaFrame);
    if(keys[GLFW_KEY_LEFT_SHIFT])
        camera.ProcessKeyboard(DOWN, deltaFrame);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(initMouse){
        lastX = xpos;
        lastY = ypos;
        initMouse = false;
    }

    GLfloat xOffset = xpos - lastX;
    GLfloat yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    GLfloat h_sensitivity = 0.1f;
    GLfloat v_sensitivity = 0.1f;
    xOffset *= v_sensitivity;
    yOffset *= h_sensitivity;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

void scrool_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.ProcessMouseScroll(yoffset);
}