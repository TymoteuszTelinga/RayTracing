#pragma once
#include <cstdint>

//static data buffer
class BufferBase
{
public:
	BufferBase(uint32_t size);
	~BufferBase();

	void Bind(uint32_t slot = 0);
	void Unbind();

	void SendData(const void* data, uint32_t size);

private:

	uint32_t m_RendererID;
};

