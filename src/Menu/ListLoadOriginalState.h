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
#ifndef OPENXCOM_ListLoadOriginalState_H
#define OPENXCOM_ListLoadOriginalState_H

#include "../Engine/State.h"
#include "../Savegame/SaveConverter.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;

/**
 * Base class for saved game screens which
 * provides the common layout and listing.
 */
class ListLoadOriginalState : public State
{
private:
	TextButton *_btnNew, *_btnCancel;
	Window *_window;
	Text *_txtTitle, *_txtName, *_txtTime, *_txtDate;
	TextButton *_btnSlot[SaveConverter::NUM_SAVES];
	Text *_txtSlotName[SaveConverter::NUM_SAVES];
	Text *_txtSlotTime[SaveConverter::NUM_SAVES];
	Text *_txtSlotDate[SaveConverter::NUM_SAVES];
	SaveOriginal _saves[SaveConverter::NUM_SAVES];
public:
	/// Creates the Saved Game state.
	ListLoadOriginalState();
	/// Cleans up the Saved Game state.
	~ListLoadOriginalState();
	/// Handler for clicking a Save Slot button.
	void btnSlotClick(Action *action);
	/// Handler for clicking the OpenXcom button.
	void btnNewClick(Action *action);
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
};

}

#endif
