#ifndef CRUISER_H
#define CRUISER_H

#include "enemy.h"

class Cruiser : public Enemy
{
public:
	Cruiser();
	void updateSprite();
	std::list<Projectile*> fire(GLfloat dt);
};

#endif