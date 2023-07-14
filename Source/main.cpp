#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <iostream>

// Переменные
int g_windowSizeX = 640;
int g_windowSizeY = 480;

// Прототипы
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

    //Указываем версию программы, старше которой программа может запускаться 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // В данном случае версия 4,6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // Core profile подномжество функций openGL, куда не включены все функции, 
    //необходимые для обратной совместимости. Строчкой ниже мы указываем 
    // использовать это при работе
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

    // Деламе контекст окна текущим, ведь окон может быть несколько
    glfwMakeContextCurrent(pWindow);

    if (!gladLoadGL())
    {
        std::cout << "Can't load GLAD" << std::endl;
        return -1;
    }

    // Вывод названия видеокарты и версии OpenGL
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OPenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(1, 1, 0, 1);

    // Цикл отрисовки картинки
    while (!glfwWindowShouldClose(pWindow))
    {
        // Отчищает буфер цвета
        glClear(GL_COLOR_BUFFER_BIT);

        // Менает местами буферы
        glfwSwapBuffers(pWindow);

        // Обработка всех ивентов из-вне (клава, мышка, изменнеие размера онка)
        glfwPollEvents();
    }
    // освобожождениие занятых ресурсов
    glfwTerminate();
    return 0;
}


// Вызывается при изменении окна
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_windowSizeX = width;
    g_windowSizeY = height;
    // Показываме OpenGL где мы хотим рисовать. 
    // Первые два параметра -- кординаты начала полотна, затем ширина и высота
    glViewport(0, 0, g_windowSizeX, g_windowSizeY);
}

// Вызывается при нажатии клавиши
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        // Устанавливаем флаг, на закрытые окна
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }
}