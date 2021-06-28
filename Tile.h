#pragma once
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"
#include <unordered_map>
#include <memory>

class Tile
{
	friend void swap(Tile& lhs, Tile& rhs);

	struct TilesPalette
	{
		std::unordered_map<int, sf::Color> TilesColor;

		TilesPalette();
	};

private:
	sf::Text textValue;
	sf::RectangleShape shape;
	sf::Color color;
	int value;

	static inline std::unique_ptr<TilesPalette> palette = std::make_unique<TilesPalette>();

public:
	Tile();
	Tile(int a) : value(a) {}
	Tile(const Tile&);

	int get_value() { return value; }
	int get_value() const { return value; }
	sf::RectangleShape get_shape() { return shape; }
	sf::RectangleShape get_shape() const { return shape; }
	sf::Text& get_text() { return textValue; }


	void move(float offsetX, float offsetY);
	void move(const sf::Vector2f& offset);

	const sf::Vector2f& getOrigin() const;

	void setFillColor(const sf::Color&);
	sf::Color getColor();
	sf::Color getColor() const;

	void setSize(const sf::Vector2f&);
	sf::Vector2f getSize() const;
	void setPosition(float, float);
	void setPosition(const sf::Vector2f&);
	void updateFillColor();

	void setValueText(const std::string&);
	void setValuePosition(float, float);
	void setValuePosition(const sf::Vector2f&);
	void setCharacterSize(unsigned);
	void setTextColor(const sf::Color&);
	void setValueFont(const sf::Font&);

	void reset();

	Tile& operator= (Tile&);
	Tile& operator+= (const Tile& rhs);
	bool operator== (const Tile& rhs) const;
	bool operator!= (const Tile& rhs) const;
	bool operator != (int);
	bool operator== (int a);
	bool operator< (const Tile& rhs);
	Tile& operator= (const int);
};

