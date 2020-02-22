#include "power_up.h"

PowerUp::PowerUp(glm::vec2 pos, GLchar type, std::string name) : GameObject(pos, glm::vec2(35, 35), "power_ups"), type(type), name(name), flux(1.0f)
{
	this->velocity = glm::vec2(0, 25);
	this->orb = GameObject(pos, glm::vec2(25, 25), "power_ups");

	switch (type)
	{
	case 'h':  // health
		this->frame = glm::vec2(0, 0);
		this->orb.frame = glm::vec2(1, 0);
		break;
	case 't':  // triple lasers
		this->frame = glm::vec2(0, 1);
		this->orb.frame = glm::vec2(1, 1);
		break;
	case 'b':  // beam
		this->frame = glm::vec2(0, 2);
		this->orb.frame = glm::vec2(1, 2);
		break;
	case 's':  // shield
		this->frame = glm::vec2(0, 3);
		this->orb.frame = glm::vec2(1, 3);
		break;
	case 'm':  // 2x multiplier
		this->frame = glm::vec2(0, 4);
		this->orb.frame = glm::vec2(1, 4);
		break;
	case 'f':  // faster firing
		this->frame = glm::vec2(0, 5);
		this->orb.frame = glm::vec2(1, 5);
		break;
	case 'd':  // x2 damage
		this->frame = glm::vec2(0, 6);
		this->orb.frame = glm::vec2(1, 6);
		break;
	case 'i':  // mine
		this->frame = glm::vec2(0, 7);
		this->orb.frame = glm::vec2(1, 7);
		break;
	case 'r':  // missle
		this->frame = glm::vec2(0, 8);
		this->orb.frame = glm::vec2(1, 8);
		break;
	case 'p':  // pellet shot
		this->frame = glm::vec2(0, 9);
		this->orb.frame = glm::vec2(1, 9);
		break;
	}
}

void PowerUp::activateEffect(Player *player)
{
	if (this->type == 't' || this->type == 'b' || this->type == 'p') // make sure conflicting power ups aren't active at the same time
	{
		player->tripleLasers = GL_FALSE;
		player->beams = GL_FALSE;
		player->pelletShot = GL_FALSE;
	}

	switch (this->type)
	{
	case 'h':  // health
		player->health += 50;
		if (player->health > player->maxHealth)
			player->health = player->maxHealth;
		break;
	case 't':  // triple laser
		player->tripleLasers = GL_TRUE;
		player->powerTimer[0] = 500;
		break;
	case 'b':  // beam
		player->beams = GL_TRUE;
		player->powerTimer[1] = 500;
		break;
	case 's':  // shield
		player->shield = GL_TRUE;
		player->powerTimer[2] = 500;
		break;
	case 'm':  // x2 multiplier
		player->multiplier = 2;
		player->powerTimer[3] = 500;
		break;
	case 'f':  // faster firing
		player->reloadTime = 2;
		player->powerTimer[4] = 500;
		break;
	case 'd':  // x2 damage
		player->damage = 2;
		player->color = glm::vec4(1.0f, 0.2f, 0.2f, 1.0f);  // turn the ship red to indicate extra damage
		player->powerTimer[5] = 500;
		break;
	case 'i':  // mine
		player->mines += 1;
		break;
	case 'r':  // missile
		player->missiles += 1;
		break;
	case 'p':  // pellet shot
		player->pelletShot = GL_TRUE;
		player->powerTimer[6] = 500;
		break;
	}

}

void PowerUp::move(GLfloat dt, GLuint width)
{
	this->position.x += this->velocity.x * dt;
	this->position.y += this->velocity.y * dt;

	this->orb.position = this->position;
	if (this->orb.color.w >= 1.0f)
		this->flux = -1;
	else if (this->orb.color.w <= -1.0f)
		this->flux = 1;

	this->orb.color += this->flux * glm::vec4(0.0f, 0.0f, 0.0f, 3.0f * dt);

	// wrap around screen
	if (this->position.x > width)
		this->position.x = -this->size.x;
	else if (this->position.x < -this->size.x)
		this->position.x = width;
}