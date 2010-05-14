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
#ifndef OPENXCOM__SOLDIER_H
#define OPENXCOM__SOLDIER_H

#include <string>
#include "SavedGame.h"
#include "LangString.h"
#include "SoldierNamePool.h"

enum SoldierGender { GENDER_MALE, GENDER_FEMALE };
enum SoldierLook { LOOK_BLONDE, LOOK_BROWNHAIR, LOOK_ORIENTAL, LOOK_AFRICAN };

using namespace std;

class Soldier
{
private:
	string _name;
	int _tu, _energy, _health, _bravery, _reactions, _firing, _throwing, _strength, _psiStrength, _psiSkill, _melee;
	LangString _rank;
	SoldierGender _gender;
	SoldierLook _look;
	int _missions, _kills;
public:
	Soldier();
	~Soldier();
	string getName();
	void setName(string name);
	LangString getRank();
};

#endif