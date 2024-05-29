#pragma once
#include <cstdint>
#include <string>

class Image
{
public:
	Image(uint32_t Width, uint32_t Height);
	~Image();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	void Resize(uint32_t Width, uint32_t Height);
	void Invalidate();

	inline uint32_t GetRendererID() const { return m_RendererID; };
	inline uint32_t GetWidth() const { return m_Width; };
	inline uint32_t GetHeight() const { return m_Height; };
	void Save(const std::string& filename);
	uint8_t* GetImageData() const;

private:
	uint32_t m_RendererID;
	uint32_t m_Width;
	uint32_t m_Height;
};

