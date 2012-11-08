/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#ifndef OPENXCOM_PLACELIFTSTATE_H
#define OPENXCOM_PLACELIFTSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class Base;
class BaseView;
class Text;
class Globe;

/**
 * Screen shown when the player has to
 * place the access lift of a base.
 */
class PlaceLiftState : public State
{
private:
	Base *_base;
	Globe *_globe;
	BaseView *_view;
	Text *_txtTitle;
	bool _first;
public:
	/// Creates the Place Lift state.
	PlaceLiftState(Game *game, Base *base, Globe *globe, bool first);
	/// Cleans up the Place Lift state.
	~PlaceLiftState();
	/// Handler for clicking the base view.
	void viewClick(Action *action);
};

}

#endif
