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
#ifndef OPENXCOM_ACTIONMENUITEM_H
#define OPENXCOM_ACTIONMENUITEM_H

#include "../Engine/InteractiveSurface.h"
#include "BattlescapeGame.h"

namespace OpenXcom
{

class BattleItem;
class State;
class Font;
class Text;

/**
 * A class that represents a single box in the action popup menu on the battlescape.
 * It shows the possible actions of an item, their TU cost and accuracy.
 * Mouse over highlights the action, when clicked the action is sent to the parent state.
 */
class ActionMenuItem : public InteractiveSurface
{
private:
	int _id;
	bool _highlighted;
	BattleActionType _action;
	int _tu;
	Text *_txtDescription, *_txtAcc, *_txtTU;
public:
	/// Creates a new ActionMenuItem.
	ActionMenuItem(int id, Font *big, Font *small, int x, int y);
	/// Cleans up the ActionMenuItem.
	~ActionMenuItem();
	/// Assign an action to it.
	void setAction(BattleActionType action, std::wstring description, std::wstring accuracy, std::wstring timeunits, int tu);
	/// Get the assigned action.
	BattleActionType getAction() const;
	/// Get the assigned action TUs.
	int getTUs() const;
	/// Set the palettes.
	void setPalette(SDL_Color *colors, int firstcolor, int ncolors);
	/// Redraw it.
	void draw();
	/// Processes a mouse hover in event.
	void mouseIn(Action *action, State *state);
	/// Processes a mouse hover out event.
	void mouseOut(Action *action, State *state);

};

}

#endif
