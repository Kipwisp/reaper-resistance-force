#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "texture.h"
#include "game_object.h"
#include "enemy.h"


// Represents a single particle and its state
struct Particle {
	glm::vec2 position, velocity;
	glm::vec4 color;
	GLfloat life;
	GLfloat rotate;
	GLint rVel;
	GLfloat scale;
	glm::vec2 frame;

	Particle() : position(0.0f), velocity(0.0f, 3.0f), color(1.0f), life(0.0f) { }
};

class ParticleGenerator
{
public:
	// Constructor
	ParticleGenerator(Shader shader, SpriteSheet sheet, GLuint amount, GLchar type);
	// Update all particles
	void update(GameObject &object, GLuint newParticles, glm::vec2 frame = glm::vec2(0,0), GLboolean fromEnemy = GL_FALSE);
	void update(GLfloat dt);
	// Render all particles
	void draw();
	void removeParticles();
private:
	// State
	std::vector<Particle> particles;
	GLuint amount;
	GLchar type;
	// Render state
	Shader shader;
	SpriteSheet sheet;
	GLuint VAO;
	GLuint VBO;
	// Initializes buffer and vertex attributes
	void init();
	// Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	GLuint firstUnusedParticle();
	// Respawns particle
	void respawnParticle(Particle &particle, GameObject &object, glm::vec2 frame, GLboolean fromEnemy);
};

#endif
