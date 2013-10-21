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
#ifndef OPENXCOM_OPTIONSBASESTATE_H
#define OPENXCOM_OPTIONSBASESTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

enum OptionsOrigin
{
	OPT_MENU,
	OPT_GEOSCAPE,
	OPT_BATTLESCAPE
};

/**
 * Options base state for common stuff
 * across Options windows.
 */
class OptionsBaseState : public State
{
protected:
	OptionsOrigin _origin;
public:
	/// Creates the Options state.
	OptionsBaseState(Game *game, OptionsOrigin origin);
	/// Cleans up the Options state.
	~OptionsBaseState();
	/// Initializes palettes.
	void init();
	/// Saves the game options.
	void saveOptions();
};

}

#endif
