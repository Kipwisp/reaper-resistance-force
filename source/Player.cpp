#include "player.h"

Player::Player()
	: GameObject(glm::vec2(-1000, -1000), glm::vec2(48, 44), "ship"), displayHealth(0.0f), health(0.0f), maxHealth(100.0f)
{

}

Player::Player(GLuint width, GLuint height)
	: GameObject(glm::vec2(width / 2 - 24, height / 2 - 22), glm::vec2(48, 44), "ship"), bulletTimer(0.0f), displayHealth(100.0f), reloadTime(3), frame(0), firingBeam(GL_FALSE), maxHealth(100), health(100.0f), multiplier(1), damage(1), missiles(3), mines(3)
{
}

void Player::move(GLfloat dt, GLuint width)
{
	this->position.x += this->velocity.x * dt;
	this->position.y += (this->velocity.y+25) * dt; // constant downward motion

	// wrap around screen
	if (this->position.x > width)
		this->position.x = -this->size.x;
	else if (this->position.x < -this->size.x)
		this->position.x = width;
}

Beam* Player::fireBeam()
{
	this->beam = new Beam(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, this->position.y - 900), this->damage, GL_TRUE, this);
	return this->beam;
}

void Player::updateSprite()
{
	if (this->health > 50)
		this->frame = 0;
	else if (this->health > 25)
		this->frame = 1;
	else
		this->frame = 2;
}