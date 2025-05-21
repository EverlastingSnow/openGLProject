#include "glm/trigonometric.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "camera.h"

#include <cmath>
#include <iostream>

// #include <vector>
// #include <random>

// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// settings
bool spin_rectangle = false;
bool showTriangle = false;
bool random_triangleColor = false;
bool showRectangle = false;
bool showCube = false;
bool multipleCubes = false;
bool spinCubeView = false;
bool freeMoveCube = false;
bool firstMouse = true;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float triangle_vertices[] = {
    // positions         // colors
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
};

float rectangle_vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
        0.3f,  0.3f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
        0.3f, -0.3f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
    -0.3f, -0.3f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
    -0.3f,  0.3f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};
float rectangle_vertices2[] = {
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
            0.3f,  0.3f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
            0.3f, -0.3f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.3f, -0.3f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.3f,  0.3f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };
unsigned int rectangle_indices[] = {  
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

float cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float fov   =  45.0f;

float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

ImVec4 triangleColor1 = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
ImVec4 triangleColor2 = ImVec4(0.0f, 1.0f, 0.0f, 0.0f);
ImVec4 triangleColor3 = ImVec4(0.0f, 0.0f, 1.0f, 0.0f);
float rectBeta = 0.2f;
float rectangleX = 0.0f;
float rectangleY = 0.0f;
float rectangleWidth = 1.0f;
float rectangleHeight = 1.0f;

void my_gui(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    {
        ImGui::Begin("Settings");

        // 三角形设置部分
        if (ImGui::CollapsingHeader("Triangle Settings")){
            ImGui::ColorEdit3("Triangle Color1", (float*)&triangleColor1);
            ImGui::ColorEdit3("Triangle Color2", (float*)&triangleColor2);
            ImGui::ColorEdit3("Triangle Color3", (float*)&triangleColor3);
            if (ImGui::Button("show Triangle")){
                showTriangle = !showTriangle;
            }
            if (showTriangle){
                if (ImGui::Button("randomize Color")){
                    random_triangleColor = !random_triangleColor;
                }
            }
        }

        // 矩形设置部分
        if (ImGui::CollapsingHeader("Rectangle Settings")){
            if (ImGui::Button("show Rectangle")){
                showRectangle = !showRectangle;
            }
            if (ImGui::Button("spin Rectangle")){
                spin_rectangle = !spin_rectangle;
            }
            if (ImGui::Button("reset Rectangle")){
                rectangleX = 0.0f;
                rectangleY = 0.0f;
                rectangleWidth = 1.0f;
                rectangleHeight = 1.0f;
                rectBeta = 0.2f;
            }
            ImGui::SliderFloat("Rectangle X", &rectangleX, -1.0f, 1.0f);
            ImGui::SliderFloat("Rectangle Y", &rectangleY, -1.0f, 1.0f);
            ImGui::SliderFloat("Rectangle Width", &rectangleWidth, 0.0f, 2.0f);
            ImGui::SliderFloat("Rectangle Height", &rectangleHeight, 0.0f, 2.0f);
            ImGui::SliderFloat("Rectangle Beta", &rectBeta, 0.0f, 1.0f);
        }
        if (ImGui::CollapsingHeader("Cube Settings")){
            if (ImGui::Button("show Cube")){
                showCube = !showCube;
            }
            if (ImGui::Button("multiple Cubes")){
                multipleCubes = !multipleCubes;
            }
            if (ImGui::Button("spin view")){
                spinCubeView = !spinCubeView;
            }
            if (ImGui::Button("free move")){
                freeMoveCube = !freeMoveCube;
            }
        }
        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
// glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
// glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
// glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
// glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);


/*
R:cameraRight
U:cameraUp
D:cameraDirection
P:cameraPos
LookAt Matrix:
| Rx Ry Rz 0 |    | 1 0 0 -Px |
| Ux Uy Uz 0 |    | 0 1 0 -Py |
| Dx Dy Dz 0 |  * | 0 0 1 -Pz |
|  0  0  0 1 |    | 0 0 0  1  |
*/
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "myOpenGLProj", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    //imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // 启用键盘导航

    // 设置平台和渲染器绑定
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // 设置样式
    ImGui::StyleColorsDark();

    // build and compile our shader program
    // ------------------------------------
    Shader triShader("src\\tri_shader.vs", "src\\tri_shader.fs"); // you can name your shader files however you like
    Shader rectShader("src\\rect_shader.vs", "src\\rect_shader.fs");
    Shader cubeShader("src\\cube_shader.vs", "src\\cube_shader.fs"); 

    unsigned int VBO_tri, VAO_tri;
    glGenVertexArrays(1, &VAO_tri);
    glGenBuffers(1, &VBO_tri);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO_tri);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_tri);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    unsigned int VBO_rect, VAO_rect, EBO_rect;
    glGenVertexArrays(1, &VAO_rect);
    glGenBuffers(1, &VBO_rect);
    glGenBuffers(1, &EBO_rect);
    glBindVertexArray(VAO_rect);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_rect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_rect);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangle_indices), rectangle_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int VBO_cube, VAO_cube;
    glGenVertexArrays(1, &VAO_cube);
    glGenBuffers(1, &VBO_cube);
    glBindVertexArray(VAO_cube);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load("src\\container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture container" << std::endl;
    }
    stbi_image_free(data);


    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("src\\laugh.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture laugh" << std::endl;
    }
    stbi_image_free(data);


    rectShader.use();
    rectShader.setFloat("Beta", rectBeta);
    rectShader.setInt("texture1", 0);
    rectShader.setInt("texture2", 1);

    cubeShader.use();
    cubeShader.setInt("texture1", 0);
    cubeShader.setInt("texture2", 1);


    // render loop
    // -----------
    float xoffset = 0.0f;
    float yoffset = 0.0f;
    float colorVal = 0.0f;
    float colorVal2 = 0.0f;
    float colorVal3 = 0.0f;

    int MAX_CUBE_NUM = 10;
    // std::vector<glm::vec3> cube_positions;
    // for (int i = 0;i < MAX_CUBE_NUM;++i){
    //     cube_positions.push_back(
    //         glm::vec3(0.1 * (rand() % 20), 0.1 * (rand() % 20), 0.1 * (rand() % 20))
    //     );
    //     // std::cout<< cube_positions[i].x << " " << cube_positions[i].y << " " << cube_positions[i].z << std::endl;
    // }

    glm::vec3 cube_positions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        my_gui();

        // render the cube
        if (showCube) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);

            cubeShader.use();

            // make sure to initialize matrix to identity matrix first
            glm::mat4 cube_view          = glm::mat4(1.0f);
            glm::mat4 cube_projection    = glm::mat4(1.0f);
            // cube_view  = glm::translate(cube_view, glm::vec3(0.0f, 0.0f, -3.0f));
            float camera_radius = 10.0f;
            float camera_X = sin(glfwGetTime()) * camera_radius;
            float camera_Z = cos(glfwGetTime()) * camera_radius;
            if (!spinCubeView){
                camera_X = 0.0f;
                camera_Z = -3.0f;
            }
            if (freeMoveCube){
                cube_view =
                    glm::lookAt(
                        cameraPos, cameraPos + cameraFront, cameraUp
                    );
            }else{
                cube_view =
                    glm::lookAt(
                        glm::vec3(camera_X, 0.0f, camera_Z),
                        glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f)
                    );
            }
            cube_projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

            unsigned int cube_viewLoc  = glGetUniformLocation(cubeShader.ID, "view");
            unsigned int cube_projectionLoc = glGetUniformLocation(cubeShader.ID, "projection");

            glUniformMatrix4fv(cube_viewLoc, 1, GL_FALSE, glm::value_ptr(cube_view));
            glUniformMatrix4fv(cube_projectionLoc, 1, GL_FALSE, glm::value_ptr(cube_projection));

            glBindVertexArray(VAO_cube);
            if (multipleCubes){
                MAX_CUBE_NUM = 10;
            }else MAX_CUBE_NUM = 1;
            for (int i = 0;i < MAX_CUBE_NUM;++i){
                glm::mat4 cube_model         = glm::mat4(1.0f);
                cube_model = glm::translate(cube_model, cube_positions[i]);
                float angle = 20.0f * i;
                cube_model = glm::rotate(cube_model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                unsigned int cube_modelLoc = glGetUniformLocation(cubeShader.ID, "model");
                glUniformMatrix4fv(cube_modelLoc, 1, GL_FALSE, glm::value_ptr(cube_model));
            
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
        }

        // render the rectangle
        if (showRectangle){
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture2);

            glm::mat4 trans = glm::mat4(1.0f);
            
            rectShader.use();
            rectShader.setFloat("Beta", rectBeta);

            if (spin_rectangle){
                rectBeta = sin(glfwGetTime()) / 2.0f + 0.5f;
                trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
                trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
            }
            rectangle_vertices2[0] = rectangle_vertices[0] + rectangleX;rectangle_vertices2[1] = rectangle_vertices[1] + rectangleY;
            rectangle_vertices2[8] = rectangle_vertices[8] + rectangleX;rectangle_vertices2[9] = rectangle_vertices[9] + rectangleY;
            rectangle_vertices2[16] = rectangle_vertices[16] + rectangleX;rectangle_vertices2[17] = rectangle_vertices[17] + rectangleY;
            rectangle_vertices2[24] = rectangle_vertices[24] + rectangleX;rectangle_vertices2[25] = rectangle_vertices[25] + rectangleY;
            glBindBuffer(GL_ARRAY_BUFFER, VBO_rect);
            glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices2), rectangle_vertices2, GL_STATIC_DRAW);
            trans = glm::scale(trans, glm::vec3(rectangleWidth, rectangleHeight, 0.0f));
            unsigned int transformLoc = glGetUniformLocation(rectShader.ID, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

            glBindVertexArray(VAO_rect);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        // render the triangle
        if (showTriangle) {
            xoffset = sin(glfwGetTime()) / 2.0;
            yoffset = cos(glfwGetTime()) / 2.0;

            colorVal = sin(glfwGetTime()) / 2.0f + 0.5f;
            colorVal2 = sin(glfwGetTime()+2.12) / 2.0f + 0.5f;
            colorVal3 = sin(glfwGetTime()+4.24) / 2.0f + 0.5f;

            if (random_triangleColor){
                triangle_vertices[3] = colorVal;
                triangle_vertices[9] = colorVal2;
                triangle_vertices[15] = colorVal3;
                triangle_vertices[4] = colorVal2;
                triangle_vertices[10] = colorVal3;
                triangle_vertices[16] = colorVal;
                triangle_vertices[5] = colorVal3;
                triangle_vertices[11] = colorVal;
                triangle_vertices[17] = colorVal2;
            }else{
                triangle_vertices[3] = triangleColor1.x;
                triangle_vertices[9] = triangleColor1.y;
                triangle_vertices[15] = triangleColor1.z;
                triangle_vertices[4] = triangleColor2.x;
                triangle_vertices[10] = triangleColor2.y;
                triangle_vertices[16] = triangleColor2.z;
                triangle_vertices[5] = triangleColor3.x;
                triangle_vertices[11] = triangleColor3.y;
                triangle_vertices[17] = triangleColor3.z;
            }
            triShader.use();
            triShader.setFloat("xOffset", xoffset);
            triShader.setFloat("yOffset", yoffset);

            glBindBuffer(GL_ARRAY_BUFFER, VBO_tri);
            glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

            glBindVertexArray(VAO_tri);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // delete imgui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO_rect);
    glDeleteBuffers(1, &VBO_rect);
    glDeleteBuffers(1, &EBO_rect);
    glDeleteVertexArrays(1, &VAO_tri);
    glDeleteBuffers(1, &VBO_tri);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // std::cout << cameraPos.x << " " << cameraPos.y << " " << cameraPos.z << std::endl;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(fov >= 1.0f && fov <= 90.0f)
        fov -= yoffset;
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= 90.0f)
        fov = 90.0f;
}