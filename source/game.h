#ifndef GAME_H
#define GAME_H
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <dirent/dirent.h>
#include <GLFW/glfw3.h>

#include "game_object.h"

// Represents the current state of the game
enum GameState {
	ACTIVE,
	MENU,
	OPTIONS1,
	OPTIONS2,
	PAUSE,
	WIN,
	LOSE
};

class Game
{
public:
	GameState state;
	GLfloat scale;
	GLboolean keys[1024];
	GLboolean keysProcessed[1024];
	GLboolean mouse[2];
	glm::vec2 mousePos;
	GLboolean exit;
	GLuint width, height;
	GLint score;
	GLfloat waveTimer;
	GLint currentTrack;
	GLfloat waveFlux;
	GLint flux;
	GLint wave;
	GLint puSpawnRates[10];
	GLint eSpawnRates[8];
	GLfloat mouseDelay;
	GLfloat tutorialLife;
	GLint enemyTimer;

	// Constructor/Destructor
	Game(GLuint width, GLuint height);
	~Game();

	// Initialize game state
	void init();

	// GameLoop
	void processInput(GLfloat dt);
	void update(GLfloat dt);
	void render();
	GLboolean checkCollision(GameObject &one, GameObject &two);
	GLint explosionDamage(GLint distance, GLint radius, GLint maxDamage);
	GLint getDistance(GameObject &one, GameObject &two);
	void updateProjectiles(GLfloat dt);
	void updatePlayer(GLfloat dt);
	void updateEnemies(GLfloat dt);
	void updateMissiles(GLfloat dt);
	void updateBeams(GLfloat dt);
	void updateAsteroids(GLfloat dt);
	void updatePowerUps(GLfloat dt);
	void updateInfoTexts(GLfloat dt);
	void drawHUD();
	void spawnEnemies(GLfloat dt);
	void spawnPowerUps(glm::vec2 pos);
	void getMusic();
	std::string selectTrack();
	void saveSettings();
	void readSettings();
	void updateSpawnRates(GLint wave);
	void resetGame();
};

#endif