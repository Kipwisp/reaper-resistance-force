#include "projectile.h"
#include "resource_manager.h"

Projectile::Projectile(glm::vec2 pos, glm::vec2 vel, GLuint damage, GLboolean fromPlayer) : GameObject(), damage(damage)
{
	this->position = pos;
	this->velocity = vel;
	this->fromPlayer = fromPlayer;
}