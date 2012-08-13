/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_NEWBATTLESTATE_H
#define OPENXCOM_NEWBATTLESTATE_H

#include "../Engine/State.h"
#include <vector>
#include <string>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Craft;

/**
 * New Battle that displays a list
 * of options to configure a new
 * standalone mission.
 */
class NewBattleState : public State
{
private:
	Window *_window;
	Text *_txtTitle, *_txtMissionType, *_txtTerrainType, *_txtAlienRace, *_txtDifficulty, *_txtDarkness, *_txtCraft;
	TextButton *_btnMissionType, *_btnTerrainType, *_btnAlienRace, *_btnDifficulty, *_btnDarkness, *_btnCraft;
	TextButton *_btnOk, *_btnCancel, *_btnEquip;
	std::vector<std::string> _missionTypes, _terrainTypes, _alienRaces, _difficulty, _darkness, _crafts;
	size_t _selMission, _selTerrain, _selAlien, _selDifficulty, _selDarkness, _selCraft;
	bool _music;
	Craft *_craft;
public:
	/// Creates the New Battle state.
	NewBattleState(Game *game);
	/// Cleans up the New Battle state.
	~NewBattleState();
	/// Resets state.
	void init();
	/// Initializes a blank savegame.
	void initSave();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Equip Craft button.
	void btnEquipClick(Action *action);
	/// Handler for clicking the Mission Type button.
	void btnMissionTypeClick(Action *action);
	/// Handler for clicking the Mission Option button.
	void btnTerrainTypeClick(Action *action);
	/// Handler for clicking the Alien Race button.
	void btnAlienRaceClick(Action *action);
	/// Handler for clicking the Difficulty button.
	void btnDifficultyClick(Action *action);
	/// Handler for clicking the Darkness button.
	void btnDarknessClick(Action *action);
	/// Handler for clicking the Craft button.
	void btnCraftClick(Action *action);
};

}

#endif
