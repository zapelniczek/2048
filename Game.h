#pragma once
#include "GameMoveset.h"
#include "Button.h"
#include "SFML/Graphics.hpp"

class Game // Finish animation functions + slower popup animation + 
	//make addition and get rid of distance in function animations
{
private:
	enum class GameState : int
	{
		Playing = 1,
		GameOver = 2,
		Win = 3
	};

	enum class LastMove : int
	{
		None = 0,
		Left = 1,
		Right = 2,
		Up = 3,
		Down = 4
	};

private:
	constexpr static size_t row = 4, column = 4;

	sf::Font genericFont; 
	std::array<sf::RectangleShape, 16> base;
	std::unique_ptr<TileOperation> tiles_moveset;
	sf::RenderWindow window;

	Button resetButton;
	Button endGame_button;
	Button continue_button;

	GameState stateOfGame;
	LastMove last_move;

	bool already_won;

private:
	void handleEvents();
	
	void handleNormalEvents(const sf::Event&);
	void handleGameOverEvents(const sf::Event&);
	void handleWinEvents(const sf::Event&);

	void drawElements();

	void prepareBase();
	void drawBase();

	void prepareTile();
	void drawTile();
	void updateTiles();

	void prepareValues();
	void drawValues();
	void updateValues();

	void updateTile(size_t pos);
	void updateValue(size_t pos);

	void prepareResetButton();
	void prepareContinueButton();
	
	void resetGame();

	void checkMoves();
	void checkWin();

	void prepareStatusString(sf::Text&);

	void keepPlaying();

	void draw_WinElements();
	void draw_GameOverElements();

	void display_animation_popup(size_t);

	void display_animation_H();

	void display_distance_animation_L();
	void display_addition_animation_L();

	void display_distance_animation_R();
	void display_addition_animation_R();

	void addition_move_animation_L(std::vector<std::pair<int, int>>&);
	void addition_move_animation_R(std::vector<std::pair<int, int>>);
	void updateTile_addition(size_t, size_t);

	void display_animation_V();

	void display_distance_animation_U();
	void display_addition_animation_U();

	void display_distance_animation_D();
	void display_addition_animation_D();

	void addition_move_animation_U(std::vector<std::pair<int, int>>);
	void addition_move_animation_D(std::vector<std::pair<int, int>>);

	std::vector<int> get_popup_points_H(const std::vector<std::pair<int, int>>&);
	std::vector<int> get_popup_points_V(const std::vector<std::pair<int, int>>&);

	std::vector<std::vector<Tile*>> 
		get_additon_move_Tiles_L(const std::vector<std::pair<int, int>>&);

	std::vector<std::vector<Tile*>>
		get_additon_move_Tiles_R(const std::vector<std::pair<int, int>>&);

	std::vector<std::vector<Tile*>>
	get_additon_move_Tiles_U(const std::vector<std::pair<int, int>>&);

	std::vector<std::vector<Tile*>>
		get_additon_move_Tiles_D(const std::vector<std::pair<int, int>>&);

	void setStartOfAnimation(size_t, int = 0, int = 0);

	void expandTileAnimation(size_t);
	void expandTileAnimation(const std::vector<int>&);

	void shrinkTileAnimation(size_t);
	void shrinkTileAnimation(const std::vector<int>&);

	void updateRowsPosition(); //maybe make it better later ?

	std::vector<float> getAnimationOffset_H(const std::vector<std::pair<int, int>>&);
	std::vector<float> getAnimationOffset_V(const std::vector<std::pair<int, int>>&);

public:
	Game();
};

