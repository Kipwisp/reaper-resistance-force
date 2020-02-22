#include "game_object.h"

GameObject::GameObject(): position(0, 0), size(1, 1), velocity(0.0f), rotation(0.0f), color(1.0f), sprite()
{

}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, std::string spriteName, glm::vec4 color, glm::vec2 velocity)
	: position(pos), size(size), velocity(velocity), color(color), rotation(0.0f)
{
	this->sprite = ResourceManager::getSpriteSheet(spriteName);
	this->size = glm::vec2(sprite.spriteWidth, sprite.spriteHeight);
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size) : position(pos), size(size)
{

}

void GameObject::draw(SpriteRenderer &renderer)
{
	renderer.drawSprite(this->sprite, this->position, this->frame, this->size, this->rotation, this->color);
}

void GameObject::draw(SpriteRenderer &renderer, GLuint row, GLuint column)
{
	renderer.drawSprite(this->sprite, this->position, glm::vec2(row,column), this->size, this->rotation, this->color);
}

void GameObject::move(GLfloat dt, GLuint width)
{
	this->position.x += this->velocity.x * dt;
	this->position.y += this->velocity.y * dt;

	// wrap around screen
	if (this->position.x > width)
		this->position.x = -this->size.x;
	else if (this->position.x < -this->size.x)
		this->position.x = width;
}