#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"

#include <cmath>
#include <iostream>

// #include <imgui/imgui.h>
// #include <imgui/imgui_impl_glfw.h>
// #include <imgui/imgui_impl_opengl3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
bool spin_rectangle = false;
bool showTriangle = false;
bool random_triangleColor = false;
bool showRectangle = false;
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

        ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

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
    Shader rectShader("src\\rect_shader.vs", "src\\rect_shader.fs"); // you can name your shader files however you like
    

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


    // render loop
    // -----------
    float xoffset = 0.0f;
    float yoffset = 0.0f;
    float colorVal = 0.0f;
    float colorVal2 = 0.0f;
    float colorVal3 = 0.0f;
    
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        my_gui();

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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}