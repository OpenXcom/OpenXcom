/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "BattlescapeGame.h"

namespace OpenXcom
{
class InteractiveSurface;
class Timer;
class MedikitView;
class Text;
class BattleItem;
class BattleUnit;

/**
 * The Medikit User Interface. Medikit is an item which allow to heal a soldier
 */
class MedikitState : public State
{
	InteractiveSurface *_surface;
	MedikitView *_medikitView;
	Text *_pkText, *_stimulantTxt,  *_healTxt, *_partTxt, *_woundTxt;
	BattleUnit * _targetUnit, *_unit;
	BattleItem *_item;
	BattleAction *_action;
	/// Handler for end button
	void onEndClick(Action * action);
	/// Handler for heal button
	void onHealClick(Action * action);
	/// Handler for stimulant button
	void onStimulantClick(Action * action);
	/// Handler for pain killer button
	void onPainKillerClick(Action * action);
	/// update medikit interface
	void update();
public:
	/// Create the MedikitState
	MedikitState (Game * game, BattleUnit * targetUnit, BattleAction *action);
	/// Handler for right-clicking anything.
	void handle(Action *action);
};
}

#endif
