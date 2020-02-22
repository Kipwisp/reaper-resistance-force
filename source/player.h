#ifndef PLAYER_H
#define PLAYER_H

#include "game_object.h"
#include "beam.h"
#include <irrKlang/irrKlang.h>
using namespace irrklang;

class Player : public GameObject
{
public:
	GLfloat bulletTimer;
	GLfloat powerTimer[7];
	GLint health;
	GLfloat maxHealth;
	GLint frame;
	GLfloat reloadTime;
	GLfloat displayHealth; // for health bar

	// powerups
	GLboolean firingBeam;
	GLboolean tripleLasers;
	GLboolean beams;
	GLboolean shield;
	GLboolean pelletShot;
	GLint multiplier;
	GLint damage;
	GLint missiles;
	GLint mines;


	Beam* beam;
	Player(GLuint width, GLuint height);
	Player();
	void move(GLfloat dt, GLuint width);
	void updateSprite();
	Beam* fireBeam();
};

#endif
