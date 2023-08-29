#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>



#include "Game/Game.h"
#include "Game/UI/GameStates/Level.h"
#include"Resources/ResourceManager.h"
#include "Renderer/Renderer.h"
#include "Physics/PhysicsEngine.h"
#include "Game/GameManager.h"

// ------------------------------------------------ Bu13D@$t ------------------------------------------------------


enum { Blue=2, Red=0, Green=1 };
short currentChannel = Blue;
float arrCol[3] = { 1.0f, 0.0f, 0.0f };
bool flag_plus = true;
float addingValue = 0.005f;

static constexpr unsigned int SCALE = 3;
static constexpr unsigned int BLOCK_SIZE = 16;

glm::uvec2 g_windowSize(SCALE*16* BLOCK_SIZE, SCALE * 15* BLOCK_SIZE);
std::unique_ptr<Game> g_game = std::make_unique<Game>(g_windowSize);




// ���������
void debugLogInConsole(double delta);
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height);
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode);
void adding(short channel, float addingValue, bool plus);
static std::string getStrKeyFromEnumOrientationTank(Tank::EOrientation eValue);
void window_pos_callback(GLFWwindow* window, int xpos, int ypos);

int main(int argc, char** argv)
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
    Game::pWindow = glfwCreateWindow(g_windowSize.x, g_windowSize.y, "Battle City", nullptr, nullptr);
    if (!Game::pWindow)
    {
        std::cout << "glfwCreateWindow() failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // ���������� ������� ��� Callback-��
    glfwSetWindowSizeCallback(Game::pWindow, glfwWindowSizeCallback);
    glfwSetWindowPosCallback(Game::pWindow, window_pos_callback);
    glfwSetKeyCallback(Game::pWindow, glfwKeyCallback);

    // ������ �������� ���� �������, ���� ���� ����� ���� ���������
    glfwMakeContextCurrent(Game::pWindow);

    if (!gladLoadGL())
    {
        std::cout << "Can't load GLAD" << std::endl;
        return -1;
    }
 
    
    
   
    
    RenderEngine::Renderer::setClearColor(0.0f,0.5f,0.5f,1.0f);
    RenderEngine::Renderer::setDepth(true);
    {
        // ��� ������� ��������� � ������� main ��������� ���� � exe.�����
        Resources::ResourceManager::setExecutablePath(argv[0]);
       
        g_game->init();
        Physics::PhysicsEngine::init(g_game->getGameManager());
        

        //glfwSetWindowSize(Game::pWindow, static_cast<int>(3*g_game->getCurrentWidth()), static_cast<int>(3 * g_game->getCurrentHeight()));
        
        auto lastTime = std::chrono::high_resolution_clock::now();

        // ���� ��������� ��������
        while (!glfwWindowShouldClose(Game::pWindow))
        {
            auto currentTime = std::chrono::high_resolution_clock::now();

            double duration = std::chrono::duration<double, std::milli>(currentTime - lastTime).count();
            lastTime = currentTime;
            g_game->update(Game::pWindow ,duration);
            


            // �������� ����� ����� ��� �����
            RenderEngine::Renderer::clear();

            //������� ���������
            if (Game::lighting)
            {

                RenderEngine::Renderer::setClearColor(arrCol[0], arrCol[1], arrCol[2], 1);
                adding(currentChannel, addingValue, flag_plus);
                
            }
            // -------------- debug mode -------------- \\ 
            //debugLogInConsole(duration);
            g_game->render();
            
            

            // ������ ������� ������
            glfwSwapBuffers(Game::pWindow);

            // ��������� ���� ������� ��-��� (�����, �����, ��������� ������� ����)
            glfwPollEvents();
        }
        Physics::PhysicsEngine::terminate();
        g_game = nullptr;
        Resources::ResourceManager::unloadAllResources();
    }
    // ��������������� ������� ��������
    glfwTerminate();
    return 0;
}


// ���������� ��� ��������� ����
void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_game->pauseWhenChangingSizeOrPosionWindow();
    g_windowSize.x = width;
    g_windowSize.y = height;
    g_game->setWindowSize(g_windowSize);
}

// ���������� ��� ������� �������
void glfwKeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //{
    //    // ������������� ����, �� �������� ����
    //    //glfwSetWindowShouldClose(pWindow, GL_TRUE);
    //}
    g_game->setKey(key, action);
}

void adding(short channel, float addingValue ,bool plus)
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

void debugLogInConsole(double delta)
{
    static double timeCounter = 0;
    timeCounter += delta;
    if (timeCounter >= 500)
    {
        timeCounter = 0;
        system("cls");
        // ����� �������� ���������� � ������ OpenGL
        std::cout << "Renderer: " << RenderEngine::Renderer::getRendererStr() << std::endl;
        std::cout << "OPenGL version: " << RenderEngine::Renderer::getVersionStr() << std::endl;
        std::cout << "First button pressed: " << getStrKeyFromEnumOrientationTank(Level::eMoveStateFirstButtonFirstPlayer) << std::endl;
        std::cout << "Second button pressed: " << getStrKeyFromEnumOrientationTank(Level::eMoveStateSecondButtonFirstPlayer) << std::endl;

    }
}

static std::string getStrKeyFromEnumOrientationTank(Tank::EOrientation eValue)
{
    switch (eValue)
    {
    case Tank::EOrientation::Top: return "W (\"Forward\")"; break;
    case Tank::EOrientation::Bottom: return "S (\"Down\")"; break;
    case Tank::EOrientation::Left: return "A (\"Left\")"; break;
    case Tank::EOrientation::Right: return "D (\"Right\")"; break;
    default:
        return " Empty ";
        break;
    }
}

void window_pos_callback(GLFWwindow* window, int xpos, int ypos)
{
    g_game->pauseWhenChangingSizeOrPosionWindow();
}