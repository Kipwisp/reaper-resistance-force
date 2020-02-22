#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "resource_manager.h"
#include "stb_image.h"
#include <iostream>
#include <fstream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

GLfloat scale = 1.0f;

// create game 
Game game(700, 900);

int main()
{
	// get scale in settings file
	//--------------------------------
	std::ifstream file("cfg/settings.cfg");
	std::string line;
	GLint i = 0;

	if (file.is_open())
	{
		while (getline(file, line))
		{
			if (line.find(':') != std::string::npos)
			{
				line = line.substr(line.find(':') + 1);
				if (i == 4 && std::stof(line) >= 0.8)
				{
					scale = std::stof(line);
				}
				i++;
			}
		}

		std::cout << "Scale set" << std::endl;
		file.close();
	}
	else
		std::cout << "Failed to open settings file" << std::endl;

	game.scale = scale;

	// screen dimensions
	const GLuint SCREEN_WIDTH = 700 * scale;
	const GLuint SCREEN_HEIGHT = 900 * scale;

	game.width = SCREEN_WIDTH;
	game.height = SCREEN_HEIGHT;

	// initialize and configure glfw
	//------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// create glfw window
	//------------------------------
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Reaper Resistance Force", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	// load OpenGL function pointers with GLAD
	//---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// OpenGL settings
	//-------------------------------------------
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initialize game
	game.init();

	// DeltaTime variables
	GLfloat deltaT = 0.0f;
	GLfloat lastFrame = 0.0f;

	int width, height, nrChannels;
	unsigned char* pixels = stbi_load("textures/cursor.png", &width, &height, &nrChannels, 0);

	GLFWimage image;
	image.width = 16;
	image.height = 16;
	image.pixels = pixels;
	GLFWcursor* cursor = glfwCreateCursor(&image, 0, 0);
	glfwSetCursor(window, cursor);

	unsigned char* pixels2 = stbi_load("textures/icon.png", &width, &height, &nrChannels, 0);
	GLFWimage image2;
	image2.width = 30;
	image2.height = 30;
	image2.pixels = pixels2;
	glfwSetWindowIcon(window, 1, &image2);

	// render loop
	//-----------
	while (!glfwWindowShouldClose(window))
	{
		// Calculate how long it took to process previous frame to compensate
		GLfloat currentFrame = glfwGetTime();
		deltaT = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();


		// Manage user input
		game.processInput(deltaT);

		// Update Game state
		game.update(deltaT);

		// Render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		game.render();

		glfwSwapBuffers(window);
	}


	// de-allocate all resources
	//------------------------------------------------------------------------
	ResourceManager::clear();

	// terminate glfw
	//------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process input from user
// ---------------------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			game.keys[key] = GL_TRUE;
			game.keysProcessed[key] = GL_FALSE;
		}
		else if (action == GLFW_RELEASE)
		{
			game.keys[key] = GL_FALSE;
			game.keysProcessed[key] = GL_TRUE;
		}
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		game.mouse[0] = GL_TRUE; 
	}
	else
	{
		game.mouse[0] = GL_FALSE;
	}

	if (game.exit)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	game.mousePos = glm::vec2(xpos, ypos);
}