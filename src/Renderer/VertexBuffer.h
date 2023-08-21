#pragma once

#include <glad/glad.h>

namespace RenderEngine
{
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();

		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer) = delete;
		VertexBuffer& operator=(VertexBuffer&& vertexBuffer) noexcept;
		VertexBuffer(VertexBuffer&& vertexBuffer) noexcept;

		void init(const void* data, const unsigned int size);
		void update(const void* data, const unsigned int size) const;
		void bind() const;
		void unbind() const;
		const GLuint getID() const { return m_id; }

	private:
		GLuint m_id;

	};
}