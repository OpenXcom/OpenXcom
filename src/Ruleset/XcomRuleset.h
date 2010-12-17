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
#ifndef OPENXCOM_XCOMRULESET_H
#define OPENXCOM_XCOMRULESET_H

#include "Ruleset.h"

namespace OpenXcom
{

/**
 * Rule set for the X-Com: UFO Defense game.
 */
class XcomRuleset : public Ruleset
{
private:
public:
	/// Creates the X-Com ruleset.
	XcomRuleset();
	/// Cleans up the X-Com ruleset.
	~XcomRuleset();
	/// Generates a saved game with starting equipment.
	SavedGame *newSave(GameDifficulty diff);
	/// Generates a battlescape saved game.
	SavedBattleGame *newBattleSave(SavedGame *save);
	/// Handles the end battle stuff.
	void endBattle(SavedGame *save);
};

}

#endif
