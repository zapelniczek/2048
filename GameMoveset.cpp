#include "GameMoveset.h"
#include "Header.h"
#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <algorithm>
#include <thread>
#include <future>

using namespace std::chrono_literals;

TileOperation::TileOperation() : rNumber(std::chrono::system_clock::now().time_since_epoch().count()), add_randomTile(true)
{
	prepareGame();
	drawTiles();
}

void TileOperation::prepareGame()&
{
	for (size_t i = 0; i < tiles.size(); ++i)
		free_tiles.insert(i);

	for (size_t i = 0; i < 2; ++i, add_randomTile = true)
		addNewRandomTile();

	add_randomTile = false;
}

void TileOperation::drawTiles()&
{
	system("cls");

	for (size_t pos = 1; pos <= row * column; ++pos)
	{
		std::cout << tiles[pos - 1].get_value() << " ";
		if (!(pos % row))
			std::cout << "\n";
	}
}

int TileOperation::addNewRandomTile()&
{

	if (free_tiles.size() == 0 || add_randomTile == false)
		return -1;

	int end_range = free_tiles.size() - 1;
	std::uniform_int_distribution rTile(0, end_range);

	auto it = std::next(free_tiles.cbegin(), rTile(rNumber));
	size_t tileID = free_tiles.extract(it).value();
	tiles[tileID] = adjustRandomTile_Draw();
	free_tiles.erase(tileID);
	filled_tiles.insert(tileID);

	add_randomTile = false;
	drawTiles();
	return tileID;
}

int TileOperation::adjustRandomTile_Draw()& 
{
	 float two_occurs = std::count(tiles.cbegin(), tiles.cend(), 2);
	 float four_occurs = std::count(tiles.cbegin(), tiles.cend(), 4);

	 float total_base = two_occurs + four_occurs;

	 std::bernoulli_distribution dist(0.1 );

	 if (dist(rNumber))
		 return 4;
	 else
		 return 2;
}

bool TileOperation::checkWin()&
{
	return std::count(tiles.cbegin(), tiles.cend(), 2048);
}

bool TileOperation::checkMove_possibility()&
{
	move_availability.store(true, std::memory_order_release); 
	if (free_tiles.size() == 0)
	{
		move_availability.store(false, std::memory_order_release);
		can_move_H();
		std::future moves_V = std::async(&TileOperation::can_move_V, this);
		moves_V.get();
	}

	return move_availability.load(std::memory_order_acquire);
}

void TileOperation::can_move_H() const&
{
	for (int r = 0; r < row && !move_availability.load(std::memory_order_acquire); ++r)
	{
		auto it_start = std::next(tiles.cbegin(), r * row);
		auto it_end = std::next(tiles.cbegin(), (r + 1) * row);

		auto it = std::adjacent_find(it_start, it_end);
			
		if (it != it_end)
			move_availability.store(true);
	}
}

void TileOperation::can_move_V() const&
{
	for (size_t c = 0; c < column  && !move_availability.load(std::memory_order_acquire); ++c)
	{
		auto it_start = std::next(tiles.begin(), c);
		std::vector<Tile> col_values = getHelperVector(it_start); 

		auto it = std::adjacent_find(col_values.cbegin(), col_values.cend());

		if (it != col_values.cend())
			move_availability.store(true);
	}
}

void TileOperation::updateTiles(size_t pos)&
{
	filled_tiles.erase(pos);
	free_tiles.insert(pos);
	drawTiles();
}

void TileOperation::updateTiles(size_t pos_freed, size_t pos_taken)&
{
	filled_tiles.erase(pos_freed);
	free_tiles.insert(pos_freed);

	free_tiles.erase(pos_taken);
	filled_tiles.insert(pos_taken);
	drawTiles();
}

Tile& TileOperation::getTile(size_t pos)
{
	return tiles[pos];
}

template <typename Iter>
std::pair<size_t, size_t> TileOperation::findHorizontalPosition(Iter current_it, Iter next_it)&
{
	size_t pos1, pos2;

	if (!is_reverse_iterator_v<Iter>)
	{
		pos1 = std::distance(static_cast<Iter>(tiles.begin()),
			current_it);
		pos2 = std::distance(static_cast<Iter>(tiles.begin()),
			next_it);
	}
	else
	{
		pos1 = std::distance(tiles.begin(),
			static_cast<std::reverse_iterator<std::_Array_iterator<Tile, row* column>>> (current_it).base() - 1);

		pos2 = std::distance(tiles.begin(),
			static_cast<std::reverse_iterator<std::_Array_iterator<Tile, row* column>>> (next_it).base() - 1);
	}
	return std::make_pair(pos1, pos2);
}

template <typename Iter>
void TileOperation::addAdjecentTiles_H(Iter it_start, Iter it_end)&
{
	for (; it_start < it_end - 1; std::advance(it_start, 2))
	{
		auto it = std::adjacent_find(it_start, it_end);
		auto x = it_start - (it_end - 1);

		if (it != it_end && *it != 0)
		{
			it_start = it; 
			auto [pos1, pos2] = findHorizontalPosition(it, it + 1);

			register_addition_animation_H(pos1, pos2);

			add_randomTile = true;
		}
	}
}

template <typename Iter>
void TileOperation::removeDistanceHorinontaly(Iter it_start, Iter it_end)& 
{
	for (auto current_it = it_start; current_it < it_end - 1; std::advance(current_it, 1))
	{
		auto next_it = current_it + 1;

		if (auto it = std::find(current_it, it_end - 1, Tile{ 0 }); it != it_end - 1)
		{
			current_it = it;
			next_it = std::find_if(current_it + 1, it_end, [](Tile& a) {return a.get_value() > 0; });

			if (next_it == it_end)
				return;
		}
		else
		{
			return;
		}
		auto [pos1, pos2] = findHorizontalPosition(current_it, next_it);
		tiles[pos1] = tiles[pos2];
		tiles[pos2] = 0;
		updateTiles(pos2, pos1);

		register_distance_animation_H(pos1, pos2);

		add_randomTile = true;
	}
}

void TileOperation::leftSlide()&
{
	clear_animation_entries_H();

	for (int r = row - 1; r >= 0; --r)
	{
		auto it_start = std::next(tiles.begin(), r * row);
		auto it_end = std::next(tiles.begin(), (r + 1) * row);

		removeDistanceHorinontaly(it_start, it_end);
		addAdjecentTiles_H(it_start, it_end);
	}
	drawTiles();
}

void TileOperation::rightSlide()&
{
	clear_animation_entries_H();

	for (int r = 0; r < row; ++r)
	{
		auto it_start = std::next(tiles.rbegin(), r * row);
		auto it_end = std::next(tiles.rbegin(), (r + 1) * row);

		removeDistanceHorinontaly(it_start, it_end);
		addAdjecentTiles_H(it_start, it_end);
	}
	drawTiles();
}

template <typename Iter>
std::vector<Tile> TileOperation::getHelperVector(Iter it_start)&
{
	std::vector<Tile> col_values(1, *it_start);

	for (size_t i = 0; i < column - 1; ++i)
	{
		std::advance(it_start, 4);
		col_values.push_back(*it_start);
	}

	return col_values;
}

template <typename Iter>
std::vector<Tile> TileOperation::getHelperVector(Iter it_start) const&
{
	std::vector<Tile> col_values(1, *it_start);

	for (size_t i = 0; i < column - 1; ++i)
	{
		std::advance(it_start, 4);
		col_values.push_back(*it_start);
	}
	return col_values;
}

template <typename Iter>
std::pair<size_t, size_t> TileOperation::findVerticalPosition(Iter containter_start, Iter it_1st, Iter it_2nd,
	int column_index, bool is_reversed)&
{
	size_t pos1, pos2;

	if (is_reversed)
	{
		pos1 = (row - 1) - std::distance(containter_start, it_1st);
		pos2 = (row - 1) - std::distance(containter_start, it_2nd);

		pos1 = pos1 * column + column_index;
		pos2 = pos2 * column + column_index;
	}
	else
	{
		pos1 = std::distance(containter_start, it_1st); // Local Vector position
		pos2 = std::distance(containter_start, it_2nd);

		pos1 = pos1 * row + column_index; // Tiles index position
		pos2 = pos2 * row + column_index;
	}

	return std::make_pair(pos1, pos2);
}

template <typename Iter>
void TileOperation::addAdjecentTiles_V(Iter it_start, int column_index)&  
{
	std::vector<Tile> col_values = getHelperVector(it_start);

	for (auto start = col_values.begin(); start < col_values.end() - 1; std::advance(start, 2))
	{
		auto it = std::adjacent_find(start, col_values.end());

		if (it != col_values.end() && *it != 0)
		{
			start = it;
			auto [pos1, pos2] = findVerticalPosition(col_values.begin(), it, it + 1,
				column_index, is_reverse_iterator_v<Iter>);

			register_addition_animation_V(pos1, pos2);

			add_randomTile = true;
		}
	}
}

template <typename Iter>
void TileOperation::removeDistanceVerticaly(Iter it_start, int column_index)& 
{
	std::vector<Tile> col_values = getHelperVector(it_start); 

	auto end_search = col_values.end();

	for (auto current_it = col_values.begin(); current_it < end_search - 1;
		std::advance(current_it, 1))
	{
		auto next_it = current_it + 1;
		if (auto it = std::find(current_it, end_search - 1, Tile{ 0 }); it != end_search - 1)
		{
			current_it = it;
			next_it = std::find_if(current_it + 1, end_search, [](Tile& a) {return a.get_value() > 0; });


			if (next_it == end_search)
				return;
		}
		else
		{
			return;
		}

		auto [pos1, pos2] = findVerticalPosition(col_values.begin(), current_it, next_it,
			column_index, is_reverse_iterator_v<Iter>);

		std::iter_swap(current_it, next_it); 

		tiles[pos1] = tiles[pos2];
		tiles[pos2] = 0;
		updateTiles(pos2, pos1);

		register_distance_animation_V(pos1, pos2);

		add_randomTile = true;
	}
}

void TileOperation::topSlide()&
{
	clear_animation_entries_V();

	for (size_t c = 0; c < column; ++c)
	{
		auto it_start = std::next(tiles.begin(), c);

		removeDistanceVerticaly(it_start, c);
		addAdjecentTiles_V(it_start, c);
	}
	drawTiles();
}

void TileOperation::bottomSlide()&
{
	clear_animation_entries_V();

	for (size_t c = 0; c < column; ++c)
	{
		auto it_start = std::next(tiles.rbegin(), c);

		removeDistanceVerticaly(it_start, column - 1 - c);
		addAdjecentTiles_V(it_start, column - 1 - c);
	}
	drawTiles();
}

void TileOperation::reset()&
{
	filled_tiles.clear();
	free_tiles.clear();

	for (size_t i = 0; i < tiles.size(); ++i)
	{
		tiles[i].reset();
	}

	add_randomTile = true;
	prepareGame();
}

void TileOperation::removeDistance_H(bool left_direct) // change addition + move 
{
	if (left_direct)
	{
		clear_animation_entries_H();

		for (int r = row - 1; r >= 0; --r)
		{
			auto it_start = std::next(tiles.begin(), r * row);
			auto it_end = std::next(tiles.begin(), (r + 1) * row);

			removeDistanceHorinontaly(it_start, it_end);
		}
	}
	else
	{
		clear_animation_entries_H();

		for (int r = 0; r < row; ++r)
		{
			auto it_start = std::next(tiles.rbegin(), r * row);
			auto it_end = std::next(tiles.rbegin(), (r + 1) * row);

			removeDistanceHorinontaly(it_start, it_end);
		}
	}

	drawTiles();
}

void TileOperation::removeDistance_V(bool up)
{
	if (up)
	{
		clear_animation_entries_V();

		for (size_t c = 0; c < column; ++c)
		{
			auto it_start = std::next(tiles.begin(), c);
			removeDistanceVerticaly(it_start, c);
		}
	}
	else
	{
		clear_animation_entries_V();

		for (size_t c = 0; c < column; ++c)
		{
			auto it_start = std::next(tiles.rbegin(), c);
			removeDistanceVerticaly(it_start, column - 1 - c);
		}
	}
}

void TileOperation::register_addition_animation_H(size_t pos1, size_t pos2)&
{
	addition_animation_H.push_back(std::make_pair(pos1, pos2));
}

void TileOperation::register_distance_animation_H(size_t pos1, size_t pos2)&
{
	distance_animation_H.push_back(std::make_pair(pos1, pos2));
}

void TileOperation::register_addition_animation_V(size_t pos1, size_t pos2)&
{
	addition_animation_V.push_back(std::make_pair(pos1, pos2));
}

void TileOperation::register_distance_animation_V(size_t pos1, size_t pos2)&
{
	distance_animation_V.push_back(std::make_pair(pos1, pos2));
}

std::vector<std::pair<int, int>> TileOperation::get_addition_animation_H() const
{
	return addition_animation_H;
}

std::vector<std::pair<int, int>> TileOperation::get_distance_animation_H() const
{
	return distance_animation_H;
}

std::vector<std::pair<int, int>> TileOperation::get_addition_animation_V() const
{
	return addition_animation_V;
}

std::vector<std::pair<int, int>> TileOperation::get_distance_animation_V() const
{
	return distance_animation_V;
}

void TileOperation::clear_animation_entries_H()
{
	addition_animation_H.clear();
	distance_animation_H.clear();
}

void TileOperation::clear_animation_entries_V()
{
	addition_animation_V.clear();
	distance_animation_V.clear();
}

std::pair<std::array<Tile, TileOperation::numberOfTiles>::iterator,
	std::array<Tile, TileOperation::numberOfTiles>::iterator> TileOperation::getTilesRange()
{
	return std::make_pair(tiles.begin(), tiles.end());
}
