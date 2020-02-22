#include <algorithm> 
#include <sstream>
#include <iostream>
#include <fstream>
#include <list>


#include <irrKlang/irrKlang.h>
using namespace irrklang;

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "player.h"
#include "destroyer.h"
#include "leviathan.h"
#include "vanguard.h"
#include "reaver.h"
#include "mincer.h"
#include "harbinger.h"
#include "cruiser.h"
#include "asteroid.h"
#include "post_processor.h"
#include "particle_generator.h"
#include "text_renderer.h"
#include "power_up.h"
#include "info_text.h"

// game related data
SpriteRenderer *sprRenderer;
PostProcessor *postProcessor;
ParticleGenerator *engineParticleGenerator;
ParticleGenerator *enemyParticleGenerator;
ParticleGenerator *hitParticleGenerator;
ParticleGenerator *fragmentParticleGenerator;
ParticleGenerator *smokeParticleGenerator;
ParticleGenerator *sparkParticleGenerator;
ParticleGenerator *explosionParticleGenerator;
ParticleGenerator *asteroidParticleGenerator;
TextRenderer *textRenderer;
Player *player;
std::list<Projectile*> projectiles;
std::list<Beam*> beams;
std::list<Missile*> missiles;
std::list<Enemy*> enemies;
std::list<Asteroid*> asteroids;
std::list<PowerUp*> powerups;
std::list<InfoText*> texts;

ISoundEngine *soundEffects = createIrrKlangDevice();
ISoundEngine *soundMusic = createIrrKlangDevice();
ISound *ambience;

GLfloat shakeTime = 0.0f;
GLboolean pressed = GL_FALSE;
GLint musicVolume;
GLint effectsVolume;
std::vector<std::string> tracks;

Game::Game(GLuint width, GLuint height) : state(MENU), keys(), width(width), height(height), keysProcessed(), mouse(), exit(GL_FALSE)
{
}

Game::~Game()
{
	saveSettings();
	// cleaning up
	delete sprRenderer;
	delete postProcessor;
	delete engineParticleGenerator;
	delete enemyParticleGenerator;
	delete hitParticleGenerator;
	delete fragmentParticleGenerator;
	delete smokeParticleGenerator;
	delete sparkParticleGenerator;
	delete explosionParticleGenerator;
	delete asteroidParticleGenerator;
	delete textRenderer;
	delete player;
	while (!projectiles.empty()) delete projectiles.front(), projectiles.pop_front();
	while (!enemies.empty()) delete enemies.front(), enemies.pop_front();
	while (!beams.empty()) delete beams.front(), beams.pop_front();
	while (!missiles.empty()) delete missiles.front(), missiles.pop_front();
	while (!asteroids.empty()) delete asteroids.front(), asteroids.pop_front();
	while (!powerups.empty()) delete powerups.front(), powerups.pop_front();
	while (!texts.empty()) delete texts.front(), texts.pop_front();
	soundEffects->drop();
	soundMusic->drop();
}

void Game::init()
{
	// load shaders
	ResourceManager::loadShader("shaders/sprite_shader.vertex", "shaders/sprite_shader.fragment", nullptr, "sprite");
	ResourceManager::loadShader("shaders/blur_shader.vertex", "shaders/blur_shader.fragment", nullptr, "blur");
	ResourceManager::loadShader("shaders/bloom_shader.vertex", "shaders/bloom_shader.fragment", nullptr, "bloom");
	ResourceManager::loadShader("shaders/extractor_shader.vertex", "shaders/extractor_shader.fragment", nullptr, "extractor");
	ResourceManager::loadShader("shaders/effects_shader.vertex", "shaders/effects_shader.fragment", nullptr, "effects");
	ResourceManager::loadShader("shaders/particle_shader.vertex", "shaders/particle_shader.fragment", nullptr, "particle");

	// configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader("sprite").use();
	ResourceManager::getShader("sprite").setInteger("image", 0);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection);
	ResourceManager::getShader("bloom").use();
	ResourceManager::getShader("bloom").setInteger("scene", 0);
	ResourceManager::getShader("bloom").setInteger("bloomBlur", 1);
	ResourceManager::getShader("bloom").setFloat("exposure", 1);
	ResourceManager::getShader("blur").use();
	ResourceManager::getShader("blur").setInteger("image", 0);
	ResourceManager::getShader("extractor").use();
	ResourceManager::getShader("extractor").setInteger("image", 0);
	ResourceManager::getShader("effects").use();
	ResourceManager::getShader("effects").setInteger("image", 0);
	ResourceManager::getShader("particle").use();
	ResourceManager::getShader("particle").setInteger("image", 0);
	ResourceManager::getShader("particle").setMatrix4("projection", projection);

	// load spritesheets
	ResourceManager::loadSpriteSheet("textures/background.jpg", GL_FALSE, "background", this->width, this->height, 3, 1);
	ResourceManager::loadSpriteSheet("textures/asteroid_1.png", GL_TRUE, "asteroid1", 100, 100, 1, 2);
	ResourceManager::loadSpriteSheet("textures/asteroid_2.png", GL_TRUE, "asteroid2", 75, 75, 1, 2);
	ResourceManager::loadSpriteSheet("textures/asteroid_3.png", GL_TRUE, "asteroid3", 50, 50, 1, 2);

	ResourceManager::loadSpriteSheet("textures/particle.png", GL_TRUE, "particle", 10, 10, 1, 2);
	ResourceManager::loadSpriteSheet("textures/explosion.png", GL_TRUE, "explosion", 250, 250, 1, 1);
	ResourceManager::loadSpriteSheet("textures/hit.png", GL_TRUE, "hit", 20, 20, 1, 1);

	ResourceManager::loadSpriteSheet("textures/ship.png", GL_TRUE, "ship", 48, 44, 1, 3);
	ResourceManager::loadSpriteSheet("textures/projectiles.png", GL_TRUE, "projectiles", 4, 24, 2, 2);
	ResourceManager::loadSpriteSheet("textures/missiles.png", GL_TRUE, "missiles", 20, 20, 2, 2);
	ResourceManager::loadSpriteSheet("textures/pellet.png", GL_TRUE, "pellet", 10, 10, 1, 2);
	ResourceManager::loadSpriteSheet("textures/power_ups.png", GL_TRUE, "power_ups", 35, 35, 2, 10);
	ResourceManager::loadSpriteSheet("textures/shield.png", GL_TRUE, "shield", 74, 74, 1, 1);

	ResourceManager::loadSpriteSheet("textures/destroyer.png", GL_TRUE, "destroyer", 30, 30, 1, 2);
	ResourceManager::loadSpriteSheet("textures/leviathan.png", GL_TRUE, "leviathan", 60, 60, 1, 3);
	ResourceManager::loadSpriteSheet("textures/vanguard.png", GL_TRUE, "vanguard", 45, 45, 1, 3);
	ResourceManager::loadSpriteSheet("textures/reaver.png", GL_TRUE, "reaver", 35, 35, 2, 3);
	ResourceManager::loadSpriteSheet("textures/mincer.png", GL_TRUE, "mincer", 35, 35, 1, 2);
	ResourceManager::loadSpriteSheet("textures/harbinger.png", GL_TRUE, "harbinger", 50, 50, 1, 3);
	ResourceManager::loadSpriteSheet("textures/cruiser.png", GL_TRUE, "cruiser", 40, 40, 1, 1);
	ResourceManager::loadSpriteSheet("textures/fragments.png", GL_TRUE, "fragments", 20, 20, 6, 3);

	ResourceManager::loadSpriteSheet("textures/health.png", GL_TRUE, "health", 700, 22, 3, 1);
	ResourceManager::loadSpriteSheet("textures/wave.png", GL_TRUE, "wave", 700, 22, 3, 1);
	ResourceManager::loadSpriteSheet("textures/ammo.png", GL_TRUE, "ammo", 80, 25, 1, 1);
	ResourceManager::loadSpriteSheet("textures/tutorial.png", GL_TRUE, "tutorial", 700, 125, 1, 1);
	ResourceManager::loadSpriteSheet("textures/button.png", GL_TRUE, "button", 200, 50, 7, 1);
	ResourceManager::loadSpriteSheet("textures/outcome.png", GL_TRUE, "outcome", 700, 400, 2, 1);
	ResourceManager::loadSpriteSheet("textures/title_screen.png", GL_TRUE, "title", 700, 200, 1, 1);
	ResourceManager::loadSpriteSheet("textures/text.png", GL_TRUE, "text", 700, 50, 2, 1);
	ResourceManager::loadSpriteSheet("textures/overlay.png", GL_TRUE, "overlay", 700, 900, 1, 1);
	ResourceManager::loadSpriteSheet("textures/bar.png", GL_TRUE, "bar", 200, 25, 2, 1);
	ResourceManager::loadSpriteSheet("textures/checkbox.png", GL_TRUE, "checkbox", 50, 50, 1, 2);
	ResourceManager::loadSpriteSheet("textures/incoming.png", GL_TRUE, "incoming", 700, 200, 1, 1);

	// random number seed
	srand(time(NULL));

	// set up renderers
	sprRenderer = new SpriteRenderer(ResourceManager::getShader("sprite"));
	postProcessor = new PostProcessor(this->width, this->height);
	engineParticleGenerator = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getSpriteSheet("particle"), 500, 'e');
	enemyParticleGenerator = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getSpriteSheet("particle"), 2500, 't');
	hitParticleGenerator = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getSpriteSheet("hit"), 1000, 'h');
	fragmentParticleGenerator = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getSpriteSheet("fragments"), 500, 'f');
	smokeParticleGenerator = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getSpriteSheet("particle"), 2000, 's');
	sparkParticleGenerator = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getSpriteSheet("particle"), 500, 'l');
	explosionParticleGenerator = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getSpriteSheet("explosion"), 500, 'm');
	asteroidParticleGenerator = new ParticleGenerator(ResourceManager::getShader("particle"), ResourceManager::getSpriteSheet("particle"), 500, 'a');
	textRenderer = new TextRenderer(this->width, this->height);
	textRenderer->load("fonts/neuropol.ttf", 24);

	// set up game objects 
	player = new Player(this->width, this->height);
	this->waveTimer = 1500.0f;
	this->waveFlux = 0.75f;
	this->flux = 1;
	this->wave = 1;
	this->updateSpawnRates(this->wave);
	this->tutorialLife = 300;
	this->enemyTimer = 5;

	readSettings();

	// set up sound
	this->getMusic();
	soundEffects->setSoundVolume(effectsVolume/200.0f);
	soundMusic->setSoundVolume(musicVolume/200.0f);
	ambience = soundEffects->play2D("sounds/ambience.wav", GL_FALSE, GL_FALSE, GL_TRUE);  // ambience
	soundMusic->play2D(selectTrack().c_str(), GL_FALSE, GL_FALSE, GL_TRUE); // background music
}

