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
#include <vector>
#include "../Engine/State.h"

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class TextList;
class Base;
class Globe;
class Craft;
class Target;

/**
 * Intercept window that lets the player launch
 * crafts into missions from the Geoscape.
 */
class InterceptState : public State
{
private:
	TextButton *_btnCancel, *_btnGotoBase;
	Window *_window;
	Text *_txtTitle, *_txtCraft, *_txtStatus, *_txtBase, *_txtWeapons;
	TextList *_lstCrafts;
	Globe *_globe;
	Base *_base;
	Target *_target;
	std::vector<Craft*> _crafts;
public:
	/// Creates the Intercept state.
	InterceptState(Globe *globe, Base *base = 0, Target *target = 0);
	/// Cleans up the Intercept state.
	~InterceptState();
	/// Handler for clicking the Cancel button.
	void btnCancelClick(Action *action);
	/// Handler for clicking the Go To Base button.
	void btnGotoBaseClick(Action *action);
	/// Handler for clicking the Crafts list.
	void lstCraftsLeftClick(Action *action);
	/// Handler for right clicking the Crafts list.
	void lstCraftsRightClick(Action *action);
};

}
