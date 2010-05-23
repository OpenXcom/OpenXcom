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

/**
 * Represents a soldier hired by the player.
 * Soldiers have a wide variety of stats that affect
 * their performance during battles.
 */
class Soldier
{
private:
	string _name;
	int _tu, _stamina, _health, _bravery, _reactions, _firing, _throwing, _strength, _psiStrength, _psiSkill, _melee;
	LangString _rank;
	SoldierGender _gender;
	SoldierLook _look;
	int _missions, _kills;
public:
	/// Creates a new soldier with a random name.
	Soldier(vector<SoldierNamePool*> *names);
	/// Cleans up the soldier.
	~Soldier();
	/// Gets the soldier's name.
	string getName();
	/// Sets the soldier's name.
	void setName(string name);
	/// Gets the soldier's rank.
	LangString getRank();
	/// Gets the soldier's time units.
	int getTimeUnits();
	/// Gets the soldier's stamina.
	int getStamina();
	/// Gets the soldier's health.
	int getHealth();
	/// Gets the soldier's bravery.
	int getBravery();
	/// Gets the soldier's reactions.
	int getReactions();
	/// Gets the soldier's firing accuracy.
	int getFiringAccuracy();
	/// Gets the soldier's throwing accuracy.
	int getThrowingAccuracy();
	/// Gets the soldier's strength.
	int getStrength();
	/// Gets the soldier's missions.
	int getMissions();
	/// Gets the soldier's kills.
	int getKills();
};

#endif