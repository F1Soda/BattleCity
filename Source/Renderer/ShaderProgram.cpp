#include "ShaderProgram.h"
#include <iostream>

namespace Renderer
{
	ShaderProgram::ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
	{
        GLuint vertexShaderID;
        if (!createShader(vertexShader, GL_VERTEX_SHADER, vertexShaderID)) 
        {
            std::cerr << "VERTEX SHADER compile-time-error" << std::endl;
            return;
        }
        GLuint fragmentShaderID;
        if (!createShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderID))
        {
            std::cerr << "FRAGMENT SHADER compile-time-error" << std::endl;
            glDeleteShader(vertexShaderID);
            return;
        }

        // Генерируем шейдерную программу
        m_ID = glCreateProgram();
        glAttachShader(m_ID, vertexShaderID); // прикрепляем к программе шейдера шедеры vs
        glAttachShader(m_ID, fragmentShaderID); // и fs
        glLinkProgram(m_ID); // Связываем всё во едино

        // Проверка на удачность линковки
        GLint succsess;
        glGetProgramiv(m_ID, GL_LINK_STATUS, &succsess);
        if (!succsess)
        {
            GLchar infLog[1024];
            glGetShaderInfoLog(m_ID, 1024, nullptr, infLog);
            std::cerr << "ERROR::SHADER: Link-time-error:\n" << infLog << std::endl;
        }
        else
        {
            m_isCompiled = true;
        }

        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
	}

    bool ShaderProgram::createShader(const std::string& source, const GLenum shaderType, GLuint& shaderID)
    {
        shaderID = glCreateShader(shaderType);
        const char* code = source.c_str();
        glShaderSource(shaderID, 1, &code, nullptr);
        glCompileShader(shaderID);

        // Проверка на ошибки
        GLint succsess; // суда записывается статут компиляции шейдера
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &succsess);
        if (!succsess)
        {
            GLchar infLog[1024];
            glGetShaderInfoLog(shaderID, 1024, nullptr, infLog);
            std::cerr << "ERROR::SHADER: Compile-time-error:\n" << infLog << std::endl;
            return false;
        }
        return true;
    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_ID);
    }

    void ShaderProgram::use() const
    {
        glUseProgram(m_ID);
    }

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram)
    {
        glDeleteProgram(m_ID);
        m_ID = shaderProgram.m_ID;
        m_isCompiled = shaderProgram.m_isCompiled;

        shaderProgram.m_ID = 0;
        shaderProgram.m_isCompiled = false;
        return *this;
    }
    
    ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram)
    {
        m_ID = shaderProgram.m_ID;
        m_isCompiled = shaderProgram.m_isCompiled;

        shaderProgram.m_ID = 0;
        shaderProgram.m_isCompiled = false;
    }

}