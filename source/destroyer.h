#ifndef DESTROYER_H
#define DESTROYER_H

#include "enemy.h"

class Destroyer : public Enemy
{
public:
	Destroyer();
	void updateSprite();
};

#endif