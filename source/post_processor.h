#ifndef POST_PROCESSOR_H
#define POST_PROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_renderer.h"
#include "shader.h"


// PostProcessor hosts all PostProcessing effects for the Breakout
// Game. It renders the game on a textured quad after which one can
// enable specific effects by enabling either the Confuse, Chaos or 
// Shake boolean. 
// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
class PostProcessor
{
public:
	// State
	Texture2D texture;
	GLuint width, height;
	GLboolean shake;
	GLboolean bloom;
	GLboolean shakeEnabled;

	// Constructor
	PostProcessor(GLuint width, GLuint height);
	// Prepares the postprocessor's framebuffer operations before rendering the game
	void beginRender();
	// Should be called after rendering the game, so it stores all the rendered data into a texture object
	void endRender();
	// Renders the PostProcessor texture quad (as a screen-encompassing large sprite)
	void render(GLfloat time, GLfloat shakeTime);
private:
	// Render state
	GLuint hdrFBO; // bloom buffer
	GLuint blurFBO; // guassian blur buffers
	GLuint ppFBO; // post processing effects buffer
	GLuint colorBuffers[3]; // 0 = original scene, 1 = brightness extracted, 2 = apply effects
	GLuint VAO;
	GLboolean horizontal; 
	GLuint pingpongFBO[2]; // buffers for gaussian blurring
	GLuint pingpongColorbuffers[2];
	// Initialize quad for rendering postprocessing texture
	void initRenderData();
};

#endif