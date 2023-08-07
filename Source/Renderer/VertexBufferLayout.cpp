#include "VertexBufferLayout.h"

namespace RenderEngine
{
	VertexBufferLayout::VertexBufferLayout()
		: m_stride(0)
	{

	}

	void VertexBufferLayout::reserveElements(const size_t count)
	{
		m_layoutElements.reserve(count);
	}

	void VertexBufferLayout::addElementLayoutFloat(const unsigned int count, const bool normilized)
	{
		VertexBufferLayoutElement tempStruct{ static_cast<GLint>(count), GL_FLOAT, normilized, static_cast<unsigned int>(count * sizeof(GLfloat)) };
		m_layoutElements.push_back(tempStruct);
		m_stride += m_layoutElements.back().size;
	}
}

