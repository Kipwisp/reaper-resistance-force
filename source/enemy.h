#ifndef ENEMY_H
#define ENEMY_H

#include "game_object.h"
#include "laser.h"
#include "beam.h"
#include "mine.h"
#include "pellet.h"
#include <GLFW/glfw3.h>
#include <list>

class Enemy : public GameObject
{
public:
	GLfloat bulletTimer;
	GLint health;
	GLint maxHealth; // for smoke particles
	GLuint maxVel;
	GLuint maxDamage;
	GLfloat reloadTime;
	GLuint points;
	GLboolean trail;
	GLboolean hasBeam;
	GLboolean hasMissile;
	glm::vec2 fragmentFrame;
	Beam* beam;

	Enemy(GLuint health, GLuint maxVel, GLuint maxDamage, GLfloat reloadTime, GLuint points, std::string spriteName, glm::vec2 fragmentFrame, GLboolean trail);
	Enemy();
	virtual std::list<Projectile*> fire(GLfloat dt);
	virtual Beam* fireBeam(GLfloat dt);
	virtual Missile* fireMissile(GLfloat dt);
	virtual void updateSprite();
};

#endif