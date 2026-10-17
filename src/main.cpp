#undef GLFW_DLL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <vector>
#include <iostream>
#include "Libs/Mesh.h"
#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "ProjectPaths.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const GLint WIDTH = 800;
const GLint HEIGHT = 600;

std::vector<Mesh *> meshList;
std::vector<Shader *> shaderList; // [0] mainShader, [1] lightShader, [2] bgShader, [3] depthShader
GLuint textureID;

const std::filesystem::path shaderDirectory =
    std::filesystem::u8path(OPENGL_STARTER_SHADER_DIR);

glm::vec3 pyramidPositions[] = {
    glm::vec3(0.0f, 0.0f, -2.5f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, 2.2f, -2.5f),
    glm::vec3(-3.8f, 2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, 2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f)};

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ตำแหน่งแสงและสีแสง
glm::vec3 lightPos = glm::vec3(5.0f, 15.0f, 5.0f);
glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 bgColour = glm::vec3(1.0f, 0.5f, 0.5f);

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(5.0f * deltaTime);
    if (cameraSpeed <= 0.001f)
        cameraSpeed = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
        yaw = -90.0f;
        pitch = 0.0f;
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

void LoadTexture(const char *fileLoc)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(fileLoc, &width, &height, &nrChannels, 0);

    if (data)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        std::cout << "Successfully loaded texture: " << fileLoc << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture: " << fileLoc << std::endl;
        stbi_image_free(data);
    }
}

void CreateOBJ()
{
    // 1. Suzanne Models (Index 0 - 9)
    Mesh *obj1 = new Mesh();
    bool loaded1 = obj1->CreateMeshFromOBJ("Models/suzanne.obj");
    if (loaded1)
    {
        for (int i = 0; i < 10; i++)
        {
            meshList.push_back(obj1);
        }
    }

    // 2. Background / Light Bulb Cube (Index 10)
    Mesh *cubeMesh = new Mesh();
    bool loaded2 = cubeMesh->CreateMeshFromOBJ("Models/cube.obj");
    if (loaded2)
    {
        meshList.push_back(cubeMesh);
    }
}

void CreateShaders()
{
    // [0] Main Shader
    Shader *shader1 = new Shader();
    shader1->CreateFromFiles((shaderDirectory / "shader.vert").string().c_str(), 
                             (shaderDirectory / "shader.frag").string().c_str());
    shaderList.push_back(shader1);

    // [1] Light Source Shader
    Shader *shader2 = new Shader();
    shader2->CreateFromFiles((shaderDirectory / "lightShader.vert").string().c_str(), 
                             (shaderDirectory / "lightShader.frag").string().c_str());
    shaderList.push_back(shader2);

    // [2] Background Shader
    Shader *bgShader = new Shader();
    bgShader->CreateFromFiles((shaderDirectory / "bgShader.vert").string().c_str(), 
                            (shaderDirectory / "bgShader.frag").string().c_str());
    shaderList.push_back(bgShader);

    // [3] Depth Shader
    Shader *depthShader = new Shader();
    depthShader->CreateFromFiles((shaderDirectory / "depthShader.vert").string().c_str(), 
                                (shaderDirectory / "depthShader.frag").string().c_str());
    shaderList.push_back(depthShader);
}

void RenderScene(glm::mat4 view, glm::mat4 projection)
{
    for (int i = 0; i < 10; i++)
    {
        glm::mat4 model(1.0f);
        model = glm::translate(model, pyramidPositions[i]);
        model = glm::rotate(model, glm::radians(2.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
        model = glm::scale(model, glm::vec3(0.8f, 0.8f, 1.0f));

        GLuint currentShader = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&currentShader);

        glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(currentShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(currentShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        meshList[i]->RenderMesh();
    }
}

int main()
{
    Window mainWindow;
    mainWindow.initialise();
    GLFWwindow *window = glfwGetCurrentContext();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    CreateOBJ();
    CreateShaders();
    LoadTexture("Textures/uvmap.png");

    // ------------------------------------------------------------------------
    // Setup Shadow Framebuffer & Depth Map Texture
    // ------------------------------------------------------------------------
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0,GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!mainWindow.getShouldClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        processInput(window);

        // --------------------------------------------------------------------
        // คำนวณตำแหน่งแสงหมุนเป็นวงกลมในแต่ละเฟรม (Dynamic Light Position)
        // --------------------------------------------------------------------
        float lightRadius = 12.0f;
        float lightSpeed = 0.3f;
        lightPos.x = sin(currentFrame * lightSpeed) * lightRadius;
        lightPos.z = cos(currentFrame * lightSpeed) * lightRadius - 5.0f;
        lightPos.y = 10.0f + sin(currentFrame * 0.5f) * 2.0f;

        // --------------------------------------------------------------------
        // FIRST PASS: Render Depth Map (Shadow Map)
        // --------------------------------------------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 50.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        shaderList[3]->UseShader(); // depthShader

        glCullFace(GL_FRONT);
        RenderScene(lightView, lightProjection);
        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // --------------------------------------------------------------------
        // SECOND PASS: Render Main Scene, Background & Light Bulb
        // --------------------------------------------------------------------
        glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(),
            0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // 1. Render Objects (Suzanne)
        shaderList[0]->UseShader(); // mainShader
        glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, glm::value_ptr(lightColour));
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, glm::value_ptr(cameraPos));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(shaderList[0]->GetUniformLocation("texture1"), 0);
        glUniform1i(shaderList[0]->GetUniformLocation("useTexture"), GL_TRUE);

        RenderScene(view, projection);

        // 2. Render Background Plane
        shaderList[2]->UseShader(); // bgShader
        glUniformMatrix4fv(shaderList[2]->GetUniformLocation("lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        glUniformMatrix4fv(shaderList[2]->GetUniformLocation("lightView"), 1, GL_FALSE, glm::value_ptr(lightView));

        glm::mat4 bgModel = glm::mat4(1.0f);
        bgModel = glm::translate(bgModel, glm::vec3(0.0f, 0.0f, -8.0f));
        bgModel = glm::scale(bgModel, glm::vec3(10.0f, 10.0f, 0.1f));

        glUniformMatrix4fv(shaderList[2]->GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(bgModel));
        glUniformMatrix4fv(shaderList[2]->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shaderList[2]->GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(shaderList[2]->GetUniformLocation("bgColour"), 1, glm::value_ptr(bgColour));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(shaderList[2]->GetUniformLocation("shadowMap"), 0);

        if (meshList.size() > 10)
        {
            meshList[10]->RenderMesh();
        }

        // 3. Render Light Source Bulb (แสดงเป็นก้อนหลอดไฟสว่างตรงตำแหน่งแสง)
        shaderList[1]->UseShader(); // lightShader
        glm::mat4 lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.3f));

        glUniformMatrix4fv(shaderList[1]->GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(lightModel));
        glUniformMatrix4fv(shaderList[1]->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shaderList[1]->GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(shaderList[1]->GetUniformLocation("lightColour"), 1, glm::value_ptr(lightColour));

        if (meshList.size() > 10)
        {
            meshList[10]->RenderMesh();
        }

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}