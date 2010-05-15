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
#include "Soldier.h"

Soldier::Soldier(vector<SoldierNamePool*> *names)
{
	_tu = SavedGame::genRandom(50, 60);
	_energy = SavedGame::genRandom(40, 70);
	_health = SavedGame::genRandom(25, 40);
	_bravery = SavedGame::genRandom(1, 6)*10;
	_reactions = SavedGame::genRandom(50, 60);
	_firing = SavedGame::genRandom(40, 70);
	_throwing = SavedGame::genRandom(50, 80);
	_strength = SavedGame::genRandom(20, 40);
	_psiStrength = SavedGame::genRandom(0, 100);
	_psiSkill = 0;
	_melee = SavedGame::genRandom(20, 40);
	_rank = STR_ROOKIE;

	int gender;
	_name = names->at(SavedGame::genRandom(0, names->size()-1))->genName(&gender);
	_gender = (SoldierGender)gender;
	_look = (SoldierLook)SavedGame::genRandom(0, 3);
}

Soldier::~Soldier()
{
}

string Soldier::getName()
{
	return _name;
}

void Soldier::setName(string name)
{
	_name = name;
}

LangString Soldier::getRank()
{
	return _rank;
}