#ifndef SPRITE_SHEET
#define SPRITE_SHEET

#include <glad/glad.h>
#include "texture.h"

class SpriteSheet : public Texture2D
{
public:
	GLuint spriteWidth;
	GLuint spriteHeight;
	GLuint rows, columns;
	SpriteSheet();
	SpriteSheet(GLuint spriteWidth, GLuint spriteHeight, GLuint rows, GLuint columns);
};
#endif
