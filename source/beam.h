#ifndef BEAM_H
#define BEAM_H

#include "projectile.h"
#include <irrKlang/irrKlang.h>
using namespace irrklang;

class Beam : public Projectile
{
public:
	GameObject* obj; // who is firing the beam
	GLboolean active;
	GLint damageTimer;
	ISound* snd; // beam sound effect
	Beam(glm::vec2 pos, GLuint damage, GLboolean fromPlayer, GameObject* obj);
	void update(GLfloat dt);
};

#endif
