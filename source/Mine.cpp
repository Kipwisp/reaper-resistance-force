#include "mine.h"

Mine::Mine(glm::vec2 pos, GLuint damage, GLboolean fromPlayer) : Missile(pos, fromPlayer ? glm::vec2(0, -25) : glm::vec2(0, 25), damage, fromPlayer)
{
	this->isMine = GL_TRUE;
	if (fromPlayer) // coming from player
	{
		this->frame = glm::vec2(1, 0);
	}
	else // coming from enemy
	{
		this->frame = glm::vec2(1, 1);
	}
}

void Mine::move(GLfloat dt, GLuint width)
{
	this->position.x += this->velocity.x * dt;
	this->position.y += this->velocity.y * dt;

	this->explosion->position = this->position - glm::vec2(this->diameter / 2) + glm::vec2(this->size.x / 2), glm::vec2(0, 0); // update missile's explosion's position
	this->rotation += 5 * dt;

	this->velocity = this->fromPlayer ? glm::vec2(0, -25) : glm::vec2(0, 25); // reset velocity just in case no targets are in range
																															   // wrap around screen
	if (this->position.x > width)
		this->position.x = -this->size.x;
	else if (this->position.x < -this->size.x)
		this->position.x = width;
}

void Mine::seek(GameObject obj)
{
	GLfloat vecX = obj.position.x - this->position.x;
	GLfloat vecY = obj.position.y - this->position.y;
	GLfloat magnitude = sqrt(pow(vecX, 2.0f)) + sqrt(pow(vecY, 2.0f));  // use unit vectors to get direction for mine's direction

	this->velocity.x = vecX/magnitude * 250;
	this->velocity.y = vecY/magnitude * 250;
}