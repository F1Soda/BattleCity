#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <iostream>

// ����������
GLfloat point[] =
{
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f
};

GLfloat colors[] =
{
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

const char* vertex_shader =
"#version 460\n"
"layout(location = 0) in vec3 vertex_position;" // layout -- ������������� ��� ����������
"layout(location = 1) in vec3 vertex_color;" // ������� ����� ����� �������� ����������
"out vec3 color;" // ���������� in � out ��������� �������� ��� ������ ������ ����������
"void main(){"
"   color = vertex_color;" // ���� ��� ������ �������� ���������� ����� �������� �� �������
"   gl_Position = vec4(vertex_position, 1.0);" // ���������� ��������� ��������, ������� ����� ��������
"}";

const char* fragment_shader =
"#version 460\n"
"in vec3 color;"
"out vec4 frag_color;"
"void main() {"
"   frag_color = vec4(color, 1.0);" // ������������� ��� ����������������� ��������
"}";

int g_windowSizeX = 800;
int g_windowSizeY = 600;


// ���������
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height);
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode);


int main(void)
{
    

    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "glfwInit() failed" << std::endl;
        return -1;
    }

    //��������� ������ ���������, ������ ������� ��������� ����� ����������� 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // � ������ ������ ������ 4,6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // Core profile ������������ ������� openGL, ���� �� �������� ��� �������, 
    //����������� ��� �������� �������������. �������� ���� �� ��������� 
    // ������������ ��� ��� ������
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* pWindow = glfwCreateWindow(g_windowSizeX, g_windowSizeY, "Battle City", nullptr, nullptr);
    if (!pWindow)
    {
        std::cout << "glfwCreateWindow() failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // ���������� ������� ��� Callback-��
    glfwSetWindowSizeCallback(pWindow, glfwWindowSizeCallback);
    glfwSetKeyCallback(pWindow, glfwKeyCallback);

    // ������ �������� ���� �������, ���� ���� ����� ���� ���������
    glfwMakeContextCurrent(pWindow);

    if (!gladLoadGL())
    {
        std::cout << "Can't load GLAD" << std::endl;
        return -1;
    }

    // ����� �������� ���������� � ������ OpenGL
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OPenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(1, 1, 0, 1);

   

    
    // ���������� ������������� ����������� �������, ����� ����� �������� ������� OpenGL
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    // �������� ����� ������� �������� ���
    glShaderSource(vs, 1, &vertex_shader, nullptr);
    // ���������� �������
    glCompileShader(vs);

    // ���� ����� ������ ��� ������������ �������
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, nullptr);
    glCompileShader(fs);

    // ���������� ��������� ���������
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vs); // ����������� � ��������� ������� ������ vs
    glAttachShader(shader_program, fs); // � fs
    glLinkProgram(shader_program); // ��������� �� �� �����

    // ������� �������, ��� ��� ������ ��� �� �����
    glDeleteShader(vs);
    glDeleteShader(fs);

    // �������� ��� ������ � ������ ����������
    GLuint points_vbo = 0; // ������ �������������, ���������� �� �����
    glGenBuffers(1, &points_vbo); // ������� ���������� VertexBufferObject � ���������� ������� � ����������
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // ���������� ������ ���� GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); // �������� � ����������
    // ���� ������, ������ ������, �������� ��������� �� �����, � ��� ��������, ���� ����������
    //������ � ����������. GL_STATIC_DRAW -- ������� ���������� ��������� � ��������� ����������

    // ���� ����� ��� ������������������ �������, ����������� �� ����
    GLuint colors_vbo = 0;
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo); 
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // ����������� ������ � ��������� ������ ��� �������� � ����������
    GLuint vao = 0; // vectors array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); // ������ �������

    glEnableVertexAttribArray(0); // �������� ������� ������� � �������. �� ��� ���� �������� ��� layout
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // ����� �������� ������ � ����� ������� ������, 
    //� ������ ��������� ������������� ����� ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);// ��������� ������:
    // ��������� ������ layout, ����� ���������� ������������ ���������, ����� ���, �����
    //���������, ����� �� �����������, ��� ��������, ��������� �� �������� � �������.

    // ���������� �� � ��� �����:
    glEnableVertexAttribArray(1); 
    glBindBuffer(GL_ARRAY_BUFFER,colors_vbo); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    

    // ���� ��������� ��������
    while (!glfwWindowShouldClose(pWindow))
    {
        // �������� ����� ����� ��� �����
        glClear(GL_COLOR_BUFFER_BIT);

        // ���������� ��������� ���������
        glUseProgram(shader_program);
        //���������� vertex attribute object
        glBindVertexArray(vao);

        //������� ���������
        glDrawArrays(GL_TRIANGLES,0, 3);

        // ������ ������� ������
        glfwSwapBuffers(pWindow);

        // ��������� ���� ������� ��-��� (�����, �����, ��������� ������� ����)
        glfwPollEvents();
    }
    // ��������������� ������� ��������
    glfwTerminate();
    return 0;
}


// ���������� ��� ��������� ����
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_windowSizeX = width;
    g_windowSizeY = height;
    // ���������� OpenGL ��� �� ����� ��������. 
    // ������ ��� ��������� -- ��������� ������ �������, ����� ������ � ������
    glViewport(0, 0, g_windowSizeX, g_windowSizeY);
}

// ���������� ��� ������� �������
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        // ������������� ����, �� �������� ����
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }
}