#include "Game.h"
#include <iostream>
#include <math.h>

Game::Game() : already_won(false), last_move(LastMove::None) // bug receving weird numbers 
{
	stateOfGame = GameState::Playing;
	tiles_moveset = std::make_unique<TileOperation>();
	window.create(sf::VideoMode(800, 600), "Title");
	prepareBase();
	prepareTile(); 
	prepareResetButton();
	genericFont.loadFromFile("Montserrat-Regular.otf");

	while (window.isOpen())
	{
		drawElements();
		window.display();
		handleEvents();
	}
}

void Game::handleEvents()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			window.close();

		if (stateOfGame == GameState::Playing)
			handleNormalEvents(event);
		
		if (stateOfGame == GameState::GameOver)
			handleGameOverEvents(event);

		if (stateOfGame == GameState::Win)
			handleWinEvents(event);
	}
}

void Game::handleNormalEvents(const sf::Event& event)
{
	if (event.type == sf::Event::MouseMoved)
	{
		int a = event.mouseMove.x;
		int b = event.mouseMove.y;

		sf::Cursor newCursor;

		if (resetButton.getGlobalBounds().contains(sf::Vector2f(a, b)))
			newCursor.loadFromSystem(sf::Cursor::Type::Hand);
		else
			newCursor.loadFromSystem(sf::Cursor::Type::Arrow);

		window.setMouseCursor(newCursor);
		return;
	}

	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			int a = event.mouseButton.x;
			int b = event.mouseButton.y;
			if (resetButton.getGlobalBounds().contains(sf::Vector2f(a, b)))
			{
				resetGame();
				updateTiles();
				updateValues();
			}
		}
	}

	if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Left:
			last_move = LastMove::Left;
			tiles_moveset->leftSlide();
			display_animation_H();
			break;

		case sf::Keyboard::Right:
			last_move = LastMove::Right;
			tiles_moveset->rightSlide();
			display_animation_H();
			break;

		case sf::Keyboard::Up:
			last_move = LastMove::Up;
			tiles_moveset->topSlide();
			display_animation_V();
			break;

		case sf::Keyboard::Down:
			last_move = LastMove::Down;
			tiles_moveset->bottomSlide();
			display_animation_V();
			break;

		case sf::Keyboard::Escape:
			window.close();
			break;

		default:
			last_move = LastMove::None;
		}
		size_t index = tiles_moveset->addNewRandomTile();
		display_animation_popup(index);

		checkMoves();
		checkWin();
		updateTiles();
		updateValues();
	}
}

void Game::handleGameOverEvents(const sf::Event& event)
{
	if (event.type == sf::Event::MouseMoved)
	{
		int a = event.mouseMove.x;
		int b = event.mouseMove.y;

		sf::Cursor newCursor;
		if (endGame_button.getGlobalBounds().contains(sf::Vector2f(a, b)))
			newCursor.loadFromSystem(sf::Cursor::Type::Hand);
		else
			newCursor.loadFromSystem(sf::Cursor::Type::Arrow);

		window.setMouseCursor(newCursor);
		return;
	}

	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			int a = event.mouseButton.x;
			int b = event.mouseButton.y;
			if (endGame_button.getGlobalBounds().contains(sf::Vector2f(a, b)))
			{
				resetGame();
				updateTiles();
				updateValues();
			}
		}
	}

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		window.close();
	
}

void Game::handleWinEvents(const sf::Event& event)
{
	if (event.type == sf::Event::MouseMoved)
	{
		int a = event.mouseMove.x;
		int b = event.mouseMove.y;

		sf::Cursor newCursor;
		if (endGame_button.getGlobalBounds().contains(sf::Vector2f(a, b)) ||
			continue_button.getGlobalBounds().contains(sf::Vector2f(a, b)))
			newCursor.loadFromSystem(sf::Cursor::Type::Hand);
		else
			newCursor.loadFromSystem(sf::Cursor::Type::Arrow);

		window.setMouseCursor(newCursor);
		return;
	}

	if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
		{
			int a = event.mouseButton.x;
			int b = event.mouseButton.y;
			if (endGame_button.getGlobalBounds().contains(sf::Vector2f(a, b)))
			{
				resetGame();
				updateTiles();
				updateValues();
			}

			else if (continue_button.getGlobalBounds().contains(sf::Vector2f(a, b)))
			{
				keepPlaying();
			}
		}
	}

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		window.close();
}

void Game::drawElements()
{
	window.clear(sf::Color(62, 62, 66));
	drawBase();
	drawTile();
	window.draw(resetButton);
	
	if (stateOfGame == GameState::GameOver)
		draw_GameOverElements();

	if (stateOfGame == GameState::Win)
		draw_WinElements();
}

void Game::prepareBase()
{
	size_t a = 130; // length of square sides
	size_t vertical_distance = 16;
	size_t horizontal_distance = 16;

	size_t i = 0;
	for (; i < base.size(); ++i)
	{
		base[i].setSize(sf::Vector2f(a, a));
		size_t pos_x = vertical_distance + (vertical_distance * (i % row)) + (a * (i % row));
		size_t pos_y = horizontal_distance + (horizontal_distance * (i / column)) + (a * (i / column));
		base[i].setPosition(pos_x, pos_y);
		base[i].setFillColor(sf::Color(11846093));
	}
}

void Game::drawBase()
{
	for (auto curr = base.cbegin(); curr < base.cend(); ++curr)
	{
		window.draw(*curr);
	}
}

void Game::prepareTile()
{
	size_t a = 130; // length of square sides
	size_t vertical_distance = 16;
	size_t horizontal_distance = 16;

	auto [start, end] = tiles_moveset->getTilesRange();

	size_t i = 0;
	for (; start < end; ++start, ++i)
	{
		Tile& tile = tiles_moveset->getTile(i);
		tile.setSize(sf::Vector2f(a, a));
		size_t pos_x = vertical_distance + (vertical_distance * (i % row)) + (a * (i % row));
		size_t pos_y = horizontal_distance + (horizontal_distance * (i / column)) + (a * (i / column));
		tile.setPosition(pos_x, pos_y);
		auto color = tile.getColor();
		tile.setFillColor(color);
		
	}

	prepareValues();
}

void Game::drawTile()
{
	auto [start, end] = tiles_moveset->getTilesRange();

	for (start; start < end; ++start)
	{
		sf::RectangleShape shape = (*start).get_shape();
		window.draw(shape);
	}

	drawValues();
}

void Game::updateTiles() 
{
	auto [curr, end] = tiles_moveset->getTilesRange();
	for (; curr < end; ++curr)
	{
		curr->updateFillColor();
	}

}

void Game::prepareValues() 
{
	auto [start, end] = tiles_moveset->getTilesRange();

	size_t i = 0;
	for (; start < end; ++start, ++i)
	{
		Tile& tile = tiles_moveset->getTile(i);
		sf::Text & text = tile.get_text();

		tile.setValueFont(genericFont);
		tile.setTextColor(sf::Color::White);
		tile.setCharacterSize(40);

		int value = tile.get_value();
		if (value != 0)
			tile.setValueText(std::to_string(value));

		sf::RectangleShape shape = (*start).get_shape();
		const sf::Vector2f& pos = shape.getPosition();

		tile.setValuePosition(pos.x + 52, pos.y + 41.3); 

	}

}

void Game::drawValues()
{
	auto [start, end] = tiles_moveset->getTilesRange();

	for (start; start < end; ++start)
	{
		sf::Text text = (*start).get_text();
		window.draw(text);
	} 
}

void Game::updateValues()
{
	auto [start, end] = tiles_moveset->getTilesRange();

	size_t i = 0;
	for (; start < end; ++start, ++i)
	{
		Tile& tile = tiles_moveset->getTile(i);
		int value = tile.get_value();

		if (value != 0)
			tile.setValueText(std::to_string(value));
		else
			tile.setValueText(std::string());


		const sf::RectangleShape& shape = (*start).get_shape();
		const sf::Vector2f& pos = shape.getPosition();


		size_t string_size = tile.get_text().getString().getSize();

		float pos_x = pos.x + 63.5 - ((string_size * 9.5) +
			(string_size - 1.0 * tile.get_text().getLetterSpacing()));

		tile.setValuePosition(pos_x, pos.y + 41.3);
	}
}

void Game::updateTile(size_t i) // might be deleted
{
	size_t a = 130; 
	size_t vertical_distance = 16;
	size_t horizontal_distance = 16;

	Tile& tile = tiles_moveset->getTile(i);
	size_t pos_x = vertical_distance + (vertical_distance * (i % row)) + (a * (i % row));
	size_t pos_y = horizontal_distance + (horizontal_distance * (i / column)) + (a * (i / column));
	tile.setPosition(pos_x, pos_y);;
}

void Game::updateValue(size_t index)
{
	Tile& tile = tiles_moveset->getTile(index);
	int value = tile.get_value();

	if (value != 0)
		tile.setValueText(std::to_string(value));
	else
		tile.setValueText(std::string());


	const sf::RectangleShape& shape = tile.get_shape();
	const sf::Vector2f& pos = shape.getPosition();


	size_t string_size = tile.get_text().getString().getSize();

	float pos_x = pos.x  + 63.5 - ((string_size * 9.5) +
		(string_size - 1.0 * tile.get_text().getLetterSpacing()));

	tile.setValuePosition(pos_x, pos.y + 41.3);
}

void Game::prepareResetButton()
{
	resetButton.setButtonSize(sf::Vector2f(184, 120));
	resetButton.setPosition(600, 16);
	resetButton.setOutlineThickness(3);
	resetButton.setOutlineColor(sf::Color::Black);
	resetButton.setColor(sf::Color(0, 187, 249));

	resetButton.setTextFont(genericFont);
	resetButton.setText("New Game");
	resetButton.setTextColor(sf::Color::White);
}

void Game::prepareContinueButton()
{
	continue_button.setButtonSize(sf::Vector2f(184, 120));
	continue_button.setPosition(200, 240);
	continue_button.setOutlineThickness(3);
	continue_button.setOutlineColor(sf::Color::Black);
	continue_button.setColor(sf::Color(0, 187, 249));

	continue_button.setTextFont(genericFont);
	continue_button.setText("Continue");
	continue_button.setTextColor(sf::Color::White);
}

void Game::resetGame()
{
	stateOfGame = GameState::Playing;
	last_move = LastMove::None;

	tiles_moveset->reset();
	prepareTile();
	prepareValues();
}

void Game::checkMoves()
{
	bool can_move = tiles_moveset->checkMove_possibility();

	if (!can_move)
	{
		stateOfGame = GameState::GameOver;
	}
	
}

void Game::checkWin()
{
	if (already_won)
		return;

	if (tiles_moveset->checkWin())
		stateOfGame = GameState::Win;
}

void Game::prepareStatusString(sf::Text& gameStatus)
{
	gameStatus.setFont(genericFont);
	gameStatus.setFillColor(sf::Color::Black);
	gameStatus.setCharacterSize(60);
	gameStatus.setOutlineThickness(1);

	gameStatus.setPosition(220, 150);
	
	if (stateOfGame == GameState::GameOver)
	{
		gameStatus.setString("Game Over");
		gameStatus.setPosition(220, 150);
	}
	else
	{
		gameStatus.setString("You win!");
		gameStatus.setPosition(270, 150);
	}

}

void Game::keepPlaying()
{
	stateOfGame = GameState::Playing;
}

void Game::draw_WinElements()
{
	sf::RectangleShape gameOver_layout(sf::Vector2f(window.getSize()));
	gameOver_layout.setFillColor(sf::Color(221, 193, 113, 210));
	window.draw(gameOver_layout);

	sf::Text gameStatus;
	prepareStatusString(gameStatus);
	window.draw(gameStatus);

	prepareContinueButton();
	window.draw(continue_button);

	endGame_button = resetButton;
	endGame_button.setPosition(400, 240);
	window.draw(endGame_button);
}

void Game::draw_GameOverElements()
{
	sf::RectangleShape gameOver_layout(sf::Vector2f(window.getSize()));
	gameOver_layout.setFillColor(sf::Color(255, 255, 255, 210));
	window.draw(gameOver_layout);

	sf::Text gameStatus;
	prepareStatusString(gameStatus);
	window.draw(gameStatus);

	endGame_button = resetButton;
	endGame_button.setPosition(308, 240);
	window.draw(endGame_button);
}
//***************************************************************************************************
void Game::display_animation_popup(size_t index)
{
	if (index != -1)
	{
		expandTileAnimation(index);
		shrinkTileAnimation(index);
	}
}

void Game::display_animation_H()
{
	if (last_move == LastMove::Left)  
	{
		display_distance_animation_L();
		display_addition_animation_L();
	}
	else
	{
		display_distance_animation_R();
		display_addition_animation_R(); 
	}
}

void Game::display_distance_animation_L()
{
	std::vector<std::pair<int, int>> distance_animation =
		tiles_moveset->get_distance_animation_H();

	if (distance_animation.empty())
		return;

	std::vector<float> distance_animation_offset = getAnimationOffset_H(distance_animation);

	for (size_t i = 0; i < distance_animation.size(); ++i)
		setStartOfAnimation(distance_animation[i].first, distance_animation_offset[i]);

	float speed = (2 * distance_animation.size());

	while (!distance_animation.empty()) 
	{
		drawElements();
		window.display();
		for (size_t i = 0; i < distance_animation.size(); ++i)
		{
			int tile_index = distance_animation[i].first;
			Tile& tile = tiles_moveset->getTile(tile_index);

			tile.move(-speed, 0.0);
			distance_animation_offset[i] -= speed;

			if (distance_animation_offset[i] <= 0.0)
			{
				updateTile(tile_index);
				distance_animation.erase(distance_animation.cbegin() + i);
				distance_animation_offset.erase(distance_animation_offset.cbegin() + i);
			}
			updateValues();
			updateTiles();
		}
	}
}

void Game::display_addition_animation_L() 
{
	std::vector<std::pair<int, int>> addition_animation =
		tiles_moveset->get_addition_animation_H();

	if (addition_animation.empty())
		return;

	std::vector<int> popup_animation = get_popup_points_H(addition_animation);

	addition_move_animation_L(addition_animation);
	tiles_moveset->removeDistance_H(true);
	updateRowsPosition();
	updateTiles();
	expandTileAnimation(popup_animation);
	shrinkTileAnimation(popup_animation);
}

void Game::display_distance_animation_R()
{
	std::vector<std::pair<int, int>> distance_animation =
		tiles_moveset->get_distance_animation_H();

	if (distance_animation.empty())
		return;

	std::vector<float> distance_animation_offset = getAnimationOffset_H(distance_animation);

	for (size_t i = 0; i < distance_animation.size(); ++i)
		setStartOfAnimation(distance_animation[i].first, distance_animation_offset[i]);
		
	float speed = 2 * distance_animation.size();

	while (!distance_animation.empty())
	{
		drawElements();
		window.display();
		for (size_t i = 0; i < distance_animation.size(); ++i)
		{
			int tile_index = distance_animation[i].first;
			Tile& tile = tiles_moveset->getTile(tile_index);

			tile.move(speed, 0.0);
			distance_animation_offset[i] += speed;

			if (distance_animation_offset[i] >= 0.0)
			{
				updateTile(tile_index);
				distance_animation.erase(distance_animation.cbegin() + i);
				distance_animation_offset.erase(distance_animation_offset.cbegin() + i);
			}
			updateValues();
			updateTiles();
		}
	}
}

void Game::display_addition_animation_R()
{
	std::vector<std::pair<int, int>> addition_animation =
		tiles_moveset->get_addition_animation_H();

	if (addition_animation.empty())
		return;

	std::vector<int> popup_animation = get_popup_points_H(addition_animation);

	addition_move_animation_R(addition_animation);
	tiles_moveset->removeDistance_H(false);
	updateRowsPosition();
	updateTiles();
	expandTileAnimation(popup_animation);
	shrinkTileAnimation(popup_animation);
}

void Game::setStartOfAnimation(size_t index, int offsetX, int offsetY)
{
	Tile& tile = tiles_moveset->getTile(index);
	tile.move(offsetX, offsetY);
}

void Game::addition_move_animation_L(std::vector<std::pair<int, int>>& animation_points) 
{
	std::vector<float> distance_animation_offset = getAnimationOffset_H(animation_points); 
	std::vector<std::vector<Tile*>> tile_distance_move = get_additon_move_Tiles_L(animation_points);

	float speed = 2 * animation_points.size();

	while (!animation_points.empty())
	{
		drawElements();
		window.display();
		updateValues();
		for (size_t i = 0; i < animation_points.size(); ++i)
		{
			int tile_index = animation_points[i].second;
			Tile& tile = tiles_moveset->getTile(tile_index);

			tile.move(-speed, 0.0); 
			distance_animation_offset[i] -= speed;

			for (auto tile_next : tile_distance_move[i])
				tile_next->move(-speed, 0.0);


			if (distance_animation_offset[i] <= 0.0)
			{
				updateTile_addition(animation_points[i].first, animation_points[i].second); 
				animation_points.erase(animation_points.cbegin() + i);
				distance_animation_offset.erase(distance_animation_offset.cbegin() + i);
				tile_distance_move.erase(tile_distance_move.cbegin() + i);
			}
		}
	}
}

void Game::addition_move_animation_R(std::vector<std::pair<int, int>> animation_points)
{
	std::vector<float> distance_animation_offset = getAnimationOffset_H(animation_points);
	std::vector<std::vector<Tile*>> tile_distance_move = get_additon_move_Tiles_R(animation_points);

	float speed = 2 * animation_points.size();

	while (!animation_points.empty())
	{
		drawElements();
		window.display();
		updateValues();

		for (size_t i = 0; i < animation_points.size(); ++i)
		{
			int tile_index = animation_points[i].second;
			Tile& tile = tiles_moveset->getTile(tile_index);

			tile.move(speed, 0.0);
			distance_animation_offset[i] += speed;

			for (auto tile_next : tile_distance_move[i])
				tile_next->move(speed, 0.0);

			if (distance_animation_offset[i] >= 0.0)
			{
				updateTile_addition(animation_points[i].first, animation_points[i].second);
				animation_points.erase(animation_points.cbegin() + i);
				distance_animation_offset.erase(distance_animation_offset.cbegin() + i);
				tile_distance_move.erase(tile_distance_move.cbegin() + i);
			}
		}
	}
}

void Game::display_animation_V()
{
	if (last_move == LastMove::Up)
	{
		display_distance_animation_U();
		display_addition_animation_U();
	}
	else
	{
		display_distance_animation_D();
		display_addition_animation_D();
	}
}

void Game::display_distance_animation_U()
{
	std::vector<std::pair<int, int>> distance_animation =
		tiles_moveset->get_distance_animation_V();

	if (distance_animation.empty())
		return;

	std::vector<float> distance_animation_offset = getAnimationOffset_V(distance_animation);

	for (size_t i = 0; i < distance_animation.size(); ++i)
		setStartOfAnimation(distance_animation[i].first, 0, distance_animation_offset[i]);

	float speed = 2 * distance_animation.size();

	while (!distance_animation.empty())
	{
		drawElements();
		window.display();
		for (size_t i = 0; i < distance_animation.size(); ++i)
		{
			int tile_index = distance_animation[i].first;
			Tile& tile = tiles_moveset->getTile(tile_index);

			tile.move(0.0, -speed);
			distance_animation_offset[i] -= speed;

			if (distance_animation_offset[i] <= 0.0)
			{
				updateTile(tile_index);
				distance_animation.erase(distance_animation.cbegin() + i);
				distance_animation_offset.erase(distance_animation_offset.cbegin() + i);
			}
			updateValues();
			updateTiles();
		}
	}
}

void Game::display_addition_animation_U()
{
	std::vector<std::pair<int, int>> addition_animation =
		tiles_moveset->get_addition_animation_V();

	if (addition_animation.empty())
		return;

	std::vector<int> popup_animation = get_popup_points_V(addition_animation);

	addition_move_animation_U(addition_animation);
	tiles_moveset->removeDistance_V(true);
	updateRowsPosition();
	updateTiles();
	expandTileAnimation(popup_animation);
	shrinkTileAnimation(popup_animation);
}

void Game::addition_move_animation_U(std::vector<std::pair<int, int>> animation_points)
{
	std::vector<float> distance_animation_offset = getAnimationOffset_V(animation_points);
	std::vector<std::vector<Tile*>> tile_distance_move = get_additon_move_Tiles_U(animation_points);

	float speed = 2 * animation_points.size();

	while (!animation_points.empty())
	{
		drawElements();
		window.display();
		updateValues();
		for (size_t i = 0; i < animation_points.size(); ++i)
		{
			int tile_index = animation_points[i].second;
			Tile& tile = tiles_moveset->getTile(tile_index);

			tile.move(0.0, -speed);
			distance_animation_offset[i] -= speed;

			for (auto tile_next : tile_distance_move[i])
				tile_next->move(0.0, -speed);

			if (distance_animation_offset[i] <= 0.0)
			{
				updateTile_addition(animation_points[i].first, animation_points[i].second);
				animation_points.erase(animation_points.cbegin() + i);
				distance_animation_offset.erase(distance_animation_offset.cbegin() + i);
				tile_distance_move.erase(tile_distance_move.cbegin() + i);
			}
		}
	}
}

void Game::display_distance_animation_D()
{
	std::vector<std::pair<int, int>> distance_animation =
		tiles_moveset->get_distance_animation_V();

	if (distance_animation.empty())
		return;

	std::vector<float> distance_animation_offset = getAnimationOffset_V(distance_animation);

	for (size_t i = 0; i < distance_animation.size(); ++i)
		setStartOfAnimation(distance_animation[i].first, 0, distance_animation_offset[i]);

	float speed = 2 * distance_animation.size();

	while (!distance_animation.empty())
	{
		drawElements();
		window.display();
		for (size_t i = 0; i < distance_animation.size(); ++i)
		{
			int tile_index = distance_animation[i].first;
			Tile& tile = tiles_moveset->getTile(tile_index);

			tile.move(0.0, speed);
			distance_animation_offset[i] += speed;

			if (distance_animation_offset[i] >= 0.0)
			{
				updateTile(tile_index);
				distance_animation.erase(distance_animation.cbegin() + i);
				distance_animation_offset.erase(distance_animation_offset.cbegin() + i);
			}
			updateValues();
			updateTiles();
		}
	}
}

void Game::display_addition_animation_D() 
{
	std::vector<std::pair<int, int>> addition_animation =
		tiles_moveset->get_addition_animation_V();

	if (addition_animation.empty())
		return;

	std::vector<int> popup_animation = get_popup_points_V(addition_animation);

	addition_move_animation_D(addition_animation);
	tiles_moveset->removeDistance_V(false);
	updateRowsPosition();
	updateTiles();
	expandTileAnimation(popup_animation);
	shrinkTileAnimation(popup_animation);
}

void Game::addition_move_animation_D(std::vector<std::pair<int, int>> animation_points)
{
	std::vector<float> distance_animation_offset = getAnimationOffset_V(animation_points);
	std::vector<std::vector<Tile*>> tile_distance_move = get_additon_move_Tiles_D(animation_points);

	float speed = 2 * animation_points.size();

	while (!animation_points.empty())
	{
		drawElements();
		window.display();
		updateValues();
		for (size_t i = 0; i < animation_points.size(); ++i)
		{
			int tile_index = animation_points[i].second;
			Tile& tile = tiles_moveset->getTile(tile_index);

			tile.move(0.0, speed);
			distance_animation_offset[i] += speed;

			for (auto tile_next : tile_distance_move[i])
				tile_next->move(0.0, speed);

			if (distance_animation_offset[i] >= 0.0)
			{
				updateTile_addition(animation_points[i].first, animation_points[i].second);
				animation_points.erase(animation_points.cbegin() + i);
				distance_animation_offset.erase(distance_animation_offset.cbegin() + i);
				tile_distance_move.erase(tile_distance_move.cbegin() + i);
			}
		}
	}
}

void Game::updateTile_addition(size_t pos1, size_t pos2) 
{
	Tile& t1 = tiles_moveset->getTile(pos1);
	Tile& t2 = tiles_moveset->getTile(pos2);
	t1 += t2;
	t2 = 0;
	tiles_moveset->updateTiles(pos2);
	updateTile(pos2);
	updateValue(pos2);
	updateTiles();
}

std::vector<std::vector<Tile*>>
Game::get_additon_move_Tiles_L(const std::vector<std::pair<int, int>>& animation_points)  
{
	std::vector<std::vector<Tile*>> ret;

	for (size_t i = 0; i < animation_points.size(); ++i)
	{
		std::vector<Tile*> curr;

		int tile_index = animation_points[i].second;

		int end = tile_index % 4;
		end = row - end;

		for (size_t i = 1; i < end; ++i)
		{
			int	index = tile_index + i;

			if (index < 16)
			{
				Tile* tile_next = &tiles_moveset->getTile(index);

				if (tile_next->get_value() != 0)
					curr.push_back(tile_next);	
			}
		}
		ret.push_back(curr);

	}
	return ret;
}

std::vector<std::vector<Tile*>>
Game::get_additon_move_Tiles_R(const std::vector<std::pair<int, int>>& animation_points) 
{
	std::vector<std::vector<Tile*>> ret;

	for (size_t i = 0; i < animation_points.size(); ++i)
	{
		std::vector<Tile*> curr;

		int tile_index = animation_points[i].second;

		int end = tile_index % 4;
		

		for (size_t i = 1; i <= end; ++i)
		{
			int	index = tile_index - i;

			if (index >= 0)
			{
				Tile* tile_next = &tiles_moveset->getTile(index);

				if (tile_next->get_value() != 0)
					curr.push_back(tile_next);
			}
		}
		ret.push_back(curr);

	}
	return ret;
}

std::vector<std::vector<Tile*>>
Game::get_additon_move_Tiles_U(const std::vector<std::pair<int, int>>& animation_points)
{
	std::vector<std::vector<Tile*>> ret;

	for (size_t i = 0; i < animation_points.size(); ++i)
	{
		std::vector<Tile*> curr;

		int tile_index = animation_points[i].second;

		int end =  (row * (column - 1)) + (tile_index % 4);


		for (size_t i = tile_index + 4; i <= end; i+= 4)
		{
			Tile* tile_next = &tiles_moveset->getTile(i);

			if (tile_next->get_value() != 0)
				curr.push_back(tile_next);
			
		}
		ret.push_back(curr);

	}
	return ret;
}

std::vector<std::vector<Tile*>>
Game::get_additon_move_Tiles_D(const std::vector<std::pair<int, int>>& animation_points) 
{
	std::vector<std::vector<Tile*>> ret;

	for (size_t i = 0; i < animation_points.size(); ++i)
	{
		std::vector<Tile*> curr;

		int tile_index = animation_points[i].second;

		int end =  tile_index % 4;

		for (int i = tile_index - 4; i >= end; i -= 4) 
		{
			Tile* tile_next = &tiles_moveset->getTile(i);

			if (tile_next->get_value() != 0)
				curr.push_back(tile_next);

		}
		ret.push_back(curr);

	}
	return ret;
}

std::vector<int> Game::get_popup_points_H(const std::vector<std::pair<int, int>>& animation_points)
{
	std::vector<int> ret(1, animation_points[0].first);

	for (size_t i = 1; i < animation_points.size(); ++i)
	{
		int index = animation_points[i].first;

		int diff_dist = abs(ret[i - 1] - animation_points[i].first);

		if (diff_dist < 3)
		{
			int val;

			if (last_move == LastMove::Left)
				val = animation_points[i].first - 1;
			else
				val = animation_points[i].first + 1;

			ret.push_back(val);
		}
		else
			ret.push_back(animation_points[i].first);
		
	}
	return ret;
}

std::vector<int> Game::get_popup_points_V(const std::vector<std::pair<int, int>>& animation_points)
{
	std::vector<int> ret(1, animation_points[0].first);

	for (size_t i = 1; i < animation_points.size(); ++i)
	{
		int index = animation_points[i].first;

		int mod1 = animation_points[i - 1].first % 4;
		int mod2 = animation_points[i].first % 4;

		if (mod1 == mod2)
		{
			int val;

			if (last_move == LastMove::Up)
				val = animation_points[i].first - 4;
			else
				val = animation_points[i].first + 4;

			ret.push_back(val);
		}
		else
			ret.push_back(animation_points[i].first);
		
	}
	return ret;
}

void Game::updateRowsPosition()
{
	size_t a = 130; // length of square sides
	size_t vertical_distance = 16;
	size_t horizontal_distance = 16;

	auto [start, end] = tiles_moveset->getTilesRange();

	size_t i = 0;
	for (; start < end; ++start, ++i)
	{
		Tile& tile = tiles_moveset->getTile(i);
		size_t pos_x = vertical_distance + (vertical_distance * (i % row)) + (a * (i % row));
		size_t pos_y = horizontal_distance + (horizontal_distance * (i / column)) + (a * (i / column));
		tile.setPosition(pos_x, pos_y);		
	}
}

void Game::expandTileAnimation(size_t index)
{
	float growth = 5.0;
	while (growth > 0.0)
	{
		updateValues();
		drawElements();
		window.display();
		
		Tile& tile = tiles_moveset->getTile(index);
		tile.move(-0.250, -0.250);
		const sf::Vector2f prev_size = tile.getSize();
		tile.setSize(sf::Vector2f(prev_size.x + 0.25, prev_size.y + 0.25));

		growth -= 0.10;

		for (size_t i = 0; i < 400; ++i);
	}
}

void Game::expandTileAnimation(const std::vector<int >& animation_points)
{
	float growth = 5.0;

	float speed = 0.1 * animation_points.size();
	float change = 0.25 * animation_points.size();

	while (growth > 0.0)
	{
		updateValues();
		drawElements();
		window.display();
		for (size_t i = 0; i < animation_points.size(); ++i, growth -= speed)
		{
			size_t index = animation_points[i];
			Tile& tile = tiles_moveset->getTile(index);
			tile.move(-change, -change);
			const sf::Vector2f prev_size = tile.getSize();
			tile.setSize(sf::Vector2f(prev_size.x + change, prev_size.y + change));


			for (size_t i = 0; i < 400; ++i);
		}
	}
}

void Game::shrinkTileAnimation(size_t index)
{
	float growth = 0.0;

	while (growth <= 5.0)
	{
		updateValues();
		drawElements();
		window.display();
		
		Tile& tile = tiles_moveset->getTile(index);
		tile.move(0.250, 0.250);
		const sf::Vector2f prev_size = tile.getSize();
		tile.setSize(sf::Vector2f(prev_size.x - 0.25, prev_size.y - 0.25));

		growth += 0.10;

		for (size_t i = 0; i < 400; ++i);
	}
}

void Game::shrinkTileAnimation(const std::vector<int>& animation_points)
{
	float growth = 0.0;

	float speed = 0.1 * animation_points.size();
	float change = 0.25 * animation_points.size();

	while (growth <= 5.0)
	{
		drawElements();
		window.display();
		for (size_t i = 0; i < animation_points.size(); ++i, growth += speed)
		{
			size_t index = animation_points[i];
			Tile& tile = tiles_moveset->getTile(index);
			tile.move(change, change);
			const sf::Vector2f prev_size = tile.getSize();
			tile.setSize(sf::Vector2f(prev_size.x - change, prev_size.y - change));

			for (size_t i = 0; i < 400; ++i);
		}
	}
}

std::vector<float> Game::getAnimationOffset_H(const std::vector<std::pair<int, int>>& animation_points)
{
	std::vector<float> offset;

	if (animation_points.empty())
		return offset;

	for (auto curr = animation_points.cbegin(); curr < animation_points.cend();
		++curr)
	{
		int tile_distance, diffrence;

		diffrence = (*curr).second - (*curr).first;		
	
		tile_distance = (diffrence * 130) + (16 * diffrence);
		offset.push_back(tile_distance);
	}

	return offset;
}

std::vector<float> Game::getAnimationOffset_V(const std::vector<std::pair<int, int>>& animation_points)
{
	std::vector<float> offset;

	if (animation_points.empty())
		return offset;

	for (auto curr = animation_points.cbegin(); curr < animation_points.cend();
		++curr)
	{
		int tile_distance, diffrence;

		diffrence = (*curr).second - (*curr).first;

		tile_distance = (diffrence * 130) + (16 * diffrence);
		tile_distance /= 4;
		offset.push_back(tile_distance);
	}

	return offset;
}
//***************************************************************************************************