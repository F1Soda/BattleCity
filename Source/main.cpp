#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <iostream>

// ����������
int g_windowSizeX = 640;
int g_windowSizeY = 480;

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

    // ���� ��������� ��������
    while (!glfwWindowShouldClose(pWindow))
    {
        // �������� ����� �����
        glClear(GL_COLOR_BUFFER_BIT);

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