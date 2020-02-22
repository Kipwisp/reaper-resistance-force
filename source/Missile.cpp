#include "missile.h"

Missile::Missile(glm::vec2 pos, glm::vec2 vel, GLuint damage, GLboolean fromPlayer) : Projectile(pos, vel, damage, fromPlayer), explode(GL_FALSE), diameter(300), isMine(GL_FALSE), secondLoop(GL_FALSE)
{
	this->sprite = ResourceManager::getSpriteSheet("missiles");
	this->size = glm::vec2(sprite.spriteWidth, sprite.spriteHeight);
	this->explosion = new GameObject();
	this->explosion->size = glm::vec2(this->diameter);
	if (fromPlayer) // coming from player
	{
		this->frame = glm::vec2(0, 0);
	}
	else // coming from enemy
	{
		this->frame = glm::vec2(0, 1);
	}
}

Missile::~Missile()
{
	delete this->explosion;
}

void Missile::move(GLfloat dt, GLuint width)
{
	this->position.x += this->velocity.x * dt;
	this->position.y += this->velocity.y * dt;

	this->explosion->position = this->position - glm::vec2(this->diameter / 2) + glm::vec2(this->size.x / 2), glm::vec2(0, 0); // update missile's explosion's position

	// wrap around screen
	if (this->position.x > width)
		this->position.x = -this->size.x;
	else if (this->position.x < -this->size.x)
		this->position.x = width;
}

void Missile::seek(GameObject obj) // for homing missiles if wanted
{

}
