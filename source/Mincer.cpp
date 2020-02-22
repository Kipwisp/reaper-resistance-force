#include "mincer.h"

Mincer::Mincer() : Enemy(2, 150, 10, 75.0f, 200, "mincer", glm::vec2(5, 0), GL_FALSE)
{
	this->hasMissile = GL_TRUE;
	this->velocity.y = rand() % (maxVel - 100) + 100; // mincers have a higher min velocity
}

void Mincer::updateSprite()
{
	if (this->health == 1)
		this->frame = glm::vec2(0, 1);
}

Missile* Mincer::fireMissile(GLfloat dt)
{
	if (this->bulletTimer > 0)
		this->bulletTimer -= 10 * dt;

	if (this->bulletTimer <= 0 && rand() % 60 == 0) // 1 in 60 chance of firing if reloaded
	{
		this->bulletTimer = this->reloadTime;
		return new Mine(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, this->position.y), this->maxDamage, GL_FALSE);
	}

	return nullptr;
}
