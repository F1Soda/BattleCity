#pragma once 

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ShaderProgram.h"
#include <string>
#include <iostream>

namespace RenderEngine
{
	class Renderer
	{
	public:
		static void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shader);
		static void drawUI(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const ShaderProgram& shader);
		static void setClearColor(float r, float g, float b, float a);
		static void Renderer::clear();
		static void setDepth(const bool enable);
		static void setViewport(unsigned int width, unsigned int height, unsigned int leftOffset = 0, unsigned int bottomOffset = 0);

		static char* getRendererStr();
		static char* getVersionStr();

		static void drawLine();

	private:


	};
}