#include "Button.h"



void Button::draw(sf::RenderTarget& target,
	sf::RenderStates states) const
{
	target.draw(base);
	target.draw(button_text);
}

void Button::setPosition(float x, float y)
{
	base.setPosition(x, y);

	float a = x + 10;
	float b = y + 40;
	setTextPosition(a, b);
}

void Button::setPosition(const sf::Vector2f& pos)
{
	base.setPosition(pos);

	float a = pos.x + 10;
	float b = pos.y + 40;
	setTextPosition(a, b);
}

void Button::setButtonSize(const sf::Vector2f& size)
{
	base.setSize(size);
}

void Button::setColor(sf::Color color)
{
	base.setFillColor(color);
}

void Button::setOutlineThickness(float thickness)
{
	base.setOutlineThickness(thickness);
}

void Button::setOutlineColor(const sf::Color& color)
{
	base.setOutlineColor(color);
}

sf::FloatRect Button::getGlobalBounds()	const
{
	return base.getGlobalBounds();
}

sf::Vector2f Button::getPosition()
{
	return base.getPosition();
}

sf::Vector2f Button::getPosition() const
{
	return base.getPosition();
}

void Button::setTextPosition(float x, float y)
{
	button_text.setPosition(x, y);
}

void Button::setTextFont(const sf::Font& font)
{
	button_text.setFont(font);
}

void Button::setText(const std::string& text)
{
	button_text.setString(text);
}

void Button::setTextColor(const sf::Color& color)
{
	button_text.setFillColor(color);
}

void Button::setTextPosition(const sf::Vector2f& pos)
{
	button_text.setPosition(pos);
}

void Button::setCharacterSize(unsigned int size)
{
	button_text.setCharacterSize(size);
}