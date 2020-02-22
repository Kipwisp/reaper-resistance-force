#include "harbinger.h"

Harbinger::Harbinger() : Enemy(6, 30, 10, 40.0f, 500, "harbinger", glm::vec2(6, 0), GL_TRUE)
{
	this->hasMissile = GL_TRUE;
}

void Harbinger::updateSprite()
{
	if (this->health == 2)
		this->frame = glm::vec2(0, 1);
	else if (this->health == 1)
		this->frame = glm::vec2(0, 2);
}

Missile* Harbinger::fireMissile(GLfloat dt)
{
	if (this->bulletTimer > 0)
		this->bulletTimer -= 10 * dt;

	if (this->bulletTimer <= 0 && rand() % 60 == 0) // 1 in 60 chance of firing if reloaded
	{
		this->bulletTimer = this->reloadTime;
		return new Missile(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, this->position.y), glm::vec2(0, 500), this->maxDamage, GL_FALSE);
	}

	return nullptr;
}
