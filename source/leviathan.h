#ifndef LEVIATHAN_H
#define LEVIATHAN_H

#include <ctime>
#include "enemy.h"

class Leviathan : public Enemy
{
public:
	GLint pCount; // keep track off lasers fired off
	clock_t timeLast;
	clock_t timeNext;
	Leviathan();
	void updateSprite();
	std::list<Projectile*> fire(GLfloat dt);
};

#endif
