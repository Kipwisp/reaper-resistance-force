#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Instantiate static variables
std::map<std::string, Texture2D> ResourceManager::textures;
std::map<std::string, SpriteSheet> ResourceManager::spriteSheets;
std::map<std::string, Shader> ResourceManager::shaders;

Shader ResourceManager::loadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, const GLchar *name)
{
	shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile, name);
	return shaders[name];
}

Shader ResourceManager::getShader(std::string name)
{
	return shaders[name];
}

Texture2D ResourceManager::loadTexture(const GLchar *file, GLboolean alpha, std::string name)
{
	textures[name] = loadTextureFromFile(file, alpha);
	return textures[name];
}

SpriteSheet ResourceManager::loadSpriteSheet(const GLchar *file, GLboolean alpha, std::string name, GLuint spriteWidth, GLuint spriteHeight, GLuint rows, GLuint columns)
{
	spriteSheets[name] = loadSpriteSheetFromFile(file, alpha, spriteWidth, spriteHeight, rows, columns);
	return spriteSheets[name];
}

Texture2D ResourceManager::getTexture(std::string name)
{
	return textures[name];
}

SpriteSheet ResourceManager::getSpriteSheet(std::string name)
{
	return spriteSheets[name];
}

void ResourceManager::clear()
{
	// (Properly) delete all shaders	
	for (auto iter : shaders)
		glDeleteProgram(iter.second.ID);
	// (Properly) delete all textures
	for (auto iter : textures)
		glDeleteTextures(1, &iter.second.ID);

	for (auto iter : spriteSheets)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, const GLchar *name)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		// Open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// If geometry shader path is present, also load a geometry shader
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = geometryCode.c_str();
	// create shader object
	Shader shader;
	shader.compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr, name);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const GLchar *file, GLboolean alpha)
{
	// Create Texture object
	Texture2D texture;
	if (alpha)
	{
		texture.internalFormat = GL_RGBA;
		texture.imageFormat = GL_RGBA;
	}
	// Load image
	int width, height, nrChannels;
	unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);
	// generate texture
	texture.generate(width, height, image);
	// free image data
	stbi_image_free(image);
	return texture;
}

SpriteSheet ResourceManager::loadSpriteSheetFromFile(const GLchar *file, GLboolean alpha, GLuint spriteHeight, GLuint spriteWidth, GLuint rows, GLuint columns)
{
	// Create Spritesheet object
	SpriteSheet sprite(spriteHeight, spriteWidth, rows, columns);
	if (alpha)
	{
		sprite.internalFormat = GL_RGBA;
		sprite.imageFormat = GL_RGBA;
	}
	// Load image
	int width, height, nrChannels;
	unsigned char* image = stbi_load(file, &width, &height, &nrChannels, 0);
	// generate texture
	sprite.generate(width, height, image);
	// free image data
	stbi_image_free(image);
	return sprite;
}