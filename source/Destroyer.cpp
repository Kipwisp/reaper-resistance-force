#include "destroyer.h"

Destroyer::Destroyer() : Enemy(2, 100, 2, 20.0f, 50, "destroyer", glm::vec2(1, 0), GL_FALSE)
{

}

void Destroyer::updateSprite()
{
	if (this->bulletTimer > 5.0f)
		this->frame = glm::vec2(0, 0);
	else
		this->frame = glm::vec2(0, 1);
}