#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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

class Window;
class Text;
class TextButton;
class TextList;

/**
 * Popup window to manage mod.io subscriptions.
 */
class ModPortalState : public State
{
private:
	Window *_window;
	Text *_txtTitle, *_txtLogin, *_txtStatus, *_txtInfo;
	TextButton *_btnLogout, *_btnUpdate;
	TextButton *_btnOk, *_btnWebsite;
public:
	/// Creates the Mods state.
	ModPortalState();
	/// Cleans up the Mods state.
	~ModPortalState();
	/// Updates the mod.io status.
	void think();
	/// Handler for clicking the Logout button.
	void btnLogoutClick(Action *action);
	/// Handler for clicking the Logout button.
	void btnUpdateClick(Action *action);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Website button.
	void btnWebsiteClick(Action *action);
};

}
