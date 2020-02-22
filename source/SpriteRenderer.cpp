#include "sprite_renderer.h"
#include <iostream>

SpriteRenderer::SpriteRenderer(Shader &shader)
{
	this->shader = shader;
	this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
	glDeleteVertexArrays(1, &this->VAO);
}

void SpriteRenderer::drawSprite(Texture2D &texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec4 color)
{
	// Prepare transformations
	this->shader.use();
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

	this->shader.setMatrix4("model", model);

	// Render textured quad
	this->shader.setVector4f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::drawSprite(SpriteSheet &sheet, glm::vec2 position, glm::vec2 spriteLocation, glm::vec2 size, GLfloat rotate, glm::vec4 color)
{
	GLfloat posX = (spriteLocation.y * sheet.spriteWidth) / (GLfloat)sheet.width;
	GLfloat posY = (spriteLocation.x * sheet.spriteHeight) / (GLfloat)sheet.height;

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

	this->shader.use();
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

	model = glm::translate(model, glm::vec3(0.5f * sheet.spriteWidth, 0.5f * sheet.spriteHeight, 0.0f)); // Move origin of rotation to center of quad
	model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
	model = glm::translate(model, glm::vec3(-0.5f * sheet.spriteWidth, -0.5f * sheet.spriteHeight, 0.0f)); // Move origin back

	model = glm::scale(model, glm::vec3(glm::vec2(size.x, size.y), 1.0f)); // Last scale

	this->shader.setMatrix4("model", model);

	// Render textured quad
	this->shader.setVector4f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	sheet.bind();

	glBindBuffer(GL_ARRAY_BUFFER, ssVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
	// Configure VAO/VBO
	GLfloat vertices[] = {
		// Pos      // Tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &ssVBO);

	glBindBuffer(GL_ARRAY_BUFFER, ssVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}