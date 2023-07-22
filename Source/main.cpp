#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "Renderer/ShaderProgram.h"
#include "Resources/ResourceManager.h"
#include "Renderer/Texture2D.h"
#include "Renderer/sprite.h"
// Переменные
GLfloat point[] =
{
    0.0f, 50.0f, 0.0f,
    50.0f, -50.0f, 0.0f,
    -50.0f, -50.0f, 0.0f
};

GLfloat colors[] =
{
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

GLfloat texCoord[] =
{
    0.5f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
};


enum { Blue=2, Red=0, Green=1 };
short currentChannel = Blue;
double arrCol[3] = { 1.0, 0.0, 0.0 };
bool flag_plus = true;
double addingValue = 0.00005;


glm::ivec2 g_windowSize(640, 480);



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
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OPenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(1, 1, 0, 1);
    {
        // При запуске программы в функцыю main передаётся путь к exe.файлу
        ResourceManager resourceManager(argv[0]);
        auto pDefaultShaderProgram = resourceManager.loadShaders("DefaultShader",
            "res/shaders/vertex.txt", "res/shaders/fragment.txt");
        if (!pDefaultShaderProgram)
        {
            std::cerr << "Can't create shader program:" << "DefaultShader" << std::endl;
            return -1;
        }

        auto pSpriteShaderProgram = resourceManager.loadShaders("SpriteShader",
            "res/shaders/vSprite.txt", "res/shaders/fSprite.txt");
        if (!pSpriteShaderProgram)
        {
            std::cerr << "Can't create shader program:" << "SpriteShader" << std::endl;
            return -1;
        }

        auto tex = resourceManager.loadTexture("DefaultTexture", "res/textures/map_16x16.png");

        std::vector<std::string> subTexturesNames = { "block", "topBlock", "bottomBLock", "leftBlock", "rightBlock",
                                                     "topLeftBlock", "topRightBlock", "bottomLeftBlock", "bottomRightBlock" ,"betton"};

        auto pTextureAtlas = resourceManager.loadTextureAtlas("DefaultTextureAtlas", "res/textures/map_16x16.png", std::move(subTexturesNames), 16,16);

        auto pSprite = resourceManager.loadSprite("NewSprite", "DefaultTextureAtlas", "SpriteShader", 100, 100, "betton");
        pSprite->setPosition(glm::vec2(200, 100));


        // Передаем наш шейдер в память видеокарты
        GLuint points_vbo = 0; // хранит идентификатор, полученный от карты
        glGenBuffers(1, &points_vbo); // драйвер генерируте VertexBufferObject и записывает значени в переменную
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // подключили буффер типа GL_ARRAY_BUFFER
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); // передаем в видеокарту
        // виду буфера, размер буфера, передаем указатель на буфер, и даём подсказу, куда записывать
        //данные в видеокарте. GL_STATIC_DRAW -- говорит оптимально размещать в хранилище видеокарты

        // Тоже самое для фрагментированного шейдера, отвечающего за цвет
        GLuint colors_vbo = 0;
        glGenBuffers(1, &colors_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

        GLuint texCoord_vbo = 0;
        glGenBuffers(1, &texCoord_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(texCoord), texCoord, GL_STATIC_DRAW);

        // Настраиваем связку и обработку данных для шейдеров в видеокарте
        GLuint vao = 0; // vectors array object
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao); // делаем текущим

        glEnableVertexAttribArray(0); // включает нулевую позицию в шейдоре. То что было написано как layout
        glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // можно работать только с одним буфером памяти, 
        //а потому постоянно переключаемся между ними
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);// связываем данные:
        // указываем индекс layout, затем количество передаваемых вертексов, затем тип, затем
        //указываем, нужно ли нормировать, шаг смещения, указатель на смещение в массиве.

        // Аналогично всё и для цвета:
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        // Для текстуры
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, texCoord_vbo);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        pDefaultShaderProgram->use();
        pDefaultShaderProgram->setInt("tex", 0); // установка переменной текстуры шейдера из нулевого слота

        glm::mat4 modelMatrix_1 = glm::mat4(1.0f);
        modelMatrix_1 = glm::translate(modelMatrix_1, glm::vec3(100.0f, 50.0f, 0.0f));
                
        glm::mat4 modelMatrix_2 = glm::mat4(1.0f);
        modelMatrix_2 = glm::translate(modelMatrix_2, glm::vec3(590.0f, 50.0f, 0.0f));

        glm::mat4 projectionMatrix = glm::ortho(0.0f, static_cast<float>(g_windowSize.x), 0.0f, static_cast<float>(g_windowSize.y), -100.0f, 100.0f);

        pDefaultShaderProgram->setMatrix4("projectionMat", projectionMatrix);

        pSpriteShaderProgram->use();
        pSpriteShaderProgram->setInt("tex", 0);
        pSpriteShaderProgram->setMatrix4("projectionMat", projectionMatrix);
        
        // Цикл отрисовки картинки
        while (!glfwWindowShouldClose(pWindow))
        {
            // Отчищает буфер цвета или кадра
            glClear(GL_COLOR_BUFFER_BIT);

            // Покдлючаем шейдерную программу
            pDefaultShaderProgram->use();
            //Подключаем vertex attribute object
            glBindVertexArray(vao);
            tex->bind();

            //команда отрисовки
            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix_1);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            pDefaultShaderProgram->setMatrix4("modelMat", modelMatrix_2);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glClearColor(arrCol[0], arrCol[1], arrCol[2], 1);

            pSprite->render();

            adding(currentChannel, addingValue, flag_plus);

            // Менает местами буферы
            glfwSwapBuffers(pWindow);

            // Обработка всех ивентов из-вне (клава, мышка, изменнеие размера онка)
            glfwPollEvents();
        }
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
    // Показываме OpenGL где мы хотим рисовать. 
    // Первые два параметра -- кординаты начала полотна, затем ширина и высота
    glViewport(0, 0, g_windowSize.x, g_windowSize.y);
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