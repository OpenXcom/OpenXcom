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

/**
 * Initializes a new soldier with random stats and a name
 * pulled from a set of SoldierNamePool's.
 * @param names List of name pools.
 */
Soldier::Soldier(vector<SoldierNamePool*> *names)
{
	_tu = SavedGame::genRandom(50, 60);
	_stamina = SavedGame::genRandom(40, 70);
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
	_missions = 0;
	_kills = 0;

	int gender;
	_name = names->at(SavedGame::genRandom(0, names->size()-1))->genName(&gender);
	_gender = (SoldierGender)gender;
	_look = (SoldierLook)SavedGame::genRandom(0, 3);
}

/**
 *
 */
Soldier::~Soldier()
{
}

/**
 * Returns the soldier's full name.
 * @return Soldier name.
 */
string Soldier::getName()
{
	return _name;
}

/**
 * Changes the soldier's full name.
 * @param name Soldier name.
 */
void Soldier::setName(string name)
{
	_name = name;
}

/**
 * Returns the soldier's military rank.
 * @return String ID for rank.
 */
LangString Soldier::getRank()
{
	return _rank;
}

/**
 * Returns the soldier's amount of time units.
 * @return Time units.
 */
int Soldier::getTimeUnits()
{
	return _tu;
}

/**
 * Returns the soldier's amount of stamina.
 * @return Stamina.
 */
int Soldier::getStamina()
{
	return _stamina;
}

/**
 * Returns the soldier's amount of health.
 * @return Health.
 */
int Soldier::getHealth()
{
	return _health;
}

/**
 * Returns the soldier's amount of bravery.
 * @return Bravery.
 */
int Soldier::getBravery()
{
	return _bravery;
}

/**
 * Returns the soldier's amount of reactions.
 * @return Reactions.
 */
int Soldier::getReactions()
{
	return _reactions;
}

/**
 * Returns the soldier's amount of firing accuracy.
 * @return Firing accuracy.
 */
int Soldier::getFiringAccuracy()
{
	return _firing;
}

/**
 * Returns the soldier's amount of throwing accuracy.
 * @return Throwing accuracy.
 */
int Soldier::getThrowingAccuracy()
{
	return _throwing;
}

/**
 * Returns the soldier's amount of strength.
 * @return Strength.
 */
int Soldier::getStrength()
{
	return _strength;
}

/**
 * Returns the soldier's amount of missions.
 * @return Missions.
 */
int Soldier::getMissions()
{
	return _missions;
}

/**
 * Returns the soldier's amount of kills.
 * @return Kills.
 */
int Soldier::getKills()
{
	return _kills;
}