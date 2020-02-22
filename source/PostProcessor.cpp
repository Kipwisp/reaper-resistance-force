#include <iostream>

#include "post_processor.h"
#include "resource_manager.h"

PostProcessor::PostProcessor(GLuint width, GLuint height)
	: texture(), width(width), height(height), shake(GL_FALSE)
{
	// Initialize hdr buffer
	glGenFramebuffers(1, &this->hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->hdrFBO);

	glGenTextures(3, this->colorBuffers);

	glBindTexture(GL_TEXTURE_2D, this->colorBuffers[0]);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffers[0], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//--------------------------------------------------------------------------------------
	// Initialize blur buffer
	glGenFramebuffers(1, &this->blurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->blurFBO);

	glGenTextures(1, &this->colorBuffers[1]);
	glBindTexture(GL_TEXTURE_2D, this->colorBuffers[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffers[1], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//--------------------------------------------------------------------------------------
	// Initialize post processing buffer
	glGenFramebuffers(1, &this->ppFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->ppFBO);

	glGenTextures(1, &this->colorBuffers[2]);
	glBindTexture(GL_TEXTURE_2D, this->colorBuffers[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// attach texture to framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorBuffers[2], 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//--------------------------------------------------------------------------------------
	// Initialize guassian ping pong buffers
	glGenFramebuffers(2, this->pingpongFBO);
	glGenTextures(2, this->pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, this->pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, this->pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->pingpongColorbuffers[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//--------------------------------------------------------------------------------------
	// Initialize render data and uniforms
	this->initRenderData();

	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset,  offset },  // top-left
		{ 0.0f,    offset },  // top-center
		{ offset,  offset },  // top-right
		{ -offset,  0.0f },  // center-left
		{ 0.0f,    0.0f },  // center-center
		{ offset,  0.0f },  // center - right
		{ -offset, -offset },  // bottom-left
		{ 0.0f,   -offset },  // bottom-center
		{ offset, -offset }   // bottom-right    
	};
	glUniform2fv(glGetUniformLocation(ResourceManager::getShader("effects").ID, "offsets"), 9, (GLfloat*)offsets);
}

void PostProcessor::beginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->hdrFBO);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
void PostProcessor::endRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->blurFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	ResourceManager::getShader("extractor").use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	this->horizontal = true;
	bool first_iteration = true;
	int amount = 5;

	ResourceManager::getShader("blur").use();

	GLfloat blur_kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv(glGetUniformLocation(ResourceManager::getShader("effects").ID, "blur_kernel"), 9, blur_kernel);

	if (this->bloom)
	{
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, this->pingpongFBO[horizontal]);
			ResourceManager::getShader("blur").setInteger("horizontal", this->horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!this->horizontal]);
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
			this->horizontal = !this->horizontal;
			if (first_iteration)
				first_iteration = false;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, ppFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	ResourceManager::getShader("bloom").use();
	ResourceManager::getShader("bloom").setInteger("bloom", this->bloom);

	// apply bloom
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::render(GLfloat time, GLfloat shakeTime)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ResourceManager::getShader("effects").use();
	ResourceManager::getShader("effects").setFloat("time", time);
	ResourceManager::getShader("effects").setInteger("shake", this->shake);
	ResourceManager::getShader("effects").setFloat("strength", shakeTime / 10);
	ResourceManager::getShader("effects").setInteger("shakeEnabled", this->shakeEnabled);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[2]);

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void PostProcessor::initRenderData()
{
	// Configure VAO/VBO
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos        // Tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		1.0f,  1.0f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (GLvoid*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}