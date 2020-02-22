#ifndef VANGUARD_H
#define VANGUARD_H

#include "enemy.h"

class Vanguard : public Enemy
{
public:
	Vanguard();
	void updateSprite();
	std::list<Projectile*> fire(GLfloat dt);
};

#endif

