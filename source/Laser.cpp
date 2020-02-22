#include "laser.h"

Laser::Laser(glm::vec2 pos, glm::vec2 vel, GLuint damage, GLboolean fromPlayer) : Projectile(pos, vel, damage, fromPlayer)
{
	this->sprite = ResourceManager::getSpriteSheet("projectiles");
	this->size = glm::vec2(sprite.spriteWidth, sprite.spriteHeight);
	if (fromPlayer) // coming from player
	{
		this->frame = glm::vec2(0, 0);
	}
	else // coming from enemy
	{
		this->frame = glm::vec2(0, 1);
	}
}
