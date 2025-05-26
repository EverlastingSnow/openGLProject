#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "camera.h"
#include "consts.h"

#include <cmath>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "model.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void define_light_properties(Shader& shader);

unsigned int loadTexture(char const * path);


// settings
bool spin_rectangle = false;
bool showTriangle = false;
bool random_triangleColor = false;
bool showRectangle = false;
bool showCube = false;
bool multipleCubes = false;
bool spinCubeView = false;
bool firstMouse = true;
bool showLight = false;
bool showApple = false;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f, lastY = SCR_HEIGHT / 2.0f;


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

        // triangle
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

        // rectangle
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
        //Cube
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
        }
        if (ImGui::CollapsingHeader("Light Settings")){
            if (ImGui::Button("show Light cubes")){
                showLight = !showLight;
            }
        }

        if (ImGui::CollapsingHeader("apple Settings")){
            if (ImGui::Button("show apple")){
                showApple = !showApple;
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Snow", NULL, NULL);
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

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

    // Assimp::Importer importer;
    // const aiScene* scene = importer.ReadFile("resources\\apple\\Model.obj", aiProcess_Triangulate);
    // if (!scene) {
    //     std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
    //     return -1;
    // }
    // std::cout << "Model loaded successfully!" << std::endl;
    // return 0;

    // build and compile our shader program
    // ------------------------------------
    Shader triShader("fs_vs\\tri_shader.vs", "fs_vs\\tri_shader.fs"); // you can name your shader files however you like
    Shader rectShader("fs_vs\\rect_shader.vs", "fs_vs\\rect_shader.fs");
    Shader cubeShader("fs_vs\\cube_shader.vs", "fs_vs\\cube_shader.fs"); 
    Shader lightShader("fs_vs\\light_shader.vs", "fs_vs\\light_shader.fs");
    // Shader appleShader("fs_vs\\apple_shader.vs", "fs_vs\\apple_shader.fs");
    Shader appleShader("fs_vs\\cube_shader.vs", "fs_vs\\cube_shader.fs");

    Model appleModel("resources\\apple\\Model.obj");

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
    

    unsigned int texture1 = loadTexture("resources\\container.jpg");
    unsigned int texture2 = loadTexture("resources\\laugh.png");
    
    rectShader.use();
    rectShader.setFloat("Beta", rectBeta);
    rectShader.setInt("texture1", 0);
    rectShader.setInt("texture2", 1);

    unsigned int VBO_cube, VAO_cube;
    glGenVertexArrays(1, &VAO_cube);
    glGenBuffers(1, &VBO_cube);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO_cube);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = loadTexture("resources\\container2.png");
    unsigned int specularMap = loadTexture("resources\\container2_specular.png");

    

    cubeShader.use();
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    

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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        my_gui();

        // render the apple
        if (showApple) {
            define_light_properties(appleShader);

            appleShader.use();
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            appleShader.setMat4("projection", projection);
            appleShader.setMat4("view", view);

            // render the loaded model
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0f, 0.0f, -5.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            appleShader.setMat4("model", model);
            appleModel.Draw(appleShader);

        }

        // render the light
        if (showLight){
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            lightShader.use();
            lightShader.setMat4("projection", projection);
            lightShader.setMat4("view", view);
       
            // we now draw as many light bulbs as we have point lights.
            glBindVertexArray(lightVAO);
            for (unsigned int i = 0; i < 4; i++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
                lightShader.setMat4("model", model);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
   
        }

        // render the cube
        if (showCube) {
            define_light_properties(cubeShader);
            // make sure to initialize matrix to identity matrix first
            glm::mat4 cube_view          = glm::mat4(1.0f);
            glm::mat4 cube_projection    = glm::mat4(1.0f);
            // cube_view  = glm::translate(cube_view, glm::vec3(0.0f, 0.0f, -3.0f));
            cube_projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            cube_view = camera.GetViewMatrix();
            cubeShader.setMat4("view", cube_view);
            cubeShader.setMat4("projection", cube_projection);

            glm::mat4 model = glm::mat4(1.0f);
            cubeShader.setMat4("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);

            glBindVertexArray(VAO_cube);
            if (multipleCubes){
                MAX_CUBE_NUM = 10;
            }else MAX_CUBE_NUM = 1;
            for (int i = 0;i < MAX_CUBE_NUM;++i){
                glm::mat4 cube_model         = glm::mat4(1.0f);
                cube_model = glm::translate(cube_model, cube_positions[i]);
                float angle = 20.0f * i;
                if (spinCubeView) {
                    angle = angle * glfwGetTime();
                } 
                cube_model = glm::rotate(cube_model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                cubeShader.setMat4("model", cube_model);
            
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
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &VAO_cube);
    glDeleteBuffers(1, &VBO_cube);
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
void define_light_properties(Shader& shader){
    shader.use();
    // shader.setVec3("light.position", lightPos);
    // shader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("viewPos", camera.Position);
    shader.setFloat("material.shininess", 32.0f);

    // light properties
    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    shader.setVec3("pointLights[0].position", pointLightPositions[0]);
    shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("pointLights[0].constant", 1.0f);
    shader.setFloat("pointLights[0].linear", 0.09f);
    shader.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    shader.setVec3("pointLights[1].position", pointLightPositions[1]);
    shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("pointLights[1].constant", 1.0f);
    shader.setFloat("pointLights[1].linear", 0.09f);
    shader.setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    shader.setVec3("pointLights[2].position", pointLightPositions[2]);
    shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("pointLights[2].constant", 1.0f);
    shader.setFloat("pointLights[2].linear", 0.09f);
    shader.setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    shader.setVec3("pointLights[3].position", pointLightPositions[3]);
    shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("pointLights[3].constant", 1.0f);
    shader.setFloat("pointLights[3].linear", 0.09f);
    shader.setFloat("pointLights[3].quadratic", 0.032f);
    // spotLight
    shader.setVec3("spotLight.position", camera.Position);
    shader.setVec3("spotLight.direction", camera.Front);
    shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09f);
    shader.setFloat("spotLight.quadratic", 0.032f);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));   
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

bool isDragging = false;
glm::vec2 dragStartPos;
glm::vec3 focusPoint(0.0f); // 相机聚焦的中心点
float cameraDistance = 5.0f; // 相机到焦点的距离

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isDragging = true;
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            dragStartPos = glm::vec2(x, y);
        } else if (action == GLFW_RELEASE) {
            isDragging = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!isDragging) return;

    glm::vec2 currentPos(xpos, ypos);
    glm::vec2 delta = currentPos - dragStartPos;
    dragStartPos = currentPos;

    // 获取窗口尺寸
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // 转换为标准化设备坐标
    glm::vec2 ndcDelta = delta / glm::vec2(width, height);
    
    // 旋转速度控制
    float rotationSpeed = 5.0f;
    float horizontalAngle = ndcDelta.x * rotationSpeed;
    float verticalAngle = ndcDelta.y * rotationSpeed;

    // 当前相机位置相对于焦点的向量
    glm::vec3 relativePos = camera.Position - focusPoint;
    
    // 创建四元数旋转
    glm::quat horizontalRot = glm::angleAxis(glm::radians(-horizontalAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat verticalRot = glm::angleAxis(glm::radians(verticalAngle), camera.Right);
    
    // 组合旋转
    glm::quat totalRot = horizontalRot * verticalRot;
    relativePos = totalRot * relativePos;
    
    // 更新相机位置
    camera.Position = focusPoint + relativePos;
    
    // 更新相机朝向
    camera.Front = glm::normalize(focusPoint - camera.Position);
    camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
    camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
}

// 滚轮缩放控制
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cameraDistance = glm::max(1.0f, cameraDistance - (float)yoffset * 0.5f);
    
    // 保持相机看向焦点
    glm::vec3 direction = glm::normalize(camera.Position - focusPoint);
    camera.Position = focusPoint + direction * cameraDistance;
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
