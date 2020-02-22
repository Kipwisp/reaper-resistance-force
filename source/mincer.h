#ifndef MINCER_H
#define MINCER_H

#include "enemy.h"

class Mincer : public Enemy
{
public:
	Mincer();
	void updateSprite();
	Missile* fireMissile(GLfloat dt);
};

#endif

