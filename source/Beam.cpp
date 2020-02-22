#include "beam.h"

Beam::Beam(glm::vec2 pos, GLuint damage, GLboolean fromPlayer, GameObject* obj) : Projectile(pos, glm::vec2(0.0f), damage, fromPlayer), obj(obj), active(GL_TRUE), damageTimer(20)
{
	this->sprite = ResourceManager::getSpriteSheet("projectiles");
	this->size = glm::vec2(sprite.spriteWidth, 900);
	this->color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
	if (fromPlayer) // coming from player
	{
		this->frame = glm::vec2(1, 0);
	}
	else // coming from enemy
	{
		this->frame = glm::vec2(1, 1);
	}
}

void Beam::update(GLfloat dt)
{
	if (this->fromPlayer)
		this->position = this->obj->position + glm::vec2(this->obj->size.x / 2 - this->size.x / 2, -(GLint)this->size.y);
	else
		this->position = this->obj->position + glm::vec2(this->obj->size.x / 2 - this->size.x / 2, this->obj->size.y - 10);
	this->damageTimer -= 10 * dt;
	if (this->color.a < 2.0f)
		this->color += glm::vec4(0.0f, 0.0f, 0.0f, 5.0f*dt);
}