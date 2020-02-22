#include "leviathan.h"

Leviathan::Leviathan() : Enemy(4, 50, 4, 50.0f, 150, "leviathan", glm::vec2(2, 0), GL_TRUE), pCount(0), timeLast(clock())
{
}

void Leviathan::updateSprite()
{
	if (this->health == 2)
		this->frame = glm::vec2(0, 1);
	if (this->health == 1)
		this->frame = glm::vec2(0, 2);
}

std::list<Projectile*> Leviathan::fire(GLfloat dt)
{
	std::list<Projectile*> projectiles;

	if (this->bulletTimer > 0)
		this->bulletTimer -= 10 * dt;

	if (this->bulletTimer <= 0 && rand() % 60 == 0 && this->pCount == 0) // 1 in 60 chance of firing if reloaded
		this->pCount = 3;

	if (this->pCount > 0)
	{
		timeNext = clock();  // get the current time
		if (GLdouble(timeNext - timeLast) >= 200)  // at least 200 ms between each shot being fired
		{
			projectiles.push_front(new Laser(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, this->position.y + this->size.y / 2), glm::vec2(0, 700), this->maxDamage - rand() % 2, GL_FALSE));
			this->pCount -= 1;

			if (this->pCount == 0)
				this->bulletTimer = this->reloadTime;

			timeLast = timeNext;  // save the current time to calculate the timing for the next shot
		}
	}

	return projectiles;
}