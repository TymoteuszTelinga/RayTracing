#include "BufferBase.h"

#include <GL/glew.h>

BufferBase::BufferBase(uint32_t size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

BufferBase::~BufferBase()
{
	glDeleteBuffers(1, &m_RendererID);
}

void BufferBase::Bind(uint32_t slot)
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_RendererID);
}

void BufferBase::Unbind()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void BufferBase::SendData(const void* data, uint32_t size)
{
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
}
