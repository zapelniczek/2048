#pragma once
#include "Tile.h"
#include <array>
#include <set>
#include <random>
#include <vector>
#include <atomic>


class TileOperation 
{
private:
	constexpr static size_t row = 4, column = 4;
	constexpr static size_t numberOfTiles = row * column;

	std::array<Tile, row* column> tiles;

	std::set<size_t> filled_tiles;
	std::set<size_t> free_tiles;

	std::default_random_engine rNumber;

	bool add_randomTile;

	mutable std::atomic<bool> move_availability;

	std::vector<std::pair<int, int>>  addition_animation_H;

	std::vector<std::pair<int, int>>  distance_animation_H;

	std::vector<std::pair<int, int>>  addition_animation_V;

	std::vector<std::pair<int, int>>  distance_animation_V;


private:
	void prepareGame()&;

	void can_move_H() const&;
	void can_move_V() const&;

	void updateTiles(size_t, size_t)&;

	int adjustRandomTile_Draw()&;

	template <typename Iter>
	void addAdjecentTiles_H(Iter, Iter)&;

	template <typename Iter>
	void removeDistanceHorinontaly(Iter, Iter)&;

	template <typename Iter>
	std::pair<size_t, size_t> findHorizontalPosition(Iter, Iter)&;

	template <typename Iter>
	std::vector<Tile> getHelperVector(Iter)&;

	template <typename Iter>
	std::vector<Tile> getHelperVector(Iter) const &;

	template <typename Iter>
	void addAdjecentTiles_V(Iter, int)&;

	template <typename Iter>
	void removeDistanceVerticaly(Iter, int)&;

	template <typename Iter>
	std::pair<size_t, size_t> findVerticalPosition(Iter, Iter, Iter, int, bool)&;

	void drawTiles()&;

	void register_addition_animation_H(size_t, size_t)&;
	void register_distance_animation_H(size_t, size_t)&;

	void register_addition_animation_V(size_t, size_t)&;
	void register_distance_animation_V(size_t, size_t)&;

	void clear_animation_entries_H();
	void clear_animation_entries_V();

public:
	TileOperation();
	TileOperation(const TileOperation&) = delete;
	TileOperation& operator= (const TileOperation&) = delete;

	bool checkMove_possibility()&;

	bool checkWin()&;

	void reset()&;

	int addNewRandomTile()&;

	void updateTiles(size_t)&;
	

	std::pair<std::array<Tile, numberOfTiles>::iterator,
		std::array<Tile, numberOfTiles>::iterator> getTilesRange();

	Tile& getTile(size_t pos);

	void removeDistance_H(bool);
	void removeDistance_V(bool);

	std::vector<std::pair<int, int>> get_addition_animation_H() const;
	std::vector<std::pair<int, int>> get_distance_animation_H() const;

	std::vector<std::pair<int, int>> get_addition_animation_V() const;
	std::vector<std::pair<int, int>> get_distance_animation_V() const;

	void leftSlide()&;
	void rightSlide()&;
	void topSlide()&;
	void bottomSlide()&;
};

