#include "reaver.h"

Reaver::Reaver() : Enemy(3, 50, 2, 50.0f, 250, "reaver", glm::vec2(4, 0), GL_TRUE), beamTimer(25), firingBeam(GL_FALSE)
{
	this->hasBeam = GL_TRUE;
}

void Reaver::updateSprite()
{
	if (this->bulletTimer <= 50)
		this->frame.y = 0;
	if (this->bulletTimer <= 35)
		this->frame.y = 1;
	if (this->bulletTimer <= 25)
		this->frame.y = 2;
	if (this->bulletTimer <= 15)
		this->frame.y = 3;

	if (this->health == 1)
		this->frame.x = 1;
}

Beam* Reaver::fireBeam(GLfloat dt)
{
	if (this->bulletTimer > 0 && !this->firingBeam)
		this->bulletTimer -= 10 * dt;

	if (this->bulletTimer <= 0 && rand() % 60 == 0 && !this->firingBeam) // 1 in 60 chance of firing if reloaded
	{
		this->beam = new Beam(glm::vec2(this->position.x + this->size.x / 2 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, this->position.y + this->size.y), this->maxDamage, GL_FALSE, this);
		this->bulletTimer = this->reloadTime;
		this->firingBeam = GL_TRUE;
		return this->beam;
	}

	if (this->firingBeam)
		beamTimer -= 10 * dt;

	if (beamTimer <= 0)
	{
		this->beamTimer = 25;
		this->beam->active = GL_FALSE;
		this->firingBeam = GL_FALSE;
	}

	return nullptr;
}