#ifndef LASER_H
#define LASER_H

#include "projectile.h"

class Laser : public Projectile
{
public:
	Laser(glm::vec2 pos, glm::vec2 vel, GLuint damage, GLboolean fromPlayer);
};

#endif