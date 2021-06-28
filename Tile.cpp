#include "Tile.h"
#include <fstream>
#include <iostream>

Tile::Tile() 
{
	palette = std::make_unique<TilesPalette>();
	color = palette->TilesColor[0];
	shape.setFillColor(color);
}

Tile::Tile(const Tile& rhs) : color(rhs.color), value(rhs.value)
{}

Tile::TilesPalette::TilesPalette()
{
	std::fstream file("TilesColor.txt");
	if (file.is_open())
	{
		int id;
		long number;
		while (file >> id && file >> number)
		{
			TilesColor.insert(std::make_pair(id, sf::Color(number)));
		}
		file.close();
	}
}

void Tile::reset()
{
	value = 0;
	color = palette->TilesColor[0];
	shape = sf::RectangleShape();
	textValue = sf::Text();
}

void Tile::setFillColor(const sf::Color& color)
{
	shape.setFillColor(color);
}

sf::Color Tile::getColor()
{
	return color;
}

sf::Color Tile::getColor() const
{
	return color;
}

void Tile::setSize(const sf::Vector2f& size)
{
	shape.setSize(size);
}

void Tile::setPosition(float x, float y)
{
	shape.setPosition(x, y);
}

void Tile::setPosition(const sf::Vector2f& position)
{
	shape.setPosition(position);
}

void Tile::updateFillColor()
{
	shape.setFillColor(color);
}

void Tile::setValueText(const std::string& text)
{
	textValue.setString(text);
}

void Tile::setValuePosition(float a , float b)
{
	textValue.setPosition(a, b);
}

void Tile::setValuePosition(const sf::Vector2f& pos)
{
	textValue.setPosition(pos);
}

void Tile::setCharacterSize(unsigned size)
{
	textValue.setCharacterSize(size);
}

void Tile::setTextColor(const sf::Color& color)
{
	textValue.setFillColor(color);
}

void Tile::setValueFont(const sf::Font& font)
{
	textValue.setFont(font);
}

const sf::Vector2f& Tile::getOrigin() const
{
	return shape.getOrigin();
}

sf::Vector2f Tile::getSize() const
{
	return shape.getSize();
}

void Tile::move(float offsetX, float offsetY)
{
	shape.move(offsetX, offsetY);
	textValue.move(offsetX, offsetY);
}
void Tile::move(const sf::Vector2f& offset)
{
	shape.move(offset);
	textValue.move(offset);
}


Tile& Tile::operator= (Tile& rhs) 
{
	value = rhs.value;
	color = rhs.color;
	rhs.color = palette->TilesColor[0];
	return *this;
}

Tile& Tile::operator+= (const Tile& rhs)
{
	value += rhs.value;
	color = palette->TilesColor[value];
	return *this;
}

Tile& Tile::operator= (const int a)
{
	value = a;
	color = palette->TilesColor[value];
	return *this;
}

bool Tile::operator== (const Tile& rhs) const 
{
	return value == rhs.value;
}

bool Tile::operator!= (const Tile& rhs) const
{
	return value != rhs.value;
}

bool Tile::operator != (int a)
{
	return value != a;
}

bool Tile::operator== (int a)
{
	return value != a;
}

bool Tile::operator< (const Tile& rhs)
{
	return value < rhs.value;
}

void swap(Tile& lhs, Tile& rhs)
{
	using std::swap;
	
	swap(lhs.textValue, rhs.textValue);
	swap(lhs.shape, rhs.shape);
	swap(lhs.color, rhs.color);
	swap(lhs.value, rhs.value);
}