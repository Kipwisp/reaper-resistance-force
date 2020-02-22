#ifndef ASTEROID_H
#define ASTEROID_H

#include "enemy.h"

class Asteroid : public Enemy
{
public:
	GLint damageTimer;
	GLuint sizeLevel;
	GLfloat rotateSpeed;

	Asteroid();
	Asteroid(glm::vec2 pos, glm::vec2 vel, GLint size);
	void move(GLfloat dt, GLuint width);
};

#endif