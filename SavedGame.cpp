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
#include "SavedGame.h"

SavedGame::SavedGame(GameDifficulty diff) : _diff(diff), _funds(0)
{
	_time = new GameTime(6, 1, 1, 1999, 12, 0, 0);
	srand(seed);
}

SavedGame::~SavedGame()
{
}

int SavedGame::seed = (int)time(NULL);

int SavedGame::genRandom(int min, int max)
{
	seed = rand();
	return (seed % (max - min + 1) + min);
}

int SavedGame::getFunds()
{
	return _funds;
}

void SavedGame::setFunds(int funds)
{
	_funds = funds;
}

GameTime *SavedGame::getTime()
{
	return _time;
}

map<LangString, Country*> *SavedGame::getCountries()
{
	return &_countries;
}

int SavedGame::getCountryFunding()
{
	int total = 0;
	for (map<LangString, Country*>::iterator i = _countries.begin(); i != _countries.end(); i++)
	{
		total += i->second->getFunding();
	}
	return total;
}

vector<Base*> *SavedGame::getBases()
{
	return &_bases;
}