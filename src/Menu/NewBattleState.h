/*
 * Copyright 2010-2015 OpenXcom Developers.
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
class ComboBox;
class Slider;
class Frame;
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
	Frame *_frameLeft, *_frameRight;
	Text *_txtTitle, *_txtMapOptions, *_txtAlienOptions;
	Text *_txtMission, *_txtCraft, *_txtDarkness, *_txtTerrain, *_txtDifficulty, *_txtAlienRace, *_txtAlienTech, *_txtDepth;
	ComboBox *_cbxMission, *_cbxCraft, *_cbxTerrain, *_cbxDifficulty, *_cbxAlienRace;
	Slider *_slrDarkness, *_slrAlienTech, *_slrDepth;
	TextButton *_btnOk, *_btnCancel, *_btnEquip, *_btnRandom;
	std::vector<std::string> _missionTypes, _terrainTypes, _alienRaces, _crafts;
	Craft *_craft;
public:
	/// Creates the New Battle state.
	NewBattleState();
	/// Cleans up the New Battle state.
	~NewBattleState();
	/// Resets state.
	void init();
	/// Loads New Battle settings.
	void load(const std::string &filename = "battle");
	/// Saves New Battle settings.
	void save(const std::string &filename = "battle");
	/// Initializes a blank savegame.
	void initSave();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Randomize button.
	void btnRandomClick(Action *action);
	/// Handler for clicking the Equip Craft button.
	void btnEquipClick(Action *action);
	/// Handler for changing the Mission combobox.
	void cbxMissionChange(Action *action);
	/// Handler for changing the Craft combobox.
	void cbxCraftChange(Action *action);
	/// Updates the depth slider accordingly when terrain selection changes.
	void cbxTerrainChange(Action *action);
};

}

#endif
