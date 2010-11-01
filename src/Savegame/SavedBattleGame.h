/*
 * Copyright 2010 OpenXcom Developers
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM__SAVEDBATTLEGAME_H
#define OPENXCOM__SAVEDBATTLEGAME_H

#include <map>
#include <vector>
#include <string>

class Tile;
class SavedGame;
class Terrain;
class MapBlock;
class Craft;
class Ufo;
class Spawnpoint;

/**
 * The game data that gets written to disk when the game is saved.
 * A saved game holds all the variable info in a game like terrain
 * soldiers, items,...
 */
class SavedBattleGame
{
private:
	Terrain* _terrain;
	int _width, _length, _height; // dimensions of the battlemap, origin is topright edge

	int _viewheight; // the layer we are on

	// using a oldskool array here, because the size is static and a little faster than vector
	Tile** _tiles; 

	Craft* _craft;
	Ufo* _ufo;
	std::vector<Spawnpoint*> _spawnpoints;
	//std::vector<BattleSoldier*> _soldiers;
	//std::vector<BattleItem*> _items;


public:
	/// Creates a new battle save, based on current generic save
	SavedBattleGame(SavedGame *save, int width, int length, int height, Terrain* terrain);
	/// Cleans up the saved game.
	~SavedBattleGame();
	/// get pointer to the tiles, a tile is the smallest component of battlescape
	Tile** getTiles();
	/// get pointer to the terrain
	Terrain *getTerrain();
	/// the terrain width
	int getWidth();
	/// the terrain length
	int getLength();
	/// the terrain height
	int getHeight();
	/// the height we are currently viewing
	int getViewHeight();
	/// set relative viewheight
	void setRelativeViewHeight(int height);
	/// conversion between coordinates and the tile index
	int getTileIndex(int x, int y, int z);
	/// get a tile at certain coordinates
	Tile *getTile(int x, int y, int z);
	/// sets the xcom craft and ufo for this battle game
	void setCrafts(Craft *craft, Ufo* ufo);
	/// get the xcom craft in the battle game
	Craft *getCraft();
	/// get the ufo in the battle game (returns NULL when no ufo)
	Ufo *getUfo();
	/// generate a new battlescape map
	void generateMap();

};

#endif
