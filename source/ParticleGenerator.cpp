#include "particle_generator.h"
#include <iostream>

ParticleGenerator::ParticleGenerator(Shader shader, SpriteSheet sheet, GLuint amount, GLchar type)
	: shader(shader), sheet(sheet), amount(amount), type(type)
{
	this->init();
}

void ParticleGenerator::update(GameObject &object, GLuint newParticles, glm::vec2 frame, GLboolean fromEnemy) // adding new particle
{
	// Add new particles 
	for (GLuint i = 0; i < newParticles; ++i)
	{
		int unusedParticle = this->firstUnusedParticle();
		this->respawnParticle(this->particles[unusedParticle], object, frame, fromEnemy);
	}
}

void ParticleGenerator::update(GLfloat dt) // updating existing particles
{
	// Update all particles
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.life -= dt; // reduce life
		if (p.life > 0.0f)
			p.position += p.velocity * dt;

		if (this->type == 'f' || this->type == 'l') // fragment particles
			p.rotate += p.rVel*dt;  // rotate slowly

		if (this->type == 's') // smoke particles
		{
			p.scale += 5 * dt; // slowly get larger
			p.position.x -= (5 * dt)*sheet.spriteWidth / 2; // adjust position due to scale increase
		}
	}
}

// Render all particles
void ParticleGenerator::draw()
{
	// Use additive blending to give it a 'glow' effect
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->shader.use();
	this->sheet.bind();
	for (Particle particle : this->particles)
	{
		if (particle.life > 0.0f)
		{
			GLfloat posX = (particle.frame.y * sheet.spriteWidth) / (GLfloat)sheet.width;
			GLfloat posY = (particle.frame.x * sheet.spriteHeight) / (GLfloat)sheet.height;

			GLfloat posX2 = posX + sheet.spriteWidth / (GLfloat)sheet.width;
			GLfloat posY2 = posY + sheet.spriteHeight / (GLfloat)sheet.height;

			GLfloat vertices[] = {   // cut out sprite from spritesheet
									 // Pos      // Tex
				0.0f, 1.0f, posX, posY2,
				1.0f, 0.0f, posX2, posY,
				0.0f, 0.0f, posX, posY,

				0.0f, 1.0f, posX, posY2,
				1.0f, 1.0f, posX2, posY2,
				1.0f, 0.0f, posX2, posY
			};

			// orginal vertices to switch back to
			GLfloat vertices2[] = {
				// Pos      // Tex
				0.0f, 1.0f, 0.0f, 1.0f,
				1.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,

				0.0f, 1.0f, 0.0f, 1.0f,
				1.0f, 1.0f, 1.0f, 1.0f,
				1.0f, 0.0f, 1.0f, 0.0f
			};

			glm::mat4 model;

				model = glm::translate(model, glm::vec3(particle.position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

			if (particle.rotate != 0)
			{
				model = glm::translate(model, glm::vec3(0.5f * sheet.spriteWidth, 0.5f * sheet.spriteHeight, 0.0f)); // Move origin of rotation to center of quad
				model = glm::rotate(model, particle.rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
				model = glm::translate(model, glm::vec3(-0.5f * sheet.spriteWidth, -0.5f * sheet.spriteHeight, 0.0f)); // Move origin back
			}

			model = glm::scale(model, glm::vec3(glm::vec2(sheet.spriteWidth, sheet.spriteHeight) * glm::vec2(particle.scale, particle.scale), 1.0f)); // Last scale

			this->shader.setMatrix4("model", model);

			this->shader.setVector2f("offset", particle.position);
			this->shader.setVector4f("color", particle.color);
			this->shader.setFloat("life", particle.life);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(0);
		}
	}
	// Don't forget to reset to default blending mode
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::removeParticles()
{
	for (GLuint i = 0; i < this->amount; ++i)
	{
		Particle &p = this->particles[i];
		p.life = 0;
	}
}

void ParticleGenerator::init()
{
	// Set up mesh and attribute properties
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &this->VBO);
	glBindVertexArray(this->VAO);
	// Fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_DYNAMIC_DRAW);
	// Set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create this->amount default particle instances
	for (GLuint i = 0; i < this->amount; ++i)
		this->particles.push_back(Particle());
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
	// First search from last used particle, this will usually return almost instantly
	for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
		if (this->particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// Otherwise, do a linear search
	for (GLuint i = 0; i < lastUsedParticle; ++i) {
		if (this->particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
	lastUsedParticle = 0;
	return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 frame, GLboolean fromEnemy)
{
	GLfloat random = ((rand() % 100) - 50) / 10.0f;
	switch (this->type)
	{
	case 'e' :  // engine particles
			particle.position = glm::vec2(object.position.x + object.size.x / 2 + random - sheet.spriteWidth / 2, object.position.y + object.size.y - sheet.spriteHeight / 2);
			particle.color = glm::vec4(0.6f, 1.5f, 1.5f, 1.0f);
			particle.life = 0.5f;
			particle.velocity.x = 0;
			particle.velocity.y = 125;
			particle.frame = frame;
			particle.rotate = 0;
			particle.scale = 1.0f;
			particle.rVel = 0;
			break;
	case 'h':  // hit particles
			particle.position = glm::vec2(object.position.x + object.size.x / 2 + ((rand() % 100) - 50) / 10.0f - sheet.spriteWidth / 2, object.position.y + object.size.y / 2 + ((rand() % 100) - 50) / 10.0f - sheet.spriteHeight / 2);
			particle.color = glm::vec4(2.0f, 0.7f, 0.3f, 1.0f);
			particle.life = 0.8f;
			particle.velocity.x = rand() % 2 == 0 ? rand() % 25 : -rand() % 25;
			particle.velocity.y = rand() % 2 == 0 ? rand() % 25 : -rand() % 25;
			particle.frame = frame;
			particle.rotate = 0;
			particle.scale = 1.0f;
			particle.rVel = 0;
			break;
	case 's':  // smoke particles
			particle.position = glm::vec2(object.position.x + object.size.x / 2 - sheet.spriteWidth / 2, object.position.y + object.size.y / 2 + random - sheet.spriteHeight / 2);
			particle.color = glm::vec4(0.1f, 0.1f, 0.1f, 0.5f);
			particle.life = 0.5f;
			particle.velocity.x = rand() % 2 == 0 ? rand() % 50 : -rand() % 50;
			particle.velocity.y = rand() % 2 == 0 ? rand() % 50 : -rand() % 50;
			particle.frame = frame;
			particle.rotate = 0;
			particle.scale = 1.0f;
			particle.rVel = 0;
			break;
	case 'f':  // fragment particles
			particle.position = glm::vec2(object.position.x + object.size.x / 2 - sheet.spriteWidth / 2, object.position.y + object.size.y / 2 - sheet.spriteHeight / 2);
			particle.color = glm::vec4(2.0f, 1.0f, 1.0f, 1.0f);
			particle.life = 3.0f;
			particle.velocity.x = rand() % 2 == 0 ? rand() % 50 : -rand() % 50;
			particle.velocity.y = rand() % 2 == 0 ? rand() % 50 : -rand() % 50;
			particle.frame = frame + glm::vec2(0, rand() % 3);
			particle.rotate = rand() % (GLint)glfwGetTime();
			particle.scale = 1.0f;
			particle.rVel = rand() % 2 == 0 ? rand() % 2 + 1 : rand() % -2 - 1;
			break;
	case 't':  // enemy particles
			particle.position = glm::vec2(object.position.x + object.size.x / 2 + random - sheet.spriteWidth / 2, object.position.y - sheet.spriteHeight / 2 + 5);
			particle.color = glm::vec4(2.0f, 0.6f, 0.5f, 1.0f);
			particle.life = 0.25f;
			particle.velocity.x = 0;
			particle.velocity.y = -125;
			particle.frame = frame;
			particle.rotate = 0;
			particle.scale = 1.0f;
			particle.rVel = 0;
			break;
	case 'l':  // spark particles
		if (fromEnemy)
			particle.position = glm::vec2(object.position.x + object.size.x / 2 - sheet.spriteWidth / 2, object.position.y + object.size.y);
		else
			particle.position = glm::vec2(object.position.x + object.size.x / 2 - sheet.spriteWidth / 2, object.position.y - sheet.spriteHeight / 2);

			particle.color = glm::vec4(2.0f, 0.7f, 0.3f, 1.0f);
			particle.life = 0.3f;
			particle.velocity.x = rand() % 2 == 0 ? rand() % 60 : -rand() % 50;
			particle.velocity.y = rand() % 2 == 0 ? rand() % 50 : -rand() % 50;
			particle.frame = frame;
			particle.rotate = rand() % (GLint)glfwGetTime();
			particle.scale = 1.0f;
			particle.rVel = rand() % 2 == 0 ? rand() % 2 + 1 : rand() % -2 - 1;
			break;
	case 'm':  // explosion from missiles particles
			particle.position = glm::vec2(object.position.x + object.size.x / 2 - sheet.spriteWidth/2, object.position.y + object.size.y / 2 - sheet.spriteHeight/2);
			particle.color = glm::vec4(2.0f, 0.7f, 0.3f, 1.0f);
			particle.life = 1.0f;
			particle.velocity.x = rand() % 2 == 0 ? rand() % 25 : -rand() % 25;
			particle.velocity.y = rand() % 2 == 0 ? rand() % 25 : -rand() % 25;
			particle.frame = frame;
			particle.rotate = 0;
			particle.scale = 1.0f;
			particle.rVel = 0;
			break;
	case 'a':  // asteroid particles
			particle.position = glm::vec2(object.position.x + object.size.x / 2 - (sheet.spriteWidth*3) / 2, object.position.y + object.size.y / 2 + random - (sheet.spriteHeight*3) / 2);
			particle.color = glm::vec4(0.1f, 0.1f, 0.1f, 0.5f);
			particle.life = 1.0f;
			particle.velocity.x = rand() % 2 == 0 ? rand() % 50 : -rand() % 50;
			particle.velocity.y = rand() % 2 == 0 ? rand() % 50 : -rand() % 50;
			particle.frame = frame;
			particle.rotate = 0;
			particle.scale = 3.0f;
			particle.rVel = 0;
			break;
	}
}