void Game::update(GLfloat dt)
{
	if (this->state == ACTIVE || this->state == LOSE || this->state == WIN)
	{
		if (player->health <= 0 && this->state == ACTIVE) // player is dead
		{
			this->state = LOSE;
			hitParticleGenerator->update(*player, 40);
			fragmentParticleGenerator->update(*player, 3, glm::vec2(8, 0));
			soundEffects->play2D("sounds/player_death.wav", GL_FALSE);
			shakeTime = 1.0f;
			postProcessor->shake = GL_TRUE;
			if (player->beam != nullptr)
				player->beam->active = GL_FALSE;
			delete player;
			player = new Player();
		}
		// for flashing wave meter and text
		if (this->waveFlux > 1.0f)
			this->flux = -1;
		else if (this->waveFlux < 0.75f)
			this->flux = 1;

		this->waveFlux += 0.5 * dt * this->flux;

		if (this->state == ACTIVE)
		{
			this->waveTimer -= 10 * dt;
			// player wins
			if (this->wave == 10 && this->waveTimer <= 0 && enemies.empty())
			{
				this->state = WIN;
				this->score *= 2;
			}

			// enter next wave
			else if (waveTimer < 0 && wave < 10)
			{
				soundEffects->play2D("sounds/next_wave.wav", GL_FALSE);
				waveTimer = 1500.0f;
				wave++;
				this->updateSpawnRates(wave);
			}
		}

		// tutorial slowly fades away
		if (this->tutorialLife > 0)
			tutorialLife -= 30 * dt;

		// update particles effects
		engineParticleGenerator->update(dt);
		enemyParticleGenerator->update(dt);
		hitParticleGenerator->update(dt);
		fragmentParticleGenerator->update(dt);
		smokeParticleGenerator->update(dt);
		sparkParticleGenerator->update(dt);
		explosionParticleGenerator->update(dt);
		asteroidParticleGenerator->update(dt);

		// update game objects
		if (this->state != LOSE)
			updatePlayer(dt);
		updateEnemies(dt);
		updateAsteroids(dt);
		updatePowerUps(dt);
		updateInfoTexts(dt);
		updateProjectiles(dt);
		updateBeams(dt);
		updateMissiles(dt);

		spawnEnemies(dt);

		// update postprocessing effects
		if (shakeTime > 0.0f)
		{
			shakeTime -= 1 * dt;
			if (shakeTime <= 0.0f)
				postProcessor->shake = GL_FALSE;
		}
	}
	else if (this->state == MENU || this->state == OPTIONS1)
	{
		engineParticleGenerator->update(*player, 2);
		engineParticleGenerator->update(dt);
	}

	// updating background music
	if (!soundMusic->isCurrentlyPlaying(tracks[currentTrack].c_str()))
		soundMusic->play2D(selectTrack().c_str(), GL_FALSE, GL_FALSE, GL_TRUE);

	// ambience loop
	if (ambience->getPlayPosition() > 4500)
		ambience->setPlayPosition(1000);

	this->mouseDelay -= 10 * dt;
}

void Game::processInput(GLfloat dt) // W = UP, A = LEFT, S = DOWN, D = RIGHT, SPACE = FIRE, Q = MINE, E = ROCKET
{
	GLint maxVelL = -300;
	GLint maxVelR = 300;
	GLint maxVelU = -325; // higher due to player moving down 25 constantly
	GLint maxVelD = 275; // lower due to player moving down 25 constantly

	GLint velL = -30;
	GLint velR = 30;
	GLint velU = -30; 
	GLint velD = 30;

	GLint velS = 10;  // speed at which ship decelerates

	GLfloat *vx = &player->velocity.x;
	GLfloat *vy = &player->velocity.y;

	if (this->state == ACTIVE || state == WIN)
	{
		if (this->keys[GLFW_KEY_ESCAPE] && this->state == ACTIVE)
			this->state = PAUSE;

		if (this->keys[GLFW_KEY_A] && *vx > maxVelL)							
			*vx += velL;			
		if (this->keys[GLFW_KEY_D] && *vx < maxVelR)					
			*vx += velR;			
		if (!(this->keys[GLFW_KEY_D] || this->keys[GLFW_KEY_A]))						// lose velocity when not accelerating
		{
			if (*vx > 0)
				*vx += -velS;
			else if (*vx < 0)
				*vx += velS;
		}

		if (this->keys[GLFW_KEY_S] && *vy < maxVelD)					
			*vy += velD;
		else if (this->keys[GLFW_KEY_W] && *vy > maxVelU)							
			*vy += velU;			

		if (!(this->keys[GLFW_KEY_W] || this->keys[GLFW_KEY_S]))						// lose velocity when not accelerating
		{
			if (*vy > 0)
				*vy += -velS;
			else if (*vy < 0)
				*vy += velS;
		}

		// make sure velocities don't exceed maximum
		if (*vy < maxVelU)
			*vy = maxVelU;

		if (*vy > maxVelD)
			*vy = maxVelD;

		if (*vx < maxVelL)
			*vx = maxVelL;

		if (*vx > maxVelR)
			*vx = maxVelR;

		// fix possible floating point errors
		if (*vx < velS && *vx > -velS)
			*vx = 0;

		if (*vy < velS && *vy > -velS)
			*vy = 0;

		if (this->keys[GLFW_KEY_SPACE] && player->bulletTimer <= 0)
		{
			std::string filename = std::string("sounds/laser_") + std::to_string(rand() % 2 + 1) + ".wav";
			if (player->tripleLasers)
			{ 
				soundEffects->play2D(filename.c_str(), GL_FALSE);
				projectiles.push_front(new Laser(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, player->position.y), glm::vec2(0, -700), player->damage, GL_TRUE));
				projectiles.push_front(new Laser(glm::vec2(player->position.x + 10 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, player->position.y + 20), glm::vec2(0,-700), player->damage, GL_TRUE));
				projectiles.push_front(new Laser(glm::vec2(player->position.x + player->size.x  - 10 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, player->position.y + 20), glm::vec2(0, -700), player->damage, GL_TRUE));
			}
			else if (player->pelletShot)
			{
				soundEffects->play2D(filename.c_str(), GL_FALSE);
				projectiles.push_front(new Pellet(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, player->position.y), glm::vec2(0, -700), player->damage, GL_TRUE));
				projectiles.push_front(new Pellet(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, player->position.y), glm::vec2(-250, -700), player->damage, GL_TRUE));
				projectiles.push_front(new Pellet(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, player->position.y), glm::vec2(-500, -700), player->damage, GL_TRUE));
				projectiles.push_front(new Pellet(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, player->position.y), glm::vec2(250, -700), player->damage, GL_TRUE));
				projectiles.push_front(new Pellet(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, player->position.y), glm::vec2(500, -700), player->damage, GL_TRUE));
			}
			else if (player->beams)
			{
				if (!player->firingBeam)
				{
					beams.push_front(player->fireBeam());
					player->firingBeam = GL_TRUE;
					player->beam->snd = soundEffects->play2D("sounds/beam.wav", false, false, true);
				}
			}
			else
			{
				soundEffects->play2D(filename.c_str(), GL_FALSE);
				projectiles.push_front(new Laser(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("projectiles").spriteWidth / 2, player->position.y), glm::vec2(0, -700), player->damage, GL_TRUE));
			}

			player->bulletTimer = player->reloadTime;
			pressed = GL_TRUE;
		}

		if (this->keysProcessed[GLFW_KEY_SPACE] && player->firingBeam || !player->beams && player->firingBeam) // deactivate beam if player was firing it
		{
			player->beam->active = GL_FALSE;
			player->firingBeam = GL_FALSE;
		}

		if (pressed && !this->keys[GLFW_KEY_SPACE]) // faster firing if pressing space rapidly
		{
			player->bulletTimer -= 80 * dt;
			pressed = GL_FALSE;
		}

		if (this->keys[GLFW_KEY_E] && player->bulletTimer <= 0)
		{
			if (player->missiles > 0)
			{
				missiles.push_front(new Missile(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, player->position.y), glm::vec2(0, -500), player->damage * 10, GL_TRUE));
				player->missiles--;
				player->bulletTimer = player->reloadTime;
			}
			else
			{
				soundEffects->play2D("sounds/no_ammo.wav", GL_FALSE);
				player->bulletTimer = player->reloadTime;
			}
		}

		if (this->keys[GLFW_KEY_Q] && player->bulletTimer <= 0)
		{
			if (player->mines > 0)
			{
				missiles.push_front(new Mine(glm::vec2(player->position.x + player->size.x / 2 - ResourceManager::getSpriteSheet("missiles").spriteWidth / 2, player->position.y), player->damage * 10, GL_TRUE));
				soundEffects->play2D("sounds/mine.wav", GL_FALSE);
				player->mines--;
				player->bulletTimer = player->reloadTime;
			}
			else
			{
				soundEffects->play2D("sounds/no_ammo.wav", GL_FALSE);
				player->bulletTimer = player->reloadTime;
			}
		}

		engineParticleGenerator->update(*player, this->keys[GLFW_KEY_S] ? 0 : 2);
	}
	if (state == LOSE || state == WIN)
	{
		if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if reset button was clicked
		{
			this->resetGame();
			soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
			this->state = ACTIVE;
			this->mouseDelay = 5;
		}
		else if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if menu button was clicked
		{
			this->resetGame();
			soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
			this->state = MENU;
			this->mouseDelay = 5;
		}
	}
	else if (state == MENU)
	{
		if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if play button was clicked
		{
			soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
			this->state = ACTIVE;
			this->mouseDelay = 5;
		}
		else if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if options button was clicked
		{
			soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
			this->state = OPTIONS1;
			this->mouseDelay = 5;
		}
		else if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 250), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if options button was clicked
		{
			this->exit = GL_TRUE;
			this->mouseDelay = 5;
		}
	}
	else if (state == PAUSE)
	{
		if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if play button was clicked
		{
			soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
			this->state = ACTIVE;
			this->mouseDelay = 5;
		}
		else if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if options button was clicked
		{
			soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
			this->state = OPTIONS2;
			this->mouseDelay = 5;
		}
		else if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 250), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if options button was clicked
		{
			soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
			this->resetGame();
			this->state = MENU;
			this->mouseDelay = 5;
		}
	}
	else if (state == OPTIONS1 || state == OPTIONS2)
	{
		if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 10, this->height / 2 - 100), glm::vec2(20, 20))) && this->mouseDelay <= 0) // check if slider was clicked
		{
			postProcessor->shakeEnabled = !postProcessor->shakeEnabled;
			this->mouseDelay = 5;
		}
		if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 10, this->height / 2 - 50), glm::vec2(20, 20))) && this->mouseDelay <= 0) // check if slider was clicked
		{
			postProcessor->bloom = !postProcessor->bloom;
			this->mouseDelay = 5;
		}
		if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2), glm::vec2(200, 15)))) // check if slider was clicked
		{
			musicVolume = mousePos.x - (this->width / 2 - 100);
			if (musicVolume < 0)
				musicVolume = 0;
			else if (musicVolume > 200)
				musicVolume = 200;
			soundMusic->setSoundVolume(musicVolume / 200.0f);
		}
		else if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(200, 15)))) // check if slider was clicked
		{
			effectsVolume = mousePos.x - (this->width / 2 - 100);
			if (effectsVolume < 0)
				effectsVolume = 0;
			else if (effectsVolume > 200)
				effectsVolume = 200;
			soundEffects->setSoundVolume(effectsVolume / 200.0f);
		}
		if (state == OPTIONS1)
		{
			if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if options button was clicked
			{
				soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
				this->state = MENU;
				this->mouseDelay = 5;
			}
		}
		else
		{
			if (this->mouse[0] && checkCollision(GameObject(this->mousePos, glm::vec2(1, 1)), GameObject(glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(200, 50))) && this->mouseDelay <= 0) // check if options button was clicked
			{
				soundEffects->play2D("sounds/button_click.wav", GL_FALSE);
				this->state = PAUSE;
				this->mouseDelay = 5;
			}
		}
	}
}

void Game::render()
{
	if (!(this->state == MENU) && !(this->state == OPTIONS1))
	{
		postProcessor->beginRender();

		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("background"), glm::vec2(0, 0), glm::vec2(glfwGetTime() / -50.0f, 0), glm::vec2(this->width, this->height), 0.0f); // move background

		std::list<PowerUp*>::iterator pu;
		for (pu = powerups.begin(); pu != powerups.end(); ++pu)
		{
			(*pu)->orb.draw(*sprRenderer);
			(*pu)->draw(*sprRenderer);
		}

		std::list<Projectile*>::iterator p;
		for (p = projectiles.begin(); p != projectiles.end(); ++p)
		{
			(*p)->draw(*sprRenderer);
		}

		std::list<Beam*>::iterator b;
		for (b = beams.begin(); b != beams.end(); ++b)
		{
			(*b)->draw(*sprRenderer);
		}

		std::list<Missile*>::iterator m;
		for (m = missiles.begin(); m != missiles.end(); ++m)
		{
			(*m)->draw(*sprRenderer);
		}

		engineParticleGenerator->draw();
		fragmentParticleGenerator->draw();
		enemyParticleGenerator->draw();

		std::list<Enemy*>::iterator e;
		for (e = enemies.begin(); e != enemies.end(); ++e)
		{
			(*e)->draw(*sprRenderer);
		}

		if (!(this->state == LOSE))
		{
			player->draw(*sprRenderer, 0, player->frame);
			if (player->shield) // if shield powerup is active
				sprRenderer->drawSprite(ResourceManager::getSpriteSheet("shield"), glm::vec2(player->position.x - (74*scale - player->size.x) / 2, player->position.y - (74*scale - player->size.y) / 2), glm::vec2(0, 0), glm::vec2(74, 74)*scale, 0.0f, glm::vec4(1.0f - player->powerTimer[2] / 500.0f, 0.3f, player->powerTimer[2] / 500.0f, 1.0f)); // shield changes color as it gets weaker
		}

		smokeParticleGenerator->draw();

		std::list<Asteroid*>::iterator a;
		for (a = asteroids.begin(); a != asteroids.end(); ++a)
		{
			(*a)->draw(*sprRenderer);
		}

		asteroidParticleGenerator->draw();
		sparkParticleGenerator->draw();

		hitParticleGenerator->draw();
		explosionParticleGenerator->draw();

		postProcessor->endRender();
		postProcessor->render(glfwGetTime(), shakeTime);

		std::list<InfoText*>::iterator t;
		for (t = texts.begin(); t != texts.end(); ++t)
		{
			(*t)->draw(*textRenderer);
		}

		drawHUD();
	}
	else
	{
		postProcessor->beginRender();

		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("background"), glm::vec2(0, 0), glm::vec2(glfwGetTime() / -50.0f, 0),  glm::vec2(this->width, this->height), 0.0f); // move background
		engineParticleGenerator->draw();
		player->draw(*sprRenderer, 0, player->frame);

		postProcessor->endRender();
		postProcessor->render(glfwGetTime(), shakeTime);

		if (this->state == MENU)
		{
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("overlay"), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(this->width, this->height));
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("title"), glm::vec2(0, 200), glm::vec2(0.0f, 0.0f), glm::vec2(this->width, 200*scale), 0.0f);
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(3, 0), glm::vec2(200, 50));
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(2, 0), glm::vec2(200, 50));
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 250), glm::vec2(6, 0), glm::vec2(200, 50));
		}
		else
		{
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("overlay"), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(700, 900));
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("text"), glm::vec2(0, 200 * scale), glm::vec2(1, 0), glm::vec2(this->width, 50 * scale));

			if (postProcessor->shakeEnabled)
				sprRenderer->drawSprite(ResourceManager::getSpriteSheet("checkbox"), glm::vec2(this->width / 2 - 10, this->height / 2 - 100), glm::vec2(0, 1), glm::vec2(20, 20));  // shake toggle
			else
				sprRenderer->drawSprite(ResourceManager::getSpriteSheet("checkbox"), glm::vec2(this->width / 2 - 10, this->height / 2 - 100), glm::vec2(0, 0), glm::vec2(20, 20));

			if (postProcessor->bloom)
				sprRenderer->drawSprite(ResourceManager::getSpriteSheet("checkbox"), glm::vec2(this->width / 2 - 10, this->height / 2 - 50), glm::vec2(0, 1), glm::vec2(20, 20));  // bloom toggle
			else
				sprRenderer->drawSprite(ResourceManager::getSpriteSheet("checkbox"), glm::vec2(this->width / 2 - 10, this->height / 2 - 50), glm::vec2(0, 0), glm::vec2(20, 20)); 

			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("bar"), glm::vec2(this->width / 2 - 100, this->height / 2), glm::vec2(0, 0), glm::vec2(200, 15));  // music volume
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("bar"), glm::vec2(this->width / 2 - 100 + musicVolume - 5, this->height / 2), glm::vec2(1, 0), glm::vec2(200, 15));

			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("bar"), glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(0, 0), glm::vec2(200, 15));  // sfx volume
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("bar"), glm::vec2(this->width / 2 - 100 + effectsVolume - 5, this->height / 2 + 50), glm::vec2(1, 0), glm::vec2(200, 15));

			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(5, 0), glm::vec2(200, 50));

			textRenderer->renderText("Shake", this->width / 2 - 250, this->height / 2 -100, 0.6f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
			textRenderer->renderText("Bloom", this->width / 2 - 250, this->height / 2 -50, 0.6f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
			textRenderer->renderText("Music Volume", this->width / 2 - 250, this->height / 2, 0.6f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
			textRenderer->renderText("SFX Volume", this->width / 2 - 250, this->height / 2 + 50, 0.6f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
		}
	}
}

GLboolean Game::checkCollision(GameObject &one, GameObject &two) 
{
	// check for x axis collision
	bool collisionX = one.position.x + one.size.x >= two.position.x && two.position.x + two.size.x >= one.position.x;
	// check for y axis collision
	bool collisionY = one.position.y + one.size.y >= two.position.y && two.position.y + two.size.y >= one.position.y;
	// if both collide on y and x axis
	return collisionX && collisionY;
}

GLint Game::explosionDamage(GLint distance, GLint radius, GLint maxDamage) 
{
	return (maxDamage / (GLfloat)radius) * (radius - distance);  // explosions hurt less the farther away you are from it
}

GLint Game::getDistance(GameObject &one, GameObject &two) // get distance between a rectangle and a circle, mostly for collision detection
{
	GLfloat radius = one.size.x / 2;

	glm::vec2 center(one.position + radius);
	glm::vec2 aabb_half_extents(two.size.x / 2, two.size.y / 2);
	glm::vec2 aabb_center(two.position.x + aabb_half_extents.x, two.position.y + aabb_half_extents.y);
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	glm::vec2 closest = aabb_center + clamped;
	difference = closest - center;

	return glm::length(difference);
}

void Game::updatePlayer(GLfloat dt)
{
	player->move(dt, this->width);
	player->updateSprite();

	if (player->bulletTimer > 0)
		player->bulletTimer -= 10 * dt;

	for (int i = 0; i < 7; i++)  // index 0 = triple lasers,  1 = beam, 2 = shield, 3 = multiplier, 4 = faster firing, 5 = 2x damage, 6 = pellet shot
	{
		if (player->powerTimer[i] > 0)
			player->powerTimer[i] -= 10 * dt;
	}

	if (player->powerTimer[0] <= 0 && player->tripleLasers)
	{
		player->tripleLasers = GL_FALSE;
		soundEffects->play2D("sounds/powerup_deactivate.wav", GL_FALSE);
	}

	if (player->powerTimer[1] <= 0 && player->beams)
	{
		player->beams = GL_FALSE;
		soundEffects->play2D("sounds/powerup_deactivate.wav", GL_FALSE);
	}

	if (player->powerTimer[2] <= 0 && player->shield)
	{
		player->shield = GL_FALSE;
		soundEffects->play2D("sounds/shield_deactivate.wav", GL_FALSE);
	}

	if (player->powerTimer[3] <= 0 && player->multiplier > 1)
	{
		player->multiplier = 1;
		soundEffects->play2D("sounds/powerup_deactivate.wav", GL_FALSE);
	}

	if (player->powerTimer[4] <= 0 && player->reloadTime == 2)
	{
		player->reloadTime = 3;
		soundEffects->play2D("sounds/powerup_deactivate.wav", GL_FALSE);
	}

	if (player->powerTimer[5] <= 0 && player->damage == 2)
	{
		player->damage = 1;
		player->color = glm::vec4(1.0f);  // return to normal color for ship
		soundEffects->play2D("sounds/powerup_deactivate.wav", GL_FALSE);
	}

	if (player->powerTimer[6] <= 0 && player->pelletShot)
	{
		player->pelletShot = GL_FALSE;
		soundEffects->play2D("sounds/powerup_deactivate.wav", GL_FALSE);
	}


	//boundaries for bottom and top of screen
	if (player->position.y > this->height - player->size.y)
	{
		player->position.y = this->height - player->size.y;
		player->velocity.y = -1;   // reset velocity to -1 (scrolling velocity)
	}

	else if (player->position.y < 0)
	{
		player->position.y = 0;
		player->velocity.y = 0; // reset velocity
	}

	if (player->health < player->displayHealth - 30 * dt)  // smoother health bar changes
		player->displayHealth -= 30 * dt;

	else if (player->health > player->displayHealth + 30 * dt)
		player->displayHealth += 30 * dt;

	if (player->health <= 25 && player->health > 0)
		smokeParticleGenerator->update(*player, 1);
}

void Game::updateProjectiles(GLfloat dt)
{
	std::list<Projectile*>::iterator p = projectiles.begin();
	GLboolean hit = GL_FALSE;

	while (p != projectiles.end())
	{
		hit = GL_FALSE;
		if ((*p)->position.y < -(*p)->size.y || (*p)->position.y > this->height)
		{
			delete *p;
			p = projectiles.erase(p);
		}
		else
		{
			(*p)->move(dt, this->width);
			std::string filename = std::string("sounds/laser_hit_") + std::to_string(rand() % 4 + 1) + ".wav";

			if ((*p)->fromPlayer)
			{
				std::list<Enemy*>::iterator e = enemies.begin();
				while (e != enemies.end() && !hit)  // check if enemy was hit by player
				{
					if (checkCollision(**p, **e))
					{
						hitParticleGenerator->update(**p, 40);
						soundEffects->play2D(filename.c_str(), GL_FALSE);
						(*e)->health -= (*p)->damage;
						delete *p;
						p = projectiles.erase(p);
						hit = GL_TRUE;
					}
					++e;
				}

				std::list<Missile*>::iterator m = missiles.begin();
				while(m != missiles.end() && !hit) // check if enemy's missiles was hit by player
				{
					if (!(*m)->fromPlayer && checkCollision(**p, **m))
					{
						std::string filenameM = std::string("sounds/explosion_") + std::to_string(rand() % 3 + 1) + ".wav";
						(*m)->explode = GL_TRUE;
						explosionParticleGenerator->update(*(*m)->explosion, 10);
						soundEffects->play2D(filenameM.c_str(), GL_FALSE);
						shakeTime = 0.5f;
						postProcessor->shake = GL_TRUE;
						delete *p;
						p = projectiles.erase(p);
						hit = GL_TRUE;
					}
					++m;
				}
			}
			else
			{
				if (checkCollision(*player, **p)) // check if player was hit by enemy
				{
					hitParticleGenerator->update(**p, 40);
					if (!player->shield)
					{
						player->health -= (*p)->damage;
						soundEffects->play2D("sounds/player_hit.wav", GL_FALSE);
						shakeTime = 0.5f;
						postProcessor->shake = GL_TRUE;
					}
					else
					{
						soundEffects->play2D("sounds/shield_hit.wav", GL_FALSE);
						player->powerTimer[2] -= 10;  // decrease shield time if hit 
					}
					delete *p;
					p = projectiles.erase(p);
					hit = GL_TRUE;
				}
			}

			std::list<Asteroid*>::iterator a = asteroids.begin();
			while (a != asteroids.end() && !hit)
			{
				if (getDistance(**a, **p) <= (*a)->sprite.spriteWidth/2)
				{
					hitParticleGenerator->update(**p, 40);
					soundEffects->play2D(filename.c_str(), GL_FALSE);
					(*a)->health -= (*p)->damage;
					delete *p;
					p = projectiles.erase(p);
					hit = GL_TRUE;
				}
				++a;
			}

			if (!hit)
				++p;
		}
	}
}

void Game::updateBeams(GLfloat dt)
{
	std::list<Beam*>::iterator b = beams.begin();

	while (b != beams.end())
	{
		GLint minY = -2000;
		Enemy *target = nullptr;
		Missile *missileTarget = nullptr;

		if (!(*b)->active)
		{
			(*b)->snd->stop();
			delete *b;
			b = beams.erase(b);
		}
		else
		{
			if ((*b)->snd != nullptr)
			{
				if ((*b)->snd->getPlayPosition() > 2000)
				{
					(*b)->snd->setPlayPosition(1000);
				}
			}

			if ((*b)->fromPlayer)
			{
				std::list<Enemy*>::iterator e = enemies.begin();
				while (e != enemies.end())
				{
					if (checkCollision(**b, **e)) // check if enemy was hit by a beam
					{

						if ((*e)->position.y > minY)
						{
							minY = (*e)->position.y;
							target = *e;
						}
					}
					++e;
				}

				std::list<Missile*>::iterator m = missiles.begin();
				while (m != missiles.end()) // check if enemy's missiles was hit by a beam
				{
					if (!(*m)->fromPlayer && checkCollision(**b, **m))
					{
						if ((*m)->position.y > minY)
						{
							minY = (*m)->position.y;
							missileTarget = *m;
						}
					}
					++m;
				}

				std::list<Asteroid*>::iterator a = asteroids.begin(); // check if asteroid was hit by a beam
				while (a != asteroids.end())
				{
					if (checkCollision(**b, **a))
					{
						if ((*a)->position.y > minY)
						{
							minY = (*a)->position.y;
							target = *a;
						}
					}
					++a;
				}


				if (target != nullptr && missileTarget == nullptr || target != nullptr && target->position.y > missileTarget->position.y) // if the beam is hitting an enemy
				{
					(*b)->size.y = (*b)->obj->position.y - target->position.y - target->size.y + 1; // add 1 to have the beam collide with object
					sparkParticleGenerator->update(**b, 5, glm::vec2(0,1));
					if ((*b)->damageTimer <= 0)
					{
						hitParticleGenerator->update(*target, 40);
						target->health -= (*b)->damage;
						(*b)->damageTimer = 10;
					}
				}
				else if (missileTarget != nullptr) // if the beam is hitting a missile
				{
					std::string filenameM = std::string("sounds/explosion_") + std::to_string(rand() % 3 + 1) + ".wav";
					(*b)->size.y = (*b)->obj->position.y - missileTarget->position.y - missileTarget->size.y + 1; // add 1 to have the beam collide with object
					sparkParticleGenerator->update(**b, 5, glm::vec2(0, 1));
					missileTarget->explode = GL_TRUE;
					explosionParticleGenerator->update(*missileTarget->explosion, 10);
					soundEffects->play2D(filenameM.c_str(), GL_FALSE);
					shakeTime = 0.5f;
					postProcessor->shake = GL_TRUE;
				}
				else
				{
					(*b)->size.y = this->height;
				}
			}
			else
			{
				GLchar target = ' ';
				minY = 2000;
				Asteroid *aTarget = nullptr;

				if (checkCollision(*player, **b)) // check if player was hit by enemy
				{
					minY = player->position.y;
					target = 'p';
				}

				std::list<Asteroid*>::iterator a = asteroids.begin(); // check if asteroid was hit by a beam
				while (a != asteroids.end())
				{
					if (checkCollision(**b, **a) && (*a)->position.y < minY)
					{
						target = 'a';
						aTarget = *a;
					}
					++a;
				}

				if (target == 'p') // player hit
				{
					(*b)->size.y = player->position.y - (*b)->obj->size.y - (*b)->obj->position.y + 20;
					sparkParticleGenerator->update(**b, 5, glm::vec2(0, 1), GL_TRUE);
					if ((*b)->damageTimer <= 0)
					{
						hitParticleGenerator->update(*player, 40);
						if (!player->shield)
						{
							player->health -= (*b)->damage;
							soundEffects->play2D("sounds/player_hit.wav", GL_FALSE);
							shakeTime = 0.5f;
							postProcessor->shake = GL_TRUE;
						}
						else
						{
							soundEffects->play2D("sounds/shield_hit.wav", GL_FALSE);
							player->powerTimer[2] -= 10;  // decrease shield time if hit 
						}
						(*b)->damageTimer = 20;
					}
				}
				else if (target == 'a') // asteroid hit
				{
					(*b)->size.y = aTarget->position.y - (*b)->obj->size.y - (*b)->obj->position.y + 20;
					sparkParticleGenerator->update(**b, 5, glm::vec2(0, 1), GL_TRUE);
					if ((*b)->damageTimer <= 0)
					{
						hitParticleGenerator->update(*aTarget, 40);
						aTarget->health -= (*b)->damage;
						(*b)->damageTimer = 20;
					}
				}
				else
					(*b)->size.y = 900;
			}

			(*b)->update(dt);
			++b;
		}
	}
}

void Game::updateMissiles(GLfloat dt)
{
	std::list<Missile*>::iterator m = missiles.begin();

	while (m != missiles.end())
	{
		if ((*m)->position.y < -(*m)->size.y || (*m)->position.y > this->height)
		{
			delete *m;
			m = missiles.erase(m);
		}

		else
		{
			(*m)->move(dt, this->width);

			if ((*m)->explode)
			{
				(*m)->secondLoop = GL_TRUE;
				if ((*m)->snd != nullptr)
					(*m)->snd->stop();
			}

			if (!(*m)->isMine)
			{
				smokeParticleGenerator->update(**m, 4);
				if ((*m)->snd == nullptr)
					(*m)->snd = soundEffects->play2D("sounds/missile.wav", false, false, true, ESM_NO_STREAMING);
			}

			std::string filename = std::string("sounds/explosion_") + std::to_string(rand() % 3 + 1) + ".wav";

			if ((*m)->fromPlayer)
			{
				GameObject *target = nullptr;
				GLint closest = (*m)->diameter/2 + 1;

				std::list<Enemy*>::iterator e = enemies.begin();
				while (e != enemies.end())
				{
					GLint distance = getDistance(*(*m)->explosion, **e);
					if (!(*m)->explode)
					{
						if (checkCollision(**m, **e)) // check if enemy was hit by player's missile
						{
							(*m)->explode = GL_TRUE;
							explosionParticleGenerator->update(*(*m)->explosion, 10);
							soundEffects->play2D(filename.c_str(), GL_FALSE);
							shakeTime = 0.5f;
							postProcessor->shake = GL_TRUE;
							break;
						}
					}
					else
					{
						GLint damage = explosionDamage(distance, (*m)->diameter/2, (*m)->damage);
						if (damage > 0)
						{
							hitParticleGenerator->update(**e, 40);
							(*e)->health -= damage;
						}
					}

					if (distance < closest)
					{
						closest = distance;
						target = *e;
					}
					if ((*m)->isMine && target != nullptr)
					{
						(*m)->seek(*target);
						if (!soundEffects->isCurrentlyPlaying("sounds/mine.wav"))
							soundEffects->play2D("sounds/mine.wav", GL_FALSE);
					}
					++e;
				}

				std::list<Missile*>::iterator m2 = missiles.begin();
				while (m2 != missiles.end()) // check if enemy's missiles was hit by player's missiles
				{
					GLint distanceM = getDistance(*(*m)->explosion, **m2);
					if (!(*m)->explode)
					{
						if (m != m2 && !(*m2)->fromPlayer && checkCollision(**m2, **m))
						{
							(*m)->explode = GL_TRUE;
							(*m2)->explode = GL_TRUE;
							explosionParticleGenerator->update(*(*m)->explosion, 10);
							soundEffects->play2D(filename.c_str(), GL_FALSE);
							shakeTime = 0.5f;
							postProcessor->shake = GL_TRUE;
							break;
						}
					}
					else
					{
						if (distanceM < (*m)->diameter / 2)
						{
							(*m2)->explode = GL_TRUE;
							explosionParticleGenerator->update(*(*m2)->explosion, 10);
							soundEffects->play2D(filename.c_str(), GL_FALSE);
							shakeTime = 0.5f;
							postProcessor->shake = GL_TRUE;
						}
					}
					++m2;
				}
			}
			else 
			{
				GLint distanceP = getDistance(*(*m)->explosion, *player);

				if (!(*m)->explode)
				{
					if (checkCollision(*player, **m)) // check if player was hit by enemy's missile
					{
						(*m)->explode = GL_TRUE;
						explosionParticleGenerator->update(*(*m)->explosion, 10);
						soundEffects->play2D(filename.c_str(), GL_FALSE);
						shakeTime = 0.5f;
						postProcessor->shake = GL_TRUE;
						break;
					}
				}
				else
				{
					GLint damage = explosionDamage(distanceP, (*m)->diameter / 2, (*m)->damage);
					if (damage > 0)
					{
						hitParticleGenerator->update(*player, 40);
						if (!player->shield)
						{
							player->health -= damage;
							soundEffects->play2D("sounds/player_hit.wav", GL_FALSE);
							shakeTime = 0.5f;
							postProcessor->shake = GL_TRUE;
						}
						else
						{
							soundEffects->play2D("sounds/shield_hit.wav", GL_FALSE);
							player->powerTimer[2] -= 10;  // decrease shield time if hit 
						}
					}

					std::list<Missile*>::iterator m2 = missiles.begin();
					while (m2 != missiles.end()) // check if other missiles were caught in explosion
					{
						GLint distanceM = getDistance(*(*m)->explosion, **m2);
						if (distanceM < (*m)->diameter / 2)
						{
							(*m2)->explode = GL_TRUE;
							explosionParticleGenerator->update(*(*m2)->explosion, 10);
							soundEffects->play2D(filename.c_str(), GL_FALSE);
							shakeTime = 0.5f;
							postProcessor->shake = GL_TRUE;
						}
						++m2;
					}
				}

				if ((*m)->isMine && distanceP <= (*m)->diameter / 2)
				{
					(*m)->seek(*player);
					if (!soundEffects->isCurrentlyPlaying("sounds/mine.wav"))
						soundEffects->play2D("sounds/mine.wav", GL_FALSE);
				}
			}

			std::list<Asteroid*>::iterator a = asteroids.begin(); // check if asteroid was hit by a missile
			while (a != asteroids.end())
			{
				GLint distanceA = getDistance(**m, **a);
				if (!(*m)->explode)
				{
					if ((getDistance(**a, **m) <= (*a)->sprite.spriteWidth / 2))
					{
						(*m)->explode = GL_TRUE;
						explosionParticleGenerator->update(*(*m)->explosion, 10);
						soundEffects->play2D(filename.c_str(), GL_FALSE);
						shakeTime = 0.5f;
						postProcessor->shake = GL_TRUE;
						break;
					}
				}
				else
				{
					GLint damage = explosionDamage(distanceA, (*m)->diameter / 2, (*m)->damage);
					if (damage > 0)
					{
						hitParticleGenerator->update(**a, 40);
						(*a)->health -= damage;
					}
				}
				++a;
			}

			if ((*m)->secondLoop)
			{
				delete *m;
				m = missiles.erase(m);
			}
			else
				++m;
		}
	}
}

void Game::updateEnemies(GLfloat dt)
{
	std::list<Enemy*>::iterator e = enemies.begin();
	while (e != enemies.end())
	{
		if ((*e)->position.y > this->height)
		{
			if ((*e)->beam != nullptr)
				(*e)->beam->active = GL_FALSE;
			delete *e;
			e = enemies.erase(e);
		}
		else if ((*e)->health <= 0)
		{
			if ((*e)->beam != nullptr)
				(*e)->beam->active = GL_FALSE;
			score += (*e)->points * player->multiplier;
			hitParticleGenerator->update(**e, 40);
			fragmentParticleGenerator->update(**e, rand() % 3 + 3, (*e)->fragmentFrame);
			std::string filename = std::string("sounds/explosion_") + std::to_string(rand() % 3 + 1) + ".wav";
			soundEffects->play2D(filename.c_str(), GL_FALSE);
			shakeTime = 0.01f * (*e)->size.x;
			postProcessor->shake = GL_TRUE;
			spawnPowerUps((*e)->position + glm::vec2((*e)->size.x/2 - 17.5, (*e)->size.y / 2 - 17.5));
			delete *e;
			e = enemies.erase(e);
		}
		else
		{
			(*e)->move(dt, this->width);
			(*e)->updateSprite();
			if ((*e)->hasBeam)
			{
				Beam* b = (*e)->fireBeam(dt);
				if (b != nullptr)
				{
					b->snd = soundEffects->play2D("sounds/beam_enemy.wav", false, false, true);
					beams.push_front(b);
				}
			}
			else if ((*e)->hasMissile)
			{
				Missile* m = (*e)->fireMissile(dt);
				if (m != nullptr)
				{
					if (m->isMine)
						soundEffects->play2D("sounds/mine.wav", GL_FALSE);
					missiles.push_front(m);
				}
			}
			else
			{
				std::list<Projectile*> p = (*e)->fire(dt);
				if (!p.empty())
				{
					projectiles.splice(projectiles.end(), p, p.begin(), p.end());
					std::string filename = std::string("sounds/laser_enemy_") + std::to_string(rand() % 2 + 1) + ".wav";
					soundEffects->play2D(filename.c_str(), GL_FALSE);
				}
			}

				
			if ((*e)->health <= (*e)->maxHealth/2)
				smokeParticleGenerator->update(**e, 1);

			if ((*e)->trail)
				enemyParticleGenerator->update(**e, 5);
			++e;
		}
	}
}

void Game::updateAsteroids(GLfloat dt)
{
	std::list<Asteroid*>::iterator a = asteroids.begin();
	while (a != asteroids.end())
	{
		if ((*a)->position.y > this->height)
		{
			delete *a;
			a = asteroids.erase(a);
		}
		else if ((*a)->health <= 0)
		{
			asteroidParticleGenerator->update(**a, 50);
			fragmentParticleGenerator->update(**a, rand() % 3 + (*a)->sizeLevel*3, (*a)->fragmentFrame);
			soundEffects->play2D("sounds/asteroid_break.wav", GL_FALSE);

			if ((*a)->sizeLevel > 1) // break off into two smaller asteroids
			{
				asteroids.push_front(new Asteroid((*a)->position, (*a)->velocity + glm::vec2((abs((*a)->velocity.x)/(*a)->velocity.x)*(rand() % 25 + 25), 0), (*a)->sizeLevel - 1)); // have the two asteroids split off into different directions
				asteroids.push_front(new Asteroid((*a)->position, glm::vec2(-(*a)->velocity.x, (*a)->velocity.y) + glm::vec2(-(abs((*a)->velocity.x) / (*a)->velocity.x)*(rand() % 25 + 25), 0), (*a)->sizeLevel - 1));
				delete *a;
				a = asteroids.erase(a);
			}
			else
			{
				delete *a;
				a = asteroids.erase(a);
			}
		}
		else
		{
			(*a)->move(dt, this->width);

			if ((*a)->damageTimer <= 0 && (getDistance(**a, *player) <= (*a)->sprite.spriteWidth / 2))
			{
				if (!player->shield)
				{
					hitParticleGenerator->update(*player, 40);
					player->health -= (*a)->sizeLevel * 3;
					soundEffects->play2D("sounds/asteroid_hit.wav", GL_FALSE);
					shakeTime = 0.5f;
					postProcessor->shake = GL_TRUE;
				}
				else
				{
					soundEffects->play2D("sounds/shield_hit.wav", GL_FALSE);
					player->powerTimer[2] -= 10;  // decrease shield time if hit 
				}
				(*a)->damageTimer = 30;
			}

			++a;
		}
	}
}

void Game::updatePowerUps(GLfloat dt)
{
	std::list<PowerUp*>::iterator pu = powerups.begin();
	while (pu != powerups.end())
	{
		(*pu)->move(dt, this->width);

		if ((*pu)->position.y > this->height)
		{
			delete *pu;
			pu = powerups.erase(pu);
		}
		else if (checkCollision(**pu, *player))
		{
			if ((*pu)->type == 's')  // if a shield play a special sound
				soundEffects->play2D("sounds/shield_activate.wav", GL_FALSE);
			else if ((*pu)->type == 'h')  // special sound for health too
				soundEffects->play2D("sounds/health_pickup.wav", GL_FALSE);
			else if ((*pu)->type == 'i' || (*pu)->type == 'r')  // along with missiles and mines
				soundEffects->play2D("sounds/ammo_pickup.wav", GL_FALSE);
			else
				soundEffects->play2D("sounds/powerup_activate.wav", GL_FALSE);
			(*pu)->activateEffect(player);
			texts.push_front(new InfoText((*pu)->position, (*pu)->name, 0.6f));
			pu = powerups.erase(pu);
		}
		else
			++pu;
	}
}

void Game::updateInfoTexts(GLfloat dt)
{
	std::list<InfoText*>::iterator t = texts.begin();
	while (t != texts.end())
	{
		if ((*t)->life <= 0)
		{
			delete *t;
			t = texts.erase(t);
		}
		else
		{
			(*t)->life -= 10 * dt;
			(*t)->position += glm::vec2(0, -15 * dt);
			(*t)->color.w = (*t)->life / 15;
			++t;
		}
	}
}

void Game::drawHUD()
{
	// drawing the health bar
	glm::vec4 healthColor = glm::vec4(1 - (player->health / player->maxHealth), player->health / player->maxHealth, 0.0f, 1.0f);
	SpriteSheet sheet = ResourceManager::getSpriteSheet("health");

	sprRenderer->drawSprite(sheet, glm::vec2(0, this->height - 22), glm::vec2(1,0), glm::vec2(this->width, 22));

	if (player->health > 0)
		sprRenderer->drawSprite(sheet, glm::vec2(27*scale - 27*scale * ((player->displayHealth+0.5)/player->maxHealth), this->height - 22), glm::vec2(0, 0), glm::vec2(this->width / (player->maxHealth / (player->displayHealth + 0.5)), 22), 0.0f, healthColor);

	sprRenderer->drawSprite(sheet, glm::vec2(0, this->height - 22), glm::vec2(2, 0), glm::vec2(this->width, 22));

	// drawing the wave meter
	sheet = ResourceManager::getSpriteSheet("wave");

	sprRenderer->drawSprite(sheet, glm::vec2(67*scale, 0), glm::vec2(1, 0), glm::vec2(this->width, 22));

	if (this->waveTimer > 0)
		sprRenderer->drawSprite(sheet, glm::vec2(162 * scale - 95 * scale * ((this->waveTimer + 0.5)/1500.0f), 0), glm::vec2(0, 0), glm::vec2(this->width / (1500.0f / (this->waveTimer)), 22), 0.0f, glm::vec4(0.2f, 0.6f, this->waveFlux, 1.0f));

	sprRenderer->drawSprite(sheet, glm::vec2(0, 0), glm::vec2(2, 0), glm::vec2(this->width, 22));

	// drawing ammo interface
	sprRenderer->drawSprite(ResourceManager::getSpriteSheet("ammo"), glm::vec2(this->width - 110, this->height - 47), glm::vec2(0, 0), glm::vec2(80, 25));

	// now the text
	std::stringstream ss;
	ss << player->health;
	textRenderer->renderText("SHIP INTEGRITY: " + ss.str() + "%", 33.0f, this->height - 38, 0.8f, healthColor);

	ss.str(""); // clear the stream before entering new data
	ss << this->score;

	textRenderer->renderText("MERIT: " + ss.str(), 33.0f, this->height - 56, 0.8f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

	ss.str("");
	ss << this->wave;

	textRenderer->renderText("WAVE: " + ss.str(), 20.0f, 5, 0.8f, glm::vec4(this->waveFlux, this->waveFlux, this->waveFlux, 1.0f));

	ss.str("");
	ss << player->missiles;

	textRenderer->renderText(ss.str(), this->width - 51.0f, this->height - 37, 0.425f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));


	ss.str("");
	ss << player->mines;

	textRenderer->renderText(ss.str(), this->width - 91.0f, this->height - 37, 0.425f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

	if (player->multiplier == 2)
		textRenderer->renderText("x2", this->width - 68.0f, this->height - 63, 0.8f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));

	if (this->tutorialLife > 0)
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("tutorial"), glm::vec2(0, this->height/2 - 50), glm::vec2(0, 0), glm::vec2(this->width, 125 * scale), 0.0f, glm::vec4(1.0f, 1.0f, 1.0f, std::min(this->tutorialLife, 100.0f)/100.0f));

	if (waveTimer > 1400 && wave != 1)
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("incoming"), glm::vec2(0, this->height / 2 - 100), glm::vec2(0, 0), glm::vec2(this->width, 200 * scale), 0.0f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f - (1500 - waveTimer)/100.0f));

	if (this->state == LOSE)
	{
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("outcome"), glm::vec2(0, this->height / 2 - 200), glm::vec2(0, 0), glm::vec2(this->width, 400 * scale));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(0, 0), glm::vec2(200, 50));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(1, 0), glm::vec2(200, 50));
	}
	else if (this->state == WIN)
	{
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("outcome"), glm::vec2(0, this->height / 2 - 200), glm::vec2(1, 0), glm::vec2(this->width, 400 * scale));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(0, 0), glm::vec2(200, 50));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(1, 0), glm::vec2(200, 50));
	}
	else if (this->state == PAUSE)
	{
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("overlay"), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(this->width, this->height));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("text"), glm::vec2(0, 200 * scale), glm::vec2(0, 0), glm::vec2(this->width, 50 * scale));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(4, 0), glm::vec2(200, 50));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(2, 0), glm::vec2(200, 50));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 250), glm::vec2(1, 0), glm::vec2(200, 50));
	}
	else if (this->state == OPTIONS2)
	{
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("overlay"), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(this->width, this->height));
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("text"), glm::vec2(0, 200 * scale), glm::vec2(1, 0), glm::vec2(this->width, 50 * scale));

		if (postProcessor->shakeEnabled)
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("checkbox"), glm::vec2(this->width / 2 - 10, this->height / 2 - 100), glm::vec2(0, 1), glm::vec2(20, 20));  // shake toggle
		else
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("checkbox"), glm::vec2(this->width / 2 - 10, this->height / 2 - 100), glm::vec2(0, 0), glm::vec2(20, 20));

		if (postProcessor->bloom)
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("checkbox"), glm::vec2(this->width / 2 - 10, this->height / 2 - 50), glm::vec2(0, 1), glm::vec2(20, 20));  // bloom toggle
		else
			sprRenderer->drawSprite(ResourceManager::getSpriteSheet("checkbox"), glm::vec2(this->width / 2 - 10, this->height / 2 - 50), glm::vec2(0, 0), glm::vec2(20, 20));

		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("bar"), glm::vec2(this->width / 2 - 100, this->height / 2), glm::vec2(0, 0), glm::vec2(200, 15));  // music volume
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("bar"), glm::vec2(this->width / 2 - 100 + musicVolume - 5, this->height / 2), glm::vec2(1, 0), glm::vec2(200, 15));

		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("bar"), glm::vec2(this->width / 2 - 100, this->height / 2 + 50), glm::vec2(0, 0), glm::vec2(200, 15));  // sfx volume
		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("bar"), glm::vec2(this->width / 2 - 100 + effectsVolume - 5, this->height / 2 + 50), glm::vec2(1, 0), glm::vec2(200, 15));

		sprRenderer->drawSprite(ResourceManager::getSpriteSheet("button"), glm::vec2(this->width / 2 - 100, this->height / 2 + 125), glm::vec2(5, 0), glm::vec2(200, 50));

		textRenderer->renderText("Shake", this->width / 2 - 250, this->height / 2 - 100, 0.6f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
		textRenderer->renderText("Bloom", this->width / 2 - 250, this->height / 2 - 50, 0.6f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
		textRenderer->renderText("Music Volume", this->width / 2 - 250, this->height / 2, 0.6f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
		textRenderer->renderText("SFX Volume", this->width / 2 - 250, this->height / 2 + 50, 0.6f, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
	}
}

void Game::spawnEnemies(GLfloat dt)
{
	if (enemyTimer <= 0)
	{
		// a couple seconds of peace between wave transitions
		if (this->waveTimer > 50 && this->waveTimer < 1400)
		{

			if (rand() % this->eSpawnRates[0] == 1)
				enemies.push_front(new Destroyer());

			if (rand() % this->eSpawnRates[1] == 1)
				enemies.push_front(new Leviathan());

			if (rand() % this->eSpawnRates[2] == 1)
				enemies.push_front(new Vanguard());

			if (rand() % this->eSpawnRates[3] == 1)
				enemies.push_front(new Reaver());

			if (rand() % this->eSpawnRates[4] == 1)
				enemies.push_front(new Mincer());

			if (rand() % this->eSpawnRates[5] == 1)
				enemies.push_front(new Harbinger());

			if (rand() % this->eSpawnRates[6] == 1)
				enemies.push_front(new Cruiser());

			if (rand() % this->eSpawnRates[7] == 1)
				asteroids.push_front(new Asteroid());
		}
		enemyTimer = 5;
	}
	else
		enemyTimer -= 5 * dt;
}

void Game::spawnPowerUps(glm::vec2 pos)
{
	GLint select = rand() % 10;
	switch (select)
	{
	case(0) :
		if (rand() % this->puSpawnRates[0] == 1)
			powerups.push_front(new PowerUp(pos, 'h', "+50 INTEGRITY"));  // health
		break;
	case(1):
		if (rand() % this->puSpawnRates[1] == 1)
			powerups.push_front(new PowerUp(pos, 't', "TRIPLE LASERS"));  // triple lasers
		break;
	case(2):
		if (rand() % this->puSpawnRates[2] == 1)
			powerups.push_front(new PowerUp(pos, 'b', "BEAM"));  // beam
		break;
	case(3):
		if (rand() % this->puSpawnRates[3] == 1)
			powerups.push_front(new PowerUp(pos, 's', "SHIELD"));  // shield
		break;
	case(4):
		if (rand() % this->puSpawnRates[4] == 1)
			powerups.push_front(new PowerUp(pos, 'm', "x2 MERIT"));  // x2 multiplier
		break;
	case(5):
		if (rand() % this->puSpawnRates[5] == 1)
			powerups.push_front(new PowerUp(pos, 'f', "FASTER FIRERATE"));  // faster firing
		break;
	case(6):
		if (rand() % this->puSpawnRates[6] == 1)
			powerups.push_front(new PowerUp(pos, 'd', "DOUBLE DAMAGE"));  // x2 damage
		break;
	case(7):
		if (rand() % this->puSpawnRates[7] == 1)
			powerups.push_front(new PowerUp(pos, 'i', "+1 MINE"));  // mine
		break;
	case(8):
		if (rand() % this->puSpawnRates[8] == 1)
			powerups.push_front(new PowerUp(pos, 'r', "+1 MISSILE"));  // missile
		break;
	case(9):
		if (rand() % this->puSpawnRates[9] == 1)
			powerups.push_front(new PowerUp(pos, 'p', "PELLET SHOT"));  // pellet shot
		break;
	}
}

void Game::getMusic() // extract music
{
	DIR *dir;
	struct dirent *ent;
	GLint count = 0; // actual files start appearing after 2nd iteration when reading directory, so only extract the file name if past that

	if ((dir = opendir("sounds/music/")) != NULL)
	{
		while ((ent = readdir(dir)))
		{
			if (count > 1)
			{
				tracks.push_back(std::string("sounds/music/") + ent->d_name);
				std::cout << ent->d_name << " loaded" << std::endl;

			}
			count++;
		}
		closedir(dir);
	}
	else
	{
		std::cout << "Couldn't find music directory" << std::endl;
		tracks.push_back(std::string(""));
	}
}

std::string Game::selectTrack() // randomly select a track from the track list
{
	if (tracks.size() > 0)
	{
		this->currentTrack = rand() % tracks.size();
		return tracks[currentTrack];
	}
	return "";
}

void Game::saveSettings()
{
	std::ofstream cfg;
	cfg.open("cfg/settings.cfg", std::ofstream::out | std::ofstream::trunc);
	if (cfg.is_open())
	{
		if (postProcessor->shakeEnabled)
			cfg << "SHAKE:true" << std::endl;
		else
			cfg << "SHAKE:false" << std::endl;

		if (postProcessor->bloom)
			cfg << "BLOOM:true" << std::endl;
		else
			cfg << "BLOOM:false" << std::endl;
		cfg << "MUSIC:" << musicVolume << std::endl;
		cfg << "SFX:" << effectsVolume << std::endl;
		cfg << "SCALE:" << scale << std::endl;
		cfg.close();
		std::cout << "Settings saved" << std::endl;
	}
	else
		std::cout << "Failed to open settings file" << std::endl;
}

void Game::readSettings()
{
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
				switch (i)
				{
				case 0:
					if (line == "true")
						postProcessor->shakeEnabled = GL_TRUE;
					else
						postProcessor->shakeEnabled = GL_FALSE;
					break;
				case 1:
					if (line == "true")
						postProcessor->bloom = GL_TRUE;
					else
						postProcessor->bloom = GL_FALSE;
					break;
				case 2:
					musicVolume = std::stoi(line);
					break;
				case 3:
					effectsVolume = std::stoi(line);
					break;
				}
				i++;
			}
		}

		std::cout << "Settings loaded" << std::endl;
		file.close();
	}
	else
		std::cout << "Failed to open settings file" << std::endl;
}

void Game::updateSpawnRates(GLint wave) // get spawn rates for power up and enemies for current wave from config files
{
	std::ifstream pufile("cfg/power_up_rates.cfg");
	GLint i = 0;
	std::string line;

	if (pufile.is_open())
	{
		while (getline(pufile, line))
		{
			if (line == "WAVE " + std::to_string(wave))
			{
				while (getline(pufile, line))
				{
					if (line.find(':') != std::string::npos)
					{
						line = line.substr(line.find(':') + 1);
						this->puSpawnRates[i] = std::stoi(line);
						i++;
					}
				}
			}
			i = 0;
		}
		pufile.close();
	}
	else
		std::cout << "Failed to open power up rates file" << std::endl;

	std::ifstream efile("cfg/enemy_rates.cfg");

	if (efile.is_open())
	{
		while (getline(efile, line))
		{
			if (line == "WAVE " + std::to_string(wave))
			{
				while (getline(efile, line))
				{
					if (line.find(':') != std::string::npos)
					{
						line = line.substr(line.find(':') + 1);
						this->eSpawnRates[i] = std::stoi(line);
						i++;
					}
				}
			}
		}
		efile.close();
	}
	else
		std::cout << "Failed to open enemy rates file" << std::endl;
}

void Game::resetGame()
{
	while (!projectiles.empty()) delete projectiles.front(), projectiles.pop_front();
	while (!enemies.empty()) {
		if (enemies.front()->beam != nullptr)
			enemies.front()->beam->active = GL_FALSE;
		delete enemies.front();
		enemies.pop_front();
	}
	if (player->beam != nullptr)
		player->beam->active = GL_FALSE;

	updateBeams(0); // stop any beam sounds currently playing
	while (!beams.empty()) delete beams.front(), beams.pop_front();
	while (!missiles.empty()) delete missiles.front(), missiles.pop_front();
	while (!asteroids.empty()) delete asteroids.front(), asteroids.pop_front();
	while (!powerups.empty()) delete powerups.front(), powerups.pop_front();

	delete player;
	player = new Player(this->width, this->height);
	this->waveTimer = 1500.0f;
	this->wave = 1;
	this->updateSpawnRates(this->wave);
	this->score = 0;
	shakeTime = 0;

	engineParticleGenerator->removeParticles();
	enemyParticleGenerator->removeParticles();
	hitParticleGenerator->removeParticles();
	fragmentParticleGenerator->removeParticles();
	smokeParticleGenerator->removeParticles();
	sparkParticleGenerator->removeParticles();
	explosionParticleGenerator->removeParticles();
	asteroidParticleGenerator->removeParticles();
}