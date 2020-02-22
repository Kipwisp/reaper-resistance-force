#include "info_text.h"

InfoText::InfoText(glm::vec2 pos, std::string text, GLfloat scale, glm::vec4 color) : position(pos), text(text), color(color), scale(scale), life(15)
{
}

void InfoText::draw(TextRenderer &renderer)
{
	renderer.renderText(this->text, this->position.x, this->position.y, this->scale, this->color);
}