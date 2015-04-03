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
#ifndef OPENXCOM_MEDIKITSTATE_H
#define OPENXCOM_MEDIKITSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class InteractiveSurface;
class MedikitView;
class Text;
class BattleItem;
class BattleUnit;
struct BattleAction;

/**
 * The Medikit User Interface. Medikit is an item for healing a soldier.
 */
class MedikitState : public State
{
	Surface *_bg;
	MedikitView *_medikitView;
	Text *_pkText, *_stimulantTxt,  *_healTxt, *_partTxt, *_woundTxt;
	InteractiveSurface *_endButton, *_stimulantButton, *_pkButton, *_healButton;
	BattleUnit *_targetUnit, *_unit;
	BattleItem *_item;
	BattleAction *_action;
	/// Handler for the end button.
	void onEndClick(Action *action);
	/// Handler for the heal button.
	void onHealClick(Action *action);
	/// Handler for the stimulant button.
	void onStimulantClick(Action *action);
	/// Handler for the pain killer button.
	void onPainKillerClick(Action *action);
	/// Updates the medikit interface.
	void update();
public:
	/// Creates the MedikitState.
	MedikitState(BattleUnit *targetUnit, BattleAction *action);
	/// Handler for right-clicking anything.
	void handle(Action *action);
};
}

#endif
