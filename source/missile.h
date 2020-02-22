#ifndef MISSILE_H
#define MISSILE_H

#include "projectile.h"
#include <irrKlang/irrKlang.h>
using namespace irrklang;

class Missile : public Projectile
{
public:
	GLint diameter; // explosion diameter
	GLboolean explode;
	GLboolean secondLoop;
	GameObject *explosion;
	GLboolean isMine;
	ISound* snd; // missile sound effect
	Missile(glm::vec2 pos, glm::vec2 vel, GLuint damage, GLboolean fromPlayer);
	~Missile();
	virtual void move(GLfloat dt, GLuint width);
	virtual void seek(GameObject obj);  // for homing missiles
};

#endif