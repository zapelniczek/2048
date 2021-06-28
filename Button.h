#pragma once
#include <SFML/Graphics.hpp>


class Button : public sf::Drawable, sf::Transformable
{

private:
	sf::RectangleShape base;
	sf::Text button_text;
	

private:
	void setTextPosition(float, float);
	void setTextPosition(const sf::Vector2f&);

public:
	Button() = default;

	void setPosition(float, float);
	void setPosition(const sf::Vector2f&);

	void setButtonSize(const sf::Vector2f&);

	void setColor(sf::Color);

	void setOutlineThickness(float);
	void setOutlineColor(const sf::Color& color);

	sf::FloatRect getGlobalBounds()	const;

	sf::Vector2f getPosition();
	sf::Vector2f getPosition() const;


	void setTextFont(const sf::Font&);
	void setText(const std::string&);
	void setTextColor(const sf::Color& color);
	void setCharacterSize(unsigned int size);

	void draw(sf::RenderTarget& target,
		sf::RenderStates states = sf::RenderStates::Default) const override;

};

