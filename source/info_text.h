#ifndef INFO_TEXT_H
#define INFO_TEXT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "text_renderer.h"


class InfoText
{
public:
	std::string text;
	glm::vec2 position;
	glm::vec4 color;
	GLfloat scale;
	GLfloat life;
	InfoText(glm::vec2 pos, std::string text, GLfloat scale, glm::vec4 color = glm::vec4(1.0f));
	void draw(TextRenderer &renderer);
};

#endif