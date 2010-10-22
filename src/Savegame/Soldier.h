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
#ifndef OPENXCOM_SOLDIER_H
#define OPENXCOM_SOLDIER_H

#include <vector>
#include <string>
#include "../Resource/LangString.h"

enum SoldierGender { GENDER_MALE, GENDER_FEMALE };
enum SoldierLook { LOOK_BLONDE, LOOK_BROWNHAIR, LOOK_ORIENTAL, LOOK_AFRICAN };

class Craft;
class SoldierNamePool;

/**
 * Represents a soldier hired by the player.
 * Soldiers have a wide variety of stats that affect
 * their performance during battles.
 */
class Soldier
{
private:
	std::string _name;
	int _tu, _stamina, _health, _bravery, _reactions, _firing, _throwing, _strength, _psiStrength, _psiSkill, _melee;
	int _rank;
	Craft *_craft;
	SoldierGender _gender;
	SoldierLook _look;
	int _missions, _kills;
public:
	/// Creates a new soldier with a random name.
	Soldier(std::vector<SoldierNamePool*> *names);
	/// Cleans up the soldier.
	~Soldier();
	/// Gets the soldier's name.
	std::string getName();
	/// Sets the soldier's name.
	void setName(std::string name);
	/// Gets the soldier's craft.
	Craft *getCraft();
	/// Sets the soldier's craft.
	void setCraft(Craft *craft);
	/// Gets a string version of the soldier's rank.
	LangString getRankString();
	/// Gets a sprite version of the soldier's rank.
	int getRankSprite();
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
