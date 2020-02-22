#include "enemy.h"
#include <iostream>

Enemy::Enemy(GLuint health, GLuint maxVel, GLuint maxDamage, GLfloat reloadTime, GLuint points, std::string spriteName, glm::vec2 fragmentFrame, GLboolean trail) :
	GameObject(glm::vec2(0, 0), glm::vec2(0, 0), spriteName), bulletTimer(25.0f), health(health), maxHealth(health), maxVel(maxVel), maxDamage(maxDamage), reloadTime(reloadTime), points(points), fragmentFrame(fragmentFrame), trail(trail), hasBeam(GL_FALSE), hasMissile(GL_FALSE)
{
	this->velocity.x = rand() % 2 == 0 ? rand() % 100 + 25 : -(rand() % 100) - 25; // have random velocities going left and right
	this->velocity.y = rand() % (maxVel - 25) + 25; // have random velocity going down
	
	this->position.x = rand() % (700 + this->sprite.spriteWidth) - this->sprite.spriteWidth;
	this->position.y = (GLint)this->sprite.spriteHeight*-1; // spawn off screen at top
}

Enemy::Enemy()
{

}

std::list<Projectile*> Enemy::fire(GLfloat dt) // fire a laser
{
	std::list<Projectile*> projectiles;

	if (this->bulletTimer > 0)
		this->bulletTimer -= 10 * dt;

	if (this->bulletTimer <= 0 && rand() % 60 == 0) // 1 in 60 chance of firing if reloaded
	{
		projectiles.push_front(new Laser(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, this->position.y + this->size.y / 2), glm::vec2(0, 700), this->maxDamage - rand() % (this->maxDamage), GL_FALSE));
		this->bulletTimer = this->reloadTime;
	}
	return projectiles;
}


void Enemy::updateSprite() 
{

}

Beam* Enemy::fireBeam(GLfloat dt)
{
	return nullptr;
}

Missile* Enemy::fireMissile(GLfloat dt)
{
	return nullptr;
}