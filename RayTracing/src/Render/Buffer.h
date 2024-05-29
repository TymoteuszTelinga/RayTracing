#pragma once
#include <array>
#include <vector>
#include <iostream>
#include "BufferBase.h"

template<typename T, size_t MaxSize = 1000>
class Buffer : public BufferBase
{
public:
	Buffer();
	~Buffer() {};

	uint32_t GetSize() const { return m_Size; }
	T& GetElement(uint32_t index) { return m_Data[index]; }
	void AddElement(const T& element);
	void AddElements(const std::vector<T>& elements);
	//remove last element
	void RemoveElement();
	void Update();

private:
	std::array<T, MaxSize> m_Data;
	uint32_t m_Size = 0;
};

template<typename T, size_t MaxSize>
inline Buffer<T, MaxSize>::Buffer() : BufferBase(sizeof(m_Data))
{
}

template<typename T, size_t MaxSize>
inline void Buffer<T, MaxSize>::AddElement(const T& element)
{
	m_Data[m_Size] = element;
	m_Size++;
}

template<typename T, size_t MaxSize>
inline void Buffer<T, MaxSize>::AddElements(const std::vector<T>& elements)
{
	memcpy(m_Data.data()+m_Size, elements.data(), elements.size()* sizeof(T));
	m_Size += elements.size();
}

template<typename T, size_t MaxSize>
inline void Buffer<T, MaxSize>::RemoveElement()
{
	if (m_Size > 0)
	{
		m_Size--;
	}
}

template<typename T, size_t MaxSize>
inline void Buffer<T, MaxSize>::Update()
{
	SendData(&m_Data, m_Size*sizeof(T));
}
