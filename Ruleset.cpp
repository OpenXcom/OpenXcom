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
#include "Ruleset.h"

Ruleset::Ruleset() : _facilities(), _crafts()
{
}

Ruleset::~Ruleset()
{
}

SavedGame *Ruleset::newSave(GameDifficulty diff)
{
	SavedGame *save = new SavedGame(diff);

	return save;
}

RuleBaseFacility *Ruleset::getBaseFacility(LangString id)
{
	return _facilities[id];
}

RuleCraft *Ruleset::getCraft(LangString id)
{
	return _crafts[id];
}