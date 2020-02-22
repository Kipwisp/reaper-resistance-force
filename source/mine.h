#ifndef MINE_H
#define MINE_H

#include "missile.h"

class Mine : public Missile
{
public:
	Mine(glm::vec2 pos, GLuint damage, GLboolean fromPlayer);
	void move(GLfloat dt, GLuint width);
	void seek(GameObject obj);
};

#endif