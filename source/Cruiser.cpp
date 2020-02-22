#include "cruiser.h"

Cruiser::Cruiser() : Enemy(4, 75, 4, 75.0f, 250, "cruiser", glm::vec2(7, 0), GL_TRUE)
{
}

void Cruiser::updateSprite()
{
	if (this->health == 2)
		this->frame = glm::vec2(0, 1);
	else if (this->health == 1)
		this->frame = glm::vec2(0, 2);
}

std::list<Projectile*> Cruiser::fire(GLfloat dt)
{
	std::list<Projectile*> projectiles;

	if (this->bulletTimer > 0)
		this->bulletTimer -= 10 * dt;

	if (this->bulletTimer <= 0 && rand() % 60 == 0) // 1 in 60 chance of firing if reloaded
	{
		projectiles.push_front(new Pellet(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, this->position.y), glm::vec2(0, 700), this->maxDamage - rand() % (this->maxDamage - 1), GL_FALSE));
		projectiles.push_front(new Pellet(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, this->position.y), glm::vec2(-250, 700), this->maxDamage - rand() % (this->maxDamage - 1), GL_FALSE));
		projectiles.push_front(new Pellet(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, this->position.y), glm::vec2(-500, 700), this->maxDamage - rand() % (this->maxDamage - 1), GL_FALSE));
		projectiles.push_front(new Pellet(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, this->position.y), glm::vec2(250, 700), this->maxDamage - rand() % (this->maxDamage - 1), GL_FALSE));
		projectiles.push_front(new Pellet(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, this->position.y), glm::vec2(500, 700), this->maxDamage - rand() % (this->maxDamage - 1), GL_FALSE));
		this->bulletTimer = this->reloadTime;
	}
	return projectiles;
}
