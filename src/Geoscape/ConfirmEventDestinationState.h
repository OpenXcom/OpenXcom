/*
 * Copyright 2010-2014 OpenXcom Developers.
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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_CONFIRMEVENTDESTINATIONSTATE_H
#define OPENXCOM_CONFIRMEVENTDESTINATIONSTATE_H

#include "../Engine/State.h"
#include "../Savegame/ScriptedEvent.h"
#include "../Engine/Game.h"

namespace OpenXcom
{
class Craft;
class Window;
class Text;
class TextButton;

/**
 * Screen that allows the player
 * to pick a target for a craft on the globe.
 */
class ConfirmEventDestinationState : public State
{
private:
	Window *_window;
	Text *_txtMessage;
	TextButton *_btnNo, *_btnYes;
	Game *_game;
	Craft *_craft;
	ScriptedEvent *_scriptedEvent;
	const MissionScreenData *_screenData;
public:
	/// Creates the Select Destination state.
	ConfirmEventDestinationState(Game *game, Craft *craft, ScriptedEvent *scriptedEvent);
	/// Cleans up the Select Destination state.
	~ConfirmEventDestinationState();
	/// Handler for clicking the Cancel button.
	void btnNoClick(Action *action);
	/// Handler for clicking the Event Destination mission button.
	void btnYesClick(Action *action);

};

}

#endif
