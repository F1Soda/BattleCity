#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/mat4x4.hpp>

namespace RenderEngine {
	//using std::string;
	class ShaderProgram
	{
	public:
		ShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);
		~ShaderProgram();
		bool isCompiled() const { return m_isCompiled; }
		void use() const; // ��������� ��������
		void setInt(const std::string& name, const GLint value);
		void ShaderProgram::setFloat(const std::string& name, const GLfloat value);
		void setMatrix4(const std::string& name, const glm::mat4& matrix);

		ShaderProgram() = delete; // ������ �� �������� �������� ��� ����� ��������
		ShaderProgram(ShaderProgram&) = delete; // ������ ������������ ������������� �����������
		// ����� ����� ���������� �������� ��� ������ �����������
		ShaderProgram& operator=(const ShaderProgram&) = delete; // ������ ������������ �������,
		// ��� ��� ��������� ������� � ����������� ����������������, ��� ������ ���� 
		// ������� ������ ��� �����������
		ShaderProgram& operator=(ShaderProgram&& shaderProgram) noexcept; // && ��������, ��� ������������ �������� ����� ���� ������ r-value
		ShaderProgram(ShaderProgram&& shaderProgram) noexcept;
	private:
		bool createShader(const std::string& source, const GLenum shaderType, GLuint& shaderID);
		bool m_isCompiled = false; // ����������, ��������� �� ���������������� �������
		GLuint m_ID = 0; // ������������� ��������� ���������
	};
}