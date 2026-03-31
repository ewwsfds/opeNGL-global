#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <map>


struct vertex
{
    float x, y, z;
};


struct rectListStruct
{
    std::string name;
    int index;


    rectListStruct(std::string n, int i)
        : name(n), index(i){}
};

std::vector<vertex> vertices = {
    // first quad
    {-0.5f, -0.5f, 0.0f}, 
    { 0.5f, -0.5f, 0.0f},
    { 0.5f,  0.5f, 0.0f},
    {-0.5f,  0.5f, 0.0f},



};


std::vector<unsigned int> indices = {
    // first quad
    0, 1, 2,
    2, 3, 0,


};



std::map<std::string, int> rectList;
struct Rectangles
{
    std::string name;
    int index;
    float left, top, right, bottom;


    Rectangles(std::string n, int i, float l, float t, float r, float b)
        : name(n), index(i), left(l), top(t), right(r), bottom(b)
    {
    }


    void vertices_init(unsigned int& VBO, unsigned int& EBO) {
        vertices.push_back({ left, top, 0.0f });
        vertices.push_back({ left, bottom, 0.0f });
        vertices.push_back({ right, top, 0.0f });
        vertices.push_back({ right, bottom, 0.0f });

        indices.push_back(index);
        indices.push_back(index + 1);
        indices.push_back(index + 2);

        indices.push_back(index + 2);
        indices.push_back(index + 3);
        indices.push_back(index);



        // 🔥 Update VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

        // 🔥 Update EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
};

// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform float offset;


void main()
{
    gl_Position = vec4(aPos.x+offset,aPos.y,aPos.z, 1.0);
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

// Compile a shader
unsigned int compileShader(unsigned int type, const char* source)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compile error:\n" << infoLog << std::endl;
    }

    return shader;
}

// Create shader program
unsigned int createShaderProgram()
{
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

int main()
{
    glfwInit();

    // OpenGL 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangle", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create window\n";
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, 800, 600);

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex layout (position only)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int shaderProgram = createShaderProgram();


    int offsetLoc = glGetUniformLocation(shaderProgram, "offset");

    float offset=0;
    bool first=false;
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        static bool added = false;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS &&!added && !first)
        {
            added = true;

            Rectangles rect("firstQuad", (vertices.size()/4), 0, 0, 1, 1);
            rect.vertices_init(VBO,EBO);
            rectList[rect.name] = rect.index;

            first = true;
        }

        else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !added)
        {
            added = true;

            Rectangles rect("restQuad", (vertices.size() / 4), 0, 0, 0.2, 1);
            rect.vertices_init(VBO, EBO);
            rectList[rect.name] = rect.index;

        }

        float time = glfwGetTime();

         offset = sin(time) * 0.3f;;

        glUniform1f(offsetLoc, offset);          // send to shader

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        
        // i dont want second triangle to move at all
        if (rectList.find("firstQuad") != rectList.end())
        {
            glUniform1f(offsetLoc, -offset);          // send to shader
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int) * rectList["firstQuad"]));

        }
        glUniform1f(offsetLoc, 0);          // send to shader

        glDrawElements(GL_TRIANGLES, vertices.size()/4*6, GL_UNSIGNED_INT, (void*)0);

        


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
