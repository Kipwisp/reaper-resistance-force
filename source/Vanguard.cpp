#include "vanguard.h"

Vanguard::Vanguard() : Enemy(3, 75, 2, 25.0f, 250, "vanguard", glm::vec2(3, 0), GL_TRUE)
{
}

void Vanguard::updateSprite()
{
	if (this->health == 2)
		this->frame = glm::vec2(0, 1);
	if (this->health == 1)
		this->frame = glm::vec2(0, 2);
}

std::list<Projectile*> Vanguard::fire(GLfloat dt)
{
	std::list<Projectile*> projectiles;

	if (this->bulletTimer > 0)
		this->bulletTimer -= 10 * dt;

	if (this->bulletTimer <= 0 && rand() % 20 == 0) // 1 in 20 chance of firing if reloaded
	{
		projectiles.push_front(new Laser(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, this->position.y + this->size.y / 2), glm::vec2(0, 700), this->maxDamage - rand() % 2, GL_FALSE));
		projectiles.push_front(new Laser(glm::vec2(this->position.x + 10 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, this->position.y + 10), glm::vec2(0, 700), this->maxDamage - rand() % 2, GL_FALSE));
		projectiles.push_front(new Laser(glm::vec2(this->position.x + this->size.x - 10 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, this->position.y + 10), glm::vec2(0, 700), this->maxDamage - rand() % 2, GL_FALSE));
		this->bulletTimer = this->reloadTime;
	}

	return projectiles;
}