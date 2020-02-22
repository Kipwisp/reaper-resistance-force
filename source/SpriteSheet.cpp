#include "sprite_sheet.h"

SpriteSheet::SpriteSheet() : Texture2D()
{

}

SpriteSheet::SpriteSheet(GLuint spriteWidth, GLuint spriteHeight, GLuint rows, GLuint columns) : Texture2D(), spriteWidth(spriteWidth), spriteHeight(spriteHeight), rows(rows), columns(columns)
{

}