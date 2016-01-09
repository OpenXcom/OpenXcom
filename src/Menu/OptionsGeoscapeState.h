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
#include "OptionsBaseState.h"

namespace OpenXcom
{

class Text;
class TextButton;
class ToggleTextButton;
class Slider;
class ComboBox;

/**
 * Screen that lets the user configure various
 * Geoscape options.
 */
class OptionsGeoscapeState : public OptionsBaseState
{
private:
	Text *_txtDragScroll;
	ComboBox *_cbxDragScroll;
	Text *_txtScrollSpeed, *_txtDogfightSpeed, *_txtClockSpeed;
	Slider *_slrScrollSpeed, *_slrDogfightSpeed, *_slrClockSpeed;
	Text *_txtGlobeDetails;
	ToggleTextButton *_btnGlobeCountries, *_btnGlobeRadars, *_btnGlobePaths;
	Text *_txtOptions;
	ToggleTextButton *_btnShowFunds;
public:
	/// Creates the Geoscape Options state.
	OptionsGeoscapeState(OptionsOrigin origin);
	/// Cleans up the Geoscape Options state.
	~OptionsGeoscapeState();
	/// Handler for changing the Drag Scroll combobox.
	void cbxDragScrollChange(Action *action);
	/// Handler for changing the scroll speed slider.
	void slrScrollSpeedChange(Action *action);
	/// Handler for changing the dogfight speed slider.
	void slrDogfightSpeedChange(Action *action);
	/// Handler for changing the clock speed slider.
	void slrClockSpeedChange(Action *action);
	/// Handler for clicking the Country Borders button.
	void btnGlobeCountriesClick(Action *action);
	/// Handler for clicking the Radar Ranges button.
	void btnGlobeRadarsClick(Action *action);
	/// Handler for clicking the Flight Paths button.
	void btnGlobePathsClick(Action *action);
	/// Handler for clicking the Show Funds button.
	void btnShowFundsClick(Action *action);
};

}
