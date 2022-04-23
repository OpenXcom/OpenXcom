#pragma once
/*
 * Copyright 2010-2019 OpenXcom Developers.
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
#include "../Engine/State.h"

namespace OpenXcom
{
	class ModListState;
	class ModInfo;
	class TextButton;
	class Window;
	class Text;

	/**
	 * Confirmation window when enabling
	 * mods that require OXCE.
	 */
	class ModConfirmExtendedState : public State
	{
	private:
		ModListState *_state;
		bool _isMaster;
		TextButton *_btnYes, *_btnNo;
		Window *_window;
		Text *_txtTitle;
	public:
		/// Creates the Confirm OXCE state.
		ModConfirmExtendedState(ModListState *state, const ModInfo *modInfo);
		/// Cleans up the Confirm OXCE state.
		~ModConfirmExtendedState();
		/// Handler for clicking the Yes button.
		void btnYesClick(Action *action);
		/// Handler for clicking the No button.
		void btnNoClick(Action *action);
	};

}
