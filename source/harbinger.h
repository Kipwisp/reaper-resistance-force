#ifndef HARBINGER_H
#define HARBINGER_H

#include "enemy.h"

class Harbinger : public Enemy
{
public:
	Harbinger();
	void updateSprite();
	Missile* fireMissile(GLfloat dt);
};

#endif