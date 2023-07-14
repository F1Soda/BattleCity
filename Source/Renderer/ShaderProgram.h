#pragma once
#include <glad/glad.h>
#include <string>

namespace Renderer {
	//using std::string;
	class ShaderProgram
	{
	public:
		ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
		~ShaderProgram();
		bool isCompiled() const { return m_isCompiled; }
		void use() const; // включение шейдеров

		ShaderProgram() = delete; // запрет на создание шейдеров без самих шейдеров
		ShaderProgram(ShaderProgram&) = delete; // нельзя пользоваться конструктором копирования
		// иначе могут возникнуть проблемы при вызове деструктора
		ShaderProgram& operator=(const ShaderProgram&) = delete; // нельзя приравнивать шейдеры,
		// так как получатся шейдеры с одинаковыми идентификаторами, что опаять таки 
		// вызовет ошибку при уничтожении
		ShaderProgram& operator=(ShaderProgram&& shaderProgram); // && означает, что передаваемое значение может быть только r-value
		ShaderProgram(ShaderProgram&& shaderProgram);
	private:
		bool createShader(const std::string& source, const GLenum shaderType, GLuint& shaderID);
		bool m_isCompiled = false; // показывает, нормально ли скомпилировались шейдеры
		GLuint m_ID = 0; // идентификатор шейдерной программы
	};
}