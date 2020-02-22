#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "sprite_sheet.h"
#include "sprite_renderer.h"
#include "resource_manager.h"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
	// Object state
	glm::vec2 position, size, velocity;
	glm::vec4 color;
	GLfloat rotation;
	glm::vec2 frame;
	// Render state
	SpriteSheet sprite;
	// Constructor(s)
	GameObject();
	GameObject::GameObject(glm::vec2 pos, glm::vec2 size, std::string spriteName, glm::vec4 color = glm::vec4(1.0f), glm::vec2 velocity = glm::vec2(0));
	GameObject::GameObject(glm::vec2 pos, glm::vec2 size);
	// Draw sprite
	virtual void draw(SpriteRenderer &renderer);
	virtual void draw(SpriteRenderer &renderer, GLuint row, GLuint column);
	virtual void move(GLfloat dt, GLuint width);
};

#endif