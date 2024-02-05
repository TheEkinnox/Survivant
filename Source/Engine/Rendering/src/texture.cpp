#include <iostream>
#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "SurvivantRendering/Resources/texture.h"

Texture::Texture() :
	m_path("")
	, m_width(0)
	, m_height(0)
	, m_numchannels(0)
	, m_pixels(nullptr)
{

}

Texture::~Texture()
{
	stbi_image_free(m_pixels);
	m_pixels = nullptr;
}

Texture::Texture(const Texture& other) : m_textureID(0), m_width(0), m_height(0), m_numchannels(0), m_pixels(nullptr),
m_minFilter(TextureMinFilter::Linear), m_magFilter(TextureMagFilter::Linear), m_wrapS(TextureWrapMode::Repeat),
m_wrapT(TextureWrapMode::Repeat), m_generateMipmaps(false)
{
	// Copy the data from the source texture
	if (other.m_textureID != 0) {
		glGenTextures(1, &m_textureID);
		Bind();

		// Copy texture parameters
		SetFiltering(m_minFilter, m_magFilter);
		SetWrapping(m_wrapS, m_wrapT);

		// Copy texture data if necessary
		if (other.m_pixels != nullptr) {
			m_width = other.m_width;
			m_height = other.m_height;
			m_numchannels = other.m_numchannels;
			m_pixels = new unsigned char[m_width * m_height * m_numchannels];
			std::copy(other.m_pixels, other.m_pixels + m_width * m_height * m_numchannels, m_pixels);

			// Generate mipmaps if needed
			if (other.m_generateMipmaps) {
				GenerateMipmaps();
			}
		}
		Unbind();
	}
}

void Texture::LoadTexture()
{
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	GLenum dataFormat = 0;
	if (m_numchannels == 4)
	{
		dataFormat = GL_RGBA;
	}
	else if (m_numchannels == 3)
	{
		dataFormat = GL_RGB;
	}

	if (m_pixels && dataFormat == 0)
	{
		std::runtime_error("Texture format not supported - num channels: {}"  + std::to_string(m_numchannels));
	}

	if (m_pixels && dataFormat != 0)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, m_pixels);
		std::cerr << "Loaded {}-channel texture: {}" << m_numchannels << " " << m_path.c_str() << std::endl;
	}
	else
	{
		float pixels[] = {
			1.f, 0.f, 1.f,			1.f,1.f,1.f,         1.f,0.f,1.f,          1.f,1.f,1.f,
			1.f,1.f,1.f,			1.f,0.f,1.f			 ,1.f,1.f,1.f			,1.f,1.f,1.f,
			1.f, 0.f, 1.f,			1.f,1.f,1.f,         1.f,0.f,1.f,          1.f,1.f,1.f,
			1.f,1.f,1.f,			1.f,0.f,1.f			 ,1.f,1.f,1.f			,1.f,1.f,1.f
		};

		m_width = 4;
		m_height = 4;
		m_numchannels = 3;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_FLOAT, pixels);
		std::cerr << "Unable to load texture: {} " << m_path.c_str() << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetFiltering(TextureMinFilter p_minFilter, TextureMagFilter p_magFilter)
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(p_minFilter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(p_magFilter));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrapping(TextureWrapMode p_wrapS, TextureWrapMode p_wrapT)
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(p_wrapS));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(p_wrapT));
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::GenerateMipmaps()
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::EnableMipmaps(bool p_generateMipMaps)
{
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	if (p_generateMipMaps)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::GetWidth() const
{
	return m_width;
}

int Texture::GetHeight() const
{
	return m_height;
}

uint32_t Texture::GetNumberOfChannels() const
{
	return m_numchannels;
}

void Texture::SetActiveTextureUnit(GLenum p_textureUnit)
{
	glActiveTexture(p_textureUnit);
}

unsigned char* Texture::LoadFile(const char* p_filepath, int& p_width, int& p_height, GLenum& p_format)
{
	int channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(p_filepath, &p_width, &p_height, &channels, 0);

	if (!data)
	{
		std::cerr << "Failed to load texture: " << p_filepath << std::endl;
		return nullptr;
	}

	// Determine the OpenGL format based on the number of channels
	if (channels == 1)
	{
		p_format = GL_RED;
	}
	else if (channels == 3)
	{
		p_format = GL_RGB;
	}
	else if (channels == 4)
	{
		p_format = GL_RGBA;
	}
	else
	{
		std::cerr << "Unsupported number of channels: " << channels << std::endl;
		stbi_image_free(data);
		return nullptr;
	}

	return data;
}

void Texture::CheckGLErrors(const std::string& p_location)
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error at " << p_location << ":" << std::endl;
	}
}

bool Texture::operator==(const Texture& other) const
{
	return m_textureID == other.m_textureID;
}


bool Texture::operator!=(const Texture& other) const
{
	return !(*this == other);
}

void Texture::FreeImageData(unsigned char* p_data)
{
	stbi_image_free(p_data);
}