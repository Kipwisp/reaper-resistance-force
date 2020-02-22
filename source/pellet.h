#ifndef PELLET_H
#define PELLET_H

#include "projectile.h"

class Pellet : public Projectile
{
public:
	Pellet(glm::vec2 pos, glm::vec2 vel, GLuint damage, GLboolean fromPlayer);
};

#endif
