#include "pellet.h"

Pellet::Pellet(glm::vec2 pos, glm::vec2 vel, GLuint damage, GLboolean fromPlayer) : Projectile(pos, vel, damage, fromPlayer)
{
	this->sprite = ResourceManager::getSpriteSheet("pellet");
	this->size = glm::vec2(sprite.spriteWidth, sprite.spriteHeight);
	if (fromPlayer) // coming from player
	{
		this->frame = glm::vec2(2, 0);
	}
	else // coming from enemy
	{
		this->frame = glm::vec2(2, 1);
	}
}
