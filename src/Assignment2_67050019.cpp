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
GLuint characterTexture = 0;
GLuint aiTexture = 0;
GLuint roomTexture = 0;

const std::filesystem::path shaderDirectory =
    std::filesystem::u8path(OPENGL_STARTER_SHADER_DIR);

glm::vec3 cameraPos = glm::vec3(1.0f, 0.1f, 1.2f);
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
glm::vec3 lightPos = glm::vec3(0.8f, 1.50f, -2.0f);
glm::vec3 lightColour = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 bgColour = glm::vec3(0.82f, 0.92f, 0.98f);

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
        cameraPos = glm::vec3(1.0f, 0.1f, 1.2f);
        yaw = -90.0f;
        pitch = 0.0f;
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

bool LoadTexture(const char *fileLoc, GLuint &texture)
{
    int width = 0;
    int height = 0;
    int channels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(fileLoc, &width, &height, &channels, 0);
    if (!data)
    {
        std::cerr << "Failed to load character texture: " << fileLoc << '\n';
        return false;
    }

    const GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return true;
}

void CreateOBJ()
{
    // Character (Index 0)
    Mesh *characterMesh = new Mesh();
    if (characterMesh->CreateMeshFromOBJ("Models/char.obj"))
    {
        meshList.push_back(characterMesh);
    }
    else
    {
        delete characterMesh;
        return;
    }

    // Kneeling AI workers and their shared console (Index 1)
    Mesh *aiWorkersMesh = new Mesh();
    if (aiWorkersMesh->CreateMeshFromOBJ("Models/ai_kneeling_workers.obj"))
    {
        meshList.push_back(aiWorkersMesh);
    }
    else
    {
        delete aiWorkersMesh;
        return;
    }

    // Cube mesh used for ceiling light fixtures (Index 2)
    Mesh *cubeMesh = new Mesh();
    if (cubeMesh->CreateMeshFromOBJ("Models/cube.obj"))
    {
        meshList.push_back(cubeMesh);
    }
    else
    {
        delete cubeMesh;
    }

    // Open-front cube room (Index 3)
    Mesh *roomMesh = new Mesh();
    if (roomMesh->CreateMeshFromOBJ("Models/scene_room.obj"))
    {
        meshList.push_back(roomMesh);
    }
    else
    {
        delete roomMesh;
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
    if (meshList.size() < 2)
        return;

    GLint currentShader = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
    const GLint textureLocation = glGetUniformLocation(currentShader, "texture1");
    const GLint useTextureLocation = glGetUniformLocation(currentShader, "useTexture");

    for (size_t i = 0; i < 2; ++i)
    {
        glm::mat4 model(1.0f);
        GLuint texture = characterTexture;
        if (i == 0)
        {
            model = glm::translate(model, glm::vec3(2.5f, -1.50f, -2.70f));
            model = glm::scale(model, glm::vec3(0.65f));
        }
        else
        {
            texture = aiTexture;
            model = glm::translate(model, glm::vec3(0.0f, -1.50f, -2.70f));
            model = glm::scale(model, glm::vec3(0.72f));
        }

        glUniformMatrix4fv(glGetUniformLocation(currentShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(currentShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(currentShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        if (textureLocation >= 0 && useTextureLocation >= 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glUniform1i(textureLocation, 0);
            glUniform1i(useTextureLocation, GL_TRUE);
        }

        meshList[i]->RenderMesh();
    }
}

int main()
{
    Window mainWindow;
    if (mainWindow.initialise() != 0)
        return 1;

    GLFWwindow *window = glfwGetCurrentContext();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);

    CreateOBJ();
    CreateShaders();
    if (!LoadTexture("Textures/char_texture.png", characterTexture) ||
        !LoadTexture("Textures/ai_kneeling_workers.png", aiTexture) ||
        !LoadTexture("Textures/scene_room.bmp", roomTexture))
        return 1;

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
        lightPos = glm::vec3(0.8f, 1.50f, -2.0f);

        // --------------------------------------------------------------------
        // FIRST PASS: Render Depth Map (Shadow Map)
        // --------------------------------------------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightProjection = glm::ortho(-3.5f, 3.5f, -1.8f, 1.8f, 0.1f, 5.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.8f, -1.2f, -2.0f), glm::vec3(0.0f, 0.0f, -1.0f));

        shaderList[3]->UseShader(); // depthShader
        glUniformMatrix4fv(shaderList[3]->GetUniformLocation("lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        glUniformMatrix4fv(shaderList[3]->GetUniformLocation("lightView"), 1, GL_FALSE, glm::value_ptr(lightView));

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        RenderScene(lightView, lightProjection);
        glCullFace(GL_BACK);
        glDisable(GL_CULL_FACE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // --------------------------------------------------------------------
        // SECOND PASS: Render Main Scene, Background & Light Bulb
        // --------------------------------------------------------------------
        glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(
            glm::radians(65.0f),
            (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(),
            0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // 1. Render Character and kneeling AI
        shaderList[0]->UseShader(); // mainShader
        glUniform3fv(shaderList[0]->GetUniformLocation("lightColour"), 1, glm::value_ptr(lightColour));
        glUniform3fv(shaderList[0]->GetUniformLocation("lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(shaderList[0]->GetUniformLocation("viewPos"), 1, glm::value_ptr(cameraPos));
        glUniformMatrix4fv(shaderList[0]->GetUniformLocation("lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        glUniformMatrix4fv(shaderList[0]->GetUniformLocation("lightView"), 1, GL_FALSE, glm::value_ptr(lightView));
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(shaderList[0]->GetUniformLocation("shadowMap"), 1);

        RenderScene(view, projection);

        // 2. Render Office Scene Room (Walls, Furniture, Window, Whiteboard, Props)
        if (meshList.size() > 3)
        {
            glm::mat4 roomModel(1.0f);
            glUniformMatrix4fv(shaderList[0]->GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(roomModel));
            glUniformMatrix4fv(shaderList[0]->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(shaderList[0]->GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, roomTexture);
            glUniform1i(shaderList[0]->GetUniformLocation("texture1"), 0);
            glUniform1i(shaderList[0]->GetUniformLocation("useTexture"), GL_TRUE);

            meshList[3]->RenderMesh();
        }

        // 3. Render three ceiling light fixtures.
        shaderList[1]->UseShader(); // lightShader
        glUniformMatrix4fv(shaderList[1]->GetUniformLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(shaderList[1]->GetUniformLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(shaderList[1]->GetUniformLocation("lightColour"), 1.0f, 1.0f, 1.0f);
        if (meshList.size() > 2)
        {
            for (float x : {-1.5f, 0.8f, 3.1f})
            {
                glm::mat4 lightModel(1.0f);
                lightModel = glm::translate(lightModel, glm::vec3(x, 1.66f, -2.0f));
                lightModel = glm::scale(lightModel, glm::vec3(1.4f, 0.06f, 0.42f));
                glUniformMatrix4fv(shaderList[1]->GetUniformLocation("model"), 1, GL_FALSE, glm::value_ptr(lightModel));
                meshList[2]->RenderMesh();
            }
        }

        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}