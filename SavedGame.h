/*
 * Copyright 2010 Daniel Albano
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
#ifndef OPENXCOM__SAVEDGAME_H
#define OPENXCOM__SAVEDGAME_H

#include <map>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "GameTime.h"
#include "Country.h"
#include "LangString.h"
#include "Base.h"

using namespace std;

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
	GameDifficulty _diff;
	GameTime *_time;
	int _funds;
	map<LangString, Country*> _countries;
	vector<Base*> _bases;
	map<LangString, int> _craftId;
public:
	/// Creates a new save with a certain difficulty.
	SavedGame(GameDifficulty diff);
	/// Cleans up the saved game.
	~SavedGame();
	/// Seed used for random number generation.
	static int seed;
	/// Generates a random number.
	static int genRandom(int min, int max);
	/// Gets the current funds.
	int getFunds();
	/// Sets new funds.
	void setFunds(int funds);
	/// Gets the current game time.
	GameTime *getTime();
	/// Gets the list of countries.
	map<LangString, Country*> *getCountries();
	/// Gets the total country funding.
	int getCountryFunding();
	/// Gets the list of bases.
	vector<Base*> *getBases();

	map<LangString, int> *getCraftIds();
};

#endif