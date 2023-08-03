#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>


#include "Game/Game.h"
#include"Resources/ResourceManager.h"
#include "Renderer/Renderer.h"



enum { Blue=2, Red=0, Green=1 };
short currentChannel = Blue;
double arrCol[3] = { 1.0, 0.0, 0.0 };
bool flag_plus = true;
double addingValue = 0.005;

glm::ivec2 g_windowSize(13*16, 14*16);
std::unique_ptr<Game> g_game = std::make_unique<Game>(g_windowSize);



// Прототипы
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height);
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode);
void adding(short channel, double addingValue, bool plus);

int main(int argc, char** argv)
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
    GLFWwindow* pWindow = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Battle City", nullptr, nullptr);
    if (!pWindow)
    {
        std::cout << "glfwCreateWindow() failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Назначение функций для Callback-оф
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
    std::cout << "Renderer: " << RenderEngine::Renderer::getRendererStr() << std::endl;
    std::cout << "OPenGL version: " << RenderEngine::Renderer::getVersionStr() << std::endl;
    
   
    
    RenderEngine::Renderer::setClearColor(0.0f,0.5f,0.5f,1.0f);
    RenderEngine::Renderer::setDepth(true);
    {
        // При запуске программы в функцыю main передаётся путь к exe.файлу
        ResourceManager::setExecutablePath(argv[0]);
        g_game->init();
        glfwSetWindowSize(pWindow, static_cast<int>(3*g_game->getCurrentLevelWidth()), static_cast<int>(3 * g_game->getCurrentLevelHeight()));
        
        auto lastTime = std::chrono::high_resolution_clock::now();

        // Цикл отрисовки картинки
        while (!glfwWindowShouldClose(pWindow))
        {
            auto currentTime = std::chrono::high_resolution_clock::now();

            double duration = std::chrono::duration<double, std::milli>(currentTime - lastTime).count();
            lastTime = currentTime;
            g_game->update(duration);

            // Отчищает буфер цвета или кадра
            RenderEngine::Renderer::clear();

            //команда отрисовки

            if (Game::lighting)
            {

                RenderEngine::Renderer::setClearColor(arrCol[0], arrCol[1], arrCol[2], 1);
                adding(currentChannel, addingValue, flag_plus);
                
            }

           

            g_game->render();

           

            // Менает местами буферы
            glfwSwapBuffers(pWindow);

            // Обработка всех ивентов из-вне (клава, мышка, изменнеие размера онка)
            glfwPollEvents();
        }

        g_game = nullptr;

        ResourceManager::unloadAllResources();
    }
    // освобожождениие занятых ресурсов
    glfwTerminate();
    
    return 0;

}


// Вызывается при изменении окна
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_windowSize.x = width;
    g_windowSize.y = height;
    const float map_aspect_ratio = static_cast<float>(g_game->getCurrentLevelWidth()) / g_game->getCurrentLevelHeight();;
    
    unsigned int viewPortWidth = width;
    unsigned int viewPortHeight = height;
    unsigned int viewPortLeftOffset = 0;
    unsigned int viewPortBottomOffset = 0;

    if (static_cast<float>(g_windowSize.x) / g_windowSize.y > map_aspect_ratio)
    {
        viewPortWidth = static_cast<int>(g_windowSize.y * map_aspect_ratio);
        viewPortLeftOffset = (g_windowSize.x - viewPortWidth) / 2;
    }
    else
    {
        viewPortHeight = static_cast<int>(g_windowSize.x / map_aspect_ratio);
        viewPortBottomOffset = (g_windowSize.y - viewPortHeight) / 2;
    }

    // Показываме OpenGL где мы хотим рисовать. 
    // Первые два параметра -- кординаты начала полотна, затем ширина и высота
    RenderEngine::Renderer::setViewport(viewPortWidth, viewPortHeight, viewPortLeftOffset, viewPortBottomOffset);
}

// Вызывается при нажатии клавиши
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        // Устанавливаем флаг, на закрытые окна
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }
    g_game->setKey(key, action);
}

void adding(short channel, double addingValue ,bool plus)
{
    if (plus)
    {
        arrCol[channel] += addingValue;
        arrCol[(channel + 1) % 3] -= addingValue*arrCol[channel];
    }
    else
    {
        arrCol[channel] -= addingValue;
        arrCol[(channel + 1) % 3] += addingValue * arrCol[channel];
    }
    if (arrCol[channel] >= 1.0 || arrCol[channel] <0.0)
    {
        flag_plus = (flag_plus + 1) % 2;
        currentChannel = (currentChannel + 1) % 3;
    }

}