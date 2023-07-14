#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <iostream>

// Переменные
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
"layout(location = 0) in vec3 vertex_position;" // layout -- идентификатор для переменной
"layout(location = 1) in vec3 vertex_color;" // которая затем будет передана видеокарте
"out vec3 color;" // модифкатор in и out указывают получает или отдает данные переменная
"void main(){"
"   color = vertex_color;" // Пока что просто передаем переменной цвета значение из массива
"   gl_Position = vec4(vertex_position, 1.0);" // определяем положение вертекса, который будет закрашен
"}";

const char* fragment_shader =
"#version 460\n"
"in vec3 color;"
"out vec4 frag_color;"
"void main() {"
"   frag_color = vec4(color, 1.0);" // устанавливаем уже интерполированное значение
"}";

int g_windowSizeX = 800;
int g_windowSizeY = 600;


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

   

    
    // Обозначаем идентификатор вертексного щейдера, чтобы потом передать функции OpenGL
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    // передаем этому шейдеру исходный код
    glShaderSource(vs, 1, &vertex_shader, nullptr);
    // Компиляция шейдера
    glCompileShader(vs);

    // Тоже самое делаем для фрагментного шейдера
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, nullptr);
    glCompileShader(fs);

    // Генерируем шейдерную программу
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vs); // прикрепляем к программе шейдера шедеры vs
    glAttachShader(shader_program, fs); // и fs
    glLinkProgram(shader_program); // Связываем всё во едино

    // удаляем шейдеры, так как больше они не нужны
    glDeleteShader(vs);
    glDeleteShader(fs);

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
    glBindBuffer(GL_ARRAY_BUFFER,colors_vbo); 
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    

    // Цикл отрисовки картинки
    while (!glfwWindowShouldClose(pWindow))
    {
        // Отчищает буфер цвета или кадра
        glClear(GL_COLOR_BUFFER_BIT);

        // Покдлючаем шейдерную программу
        glUseProgram(shader_program);
        //Подключаем vertex attribute object
        glBindVertexArray(vao);

        //команда отрисовки
        glDrawArrays(GL_TRIANGLES,0, 3);

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