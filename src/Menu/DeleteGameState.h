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
#ifndef OPENXCOM_DELETEGAMESTATE_H
#define OPENXCOM_DELETEGAMESTATE_H

#include <string>
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class SavedGameState;

/**
 * window used to confirm save game deletion.
 */
class DeleteGameState : public State
{
private:
	TextButton *_btnNo, *_btnYes;
	Window *_window;
	Text *_txtMessage;
	SavedGameState *_parent;
	std::string _filename;
	bool _geo;
public:
	/// Creates the Confirm state.
	DeleteGameState(Game *game, bool geo, const std::wstring &save, SavedGameState *parent);
	/// Cleans up the Confirm state.
	~DeleteGameState();
	/// Handler for clicking the confirm button.
	void btnYesClick(Action *action);
	/// Handler for clicking the cancel button.
	void btnNoClick(Action *action);
};

}

#endif
