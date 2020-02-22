#ifndef REAVER_H
#define REAVER_H

#include "enemy.h"
#include "beam.h"

class Reaver : public Enemy
{
public:
	GLfloat beamTimer;
	GLboolean firingBeam;
	Reaver();
	void updateSprite();
	Beam* fireBeam(GLfloat dt);
};

#endif
