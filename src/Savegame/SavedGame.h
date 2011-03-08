/*
 * Copyright 2010 OpenXcom Developers.
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
#ifndef OPENXCOM_SAVEDGAME_H
#define OPENXCOM_SAVEDGAME_H

#include <map>
#include <vector>
#include <string>

#define USER_DIR "./USER/"

namespace OpenXcom
{

class Ruleset;
class GameTime;
class Country;
class Base;
class Region;
class Ufo;
class Waypoint;
class SavedBattleGame;
class TextList;
class Language;
class UfopaediaSaved;

/**
 * Enumator containing all the possible game difficulties.
 */
enum GameDifficulty { DIFF_BEGINNER, DIFF_EXPERIENCED, DIFF_VETERAN, DIFF_GENIUS, DIFF_SUPERHUMAN };

/**
 * The game data that gets written to disk when the game is saved.
 * A saved game holds all the variable info in a game like funds,
 * game time, current bases and contents, world activities, score, etc.
 */
class SavedGame
{
private:
	GameDifficulty _difficulty;
	GameTime *_time;
	int _funds;
	std::vector<Country*> _countries;
	std::vector<Region*> _regions;
	std::vector<Base*> _bases;
	std::vector<Ufo*> _ufos;
	std::map<std::string, int> _craftId;
	std::vector<Waypoint*> _waypoints;
	int _ufoId, _waypointId;
	SavedBattleGame *_battleGame;
	UfopaediaSaved *_ufopaedia;
public:
	/// Creates a new save with a certain difficultyiculty.
	SavedGame(GameDifficulty difficulty);
	/// Cleans up the saved game.
	~SavedGame();
	/// Gets list of saves in the user directory.
	static void getList(TextList *list, Language *lang);
	/// Loads a saved game from YAML.
	void load(const std::string &filename, Ruleset *rule);
	/// Saves a saved game to YAML.
	void save(const std::string &filename) const;
	/// Gets the current funds.
	int getFunds() const;
	/// Sets new funds.
	void setFunds(int funds);
	/// Handles monthly funding.
	void monthlyFunding();
	/// Gets the current game time.
	GameTime *const getTime() const;
	/// Gets the list of countries.
	std::vector<Country*> *const getCountries();
	/// Gets the total country funding.
	int getCountryFunding() const;
	/// Gets the list of regions.
	std::vector<Region*> *const getRegions();
	/// Gets the list of bases.
	std::vector<Base*> *const getBases();
	/// Gets the total base maintenance.
	int getBaseMaintenance() const;
	/// Gets the current craft IDs.
	std::map<std::string, int> *const getCraftIds();
	/// Gets the list of UFOs.
	std::vector<Ufo*> *const getUfos();
	/// Gets the current UFO ID.
	int *const getUfoId();
	/// Gets the list of waypoints.
	std::vector<Waypoint*> *const getWaypoints();
	/// Gets the current waypoint ID.
	int *const getWaypointId();
	/// Gets the current battle game.
	SavedBattleGame *const getBattleGame();
	/// Sets the current battle game.
	void setBattleGame(SavedBattleGame *battleGame);
	/// Handles the end battle stuff.
	void endBattle();
	/// Gets the current Ufopaedia parameters.
	UfopaediaSaved *getUfopaedia();
};

}

#endif
