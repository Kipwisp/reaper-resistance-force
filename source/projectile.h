#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "game_object.h"

class Projectile : public GameObject
{
public:
	GLboolean fromPlayer;
	GLuint damage;

	Projectile(glm::vec2 pos, glm::vec2 vel, GLuint damage, GLboolean fromPlayer);
};

#endif
