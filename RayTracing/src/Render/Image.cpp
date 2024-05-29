#include "Image.h"
#include <GL/glew.h>
#include <iostream>
#include <fstream>

#define BMP 

Image::Image(uint32_t Width, uint32_t Height) : m_Width(Width), m_Height(Height), m_RendererID(0)
{
	Invalidate();
}

Image::~Image()
{
	glDeleteTextures(1, &m_RendererID);
}

void Image::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Image::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::Resize(uint32_t Width, uint32_t Height)
{
	m_Width = Width;
	m_Height = Height;

	Invalidate();
}

void Image::Invalidate()
{
	if (m_RendererID)
	{
		glDeleteTextures(1, &m_RendererID);
	}

	glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);

	glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureStorage2D(m_RendererID, 1, GL_RGBA32F, m_Width, m_Height);
	glBindImageTexture(0, m_RendererID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

static float clamp(float val)
{
	return std::min(std::max(0.f, val), 1.f);
}

void Image::Save(const std::string& filename)
{
	float* Pixels = new float[m_Width * m_Height*4];
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, Pixels);


#ifdef BMP

	const uint8_t PaddingSize = (4 - (3 * m_Width) % 4) % 4;
	const uint32_t PixelArraySize = m_Width*m_Height*3 + PaddingSize * m_Height;

	char Padding[4] = { 0,0,0,0 };
	char BMPHeader[54] = { 0 };
	*(uint8_t* )(BMPHeader + 0x0 ) = 'B'; //ID
	*(uint8_t* )(BMPHeader + 0x1 ) = 'M'; //ID
	*(uint32_t*)(BMPHeader + 0x2 ) = PixelArraySize + 54; // FileSize
	*(uint32_t*)(BMPHeader + 0xA ) = 54; // PixelArray offset
	//strat of DIB header
	*(uint32_t*)(BMPHeader + 0xE ) = 40; // DIB Header size
	*(uint32_t*)(BMPHeader + 0x12) = m_Width; // width of image
	*(uint32_t*)(BMPHeader + 0x16) = m_Height;// height of image
	*(uint16_t*)(BMPHeader + 0x1A) = 1;  // Number of color planes being used
	*(uint16_t*)(BMPHeader + 0x1C) = 24; // Bit per pixel
	*(uint32_t*)(BMPHeader + 0x22) = PixelArraySize; // totoal size of pixel array
	*(uint32_t*)(BMPHeader + 0x26) = 2835; // Print resolution of the image horizontal
	*(uint32_t*)(BMPHeader + 0x2A) = 2835; // Print resolution of the image vertical

	std::ofstream file(filename, std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "Save image error\n";
		delete[] Pixels;
		return;
	}

	file.write(BMPHeader, 54);
	float* pixel = Pixels;
	for (uint32_t y = 0; y < m_Height; y++)
	{
		for (uint32_t x = 0; x < m_Width; x++)
		{
			uint8_t r = clamp(*(pixel)) * 255;
			uint8_t g = clamp(*(pixel+1)) * 255;
			uint8_t b = clamp(*(pixel+2)) * 255;
			pixel += 4;

			uint8_t pixel[] = { b,g,r };
			file.write(reinterpret_cast<char*>(pixel), 3);
		}
		file.write(Padding, PaddingSize);
	}

	file.close();

#endif // BMP

#ifdef PPM
	std::ofstream file2("test.ppm", std::ios::binary);
	if (file2.is_open())
	{
		file2 << "P6\n";
		file2 << m_Width << ' ' << m_Height <<'\n';
		file2 << 255 << '\n';
		for (uint32_t i = 0; i < m_Width * m_Height * 4; i+=4)
		{
			file2 << uint8_t(Pixels[i]*255) << uint8_t(Pixels[i + 1] * 255) << uint8_t(Pixels[i + 2] * 255);
		}
	}
#endif // PPM

	delete[] Pixels;
}

uint8_t* Image::GetImageData() const
{
	uint8_t* ImageData = new uint8_t[m_Width * m_Height * 4];
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);

	return ImageData;
}
