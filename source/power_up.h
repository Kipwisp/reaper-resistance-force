#ifndef POWERUP_H
#define POWERUP_H

#include "player.h"

class PowerUp : public GameObject
{
public:
	GLchar type;
	GameObject orb; 
	GLfloat flux;
	std::string name;
	PowerUp(glm::vec2 pos, GLchar type, std::string name);
	void activateEffect(Player* player);
	void move(GLfloat dt, GLuint width);
};

#endif