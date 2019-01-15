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
#include "OptionsGeoscapeState.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/Slider.h"
#include "../Interface/ToggleTextButton.h"
#include "../Interface/ComboBox.h"
#include "../Engine/Action.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Geoscape Options screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsGeoscapeState::OptionsGeoscapeState(OptionsOrigin origin) : OptionsBaseState(origin)
{
	setCategory(_btnGeoscape);

	// Create objects
	_txtDragScroll = new Text(114, 9, 206, 8);
	_cbxDragScroll = new ComboBox(this, 104, 16, 206, 18);

	_txtScrollSpeed = new Text(114, 9, 94, 8);
	_slrScrollSpeed = new Slider(104, 16, 94, 18);

	_txtDogfightSpeed = new Text(114, 9, 206, 40);
	_slrDogfightSpeed = new Slider(104, 16, 206, 50);

	_txtClockSpeed = new Text(114, 9, 94, 40);
	_slrClockSpeed = new Slider(104, 16, 94, 50);

	_txtGlobeDetails = new Text(114, 9, 94, 82);
	_btnGlobeCountries = new ToggleTextButton(104, 16, 94, 92);
	_btnGlobeRadars = new ToggleTextButton(104, 16, 94, 110);
	_btnGlobePaths = new ToggleTextButton(104, 16, 94, 128);

	_txtOptions = new Text(114, 9, 206, 82);
	_btnShowFunds = new ToggleTextButton(104, 16, 206, 92);

	add(_txtScrollSpeed, "text", "geoscapeMenu");
	add(_slrScrollSpeed, "button", "geoscapeMenu");

	add(_txtDogfightSpeed, "text", "geoscapeMenu");
	add(_slrDogfightSpeed, "button", "geoscapeMenu");

	add(_txtClockSpeed, "text", "geoscapeMenu");
	add(_slrClockSpeed, "button", "geoscapeMenu");

	add(_txtGlobeDetails, "text", "geoscapeMenu");
	add(_btnGlobeCountries, "button", "geoscapeMenu");
	add(_btnGlobeRadars, "button", "geoscapeMenu");
	add(_btnGlobePaths, "button", "geoscapeMenu");

	add(_txtOptions, "text", "geoscapeMenu");
	add(_btnShowFunds, "button", "geoscapeMenu");

	add(_txtDragScroll, "text", "geoscapeMenu");
	add(_cbxDragScroll, "button", "geoscapeMenu");

	centerAllSurfaces();

	// Set up objects
	_txtDragScroll->setText(tr("STR_DRAG_SCROLL"));

	std::vector<std::string> dragScrolls;
	dragScrolls.push_back(tr("STR_DISABLED"));
	dragScrolls.push_back(tr("STR_LEFT_MOUSE_BUTTON"));
	dragScrolls.push_back(tr("STR_MIDDLE_MOUSE_BUTTON"));
	dragScrolls.push_back(tr("STR_RIGHT_MOUSE_BUTTON"));

	_cbxDragScroll->setOptions(dragScrolls);
	_cbxDragScroll->setSelected(Options::geoDragScrollButton);
	_cbxDragScroll->onChange((ActionHandler)&OptionsGeoscapeState::cbxDragScrollChange);
	_cbxDragScroll->setTooltip("STR_DRAG_SCROLL_DESC");
	_cbxDragScroll->onMouseIn((ActionHandler)&OptionsGeoscapeState::txtTooltipIn);
	_cbxDragScroll->onMouseOut((ActionHandler)&OptionsGeoscapeState::txtTooltipOut);

	_txtScrollSpeed->setText(tr("STR_SCROLL_SPEED"));

	_slrScrollSpeed->setRange(100, 10);
	_slrScrollSpeed->setValue(Options::geoScrollSpeed);
	_slrScrollSpeed->setTooltip("STR_SCROLL_SPEED_GEO_DESC");
	_slrScrollSpeed->onChange((ActionHandler)&OptionsGeoscapeState::slrScrollSpeedChange);
	_slrScrollSpeed->onMouseIn((ActionHandler)&OptionsGeoscapeState::txtTooltipIn);
	_slrScrollSpeed->onMouseOut((ActionHandler)&OptionsGeoscapeState::txtTooltipOut);

	_txtDogfightSpeed->setText(tr("STR_DOGFIGHT_SPEED"));

	_slrDogfightSpeed->setRange(50, 20);
	_slrDogfightSpeed->setValue(Options::dogfightSpeed);
	_slrDogfightSpeed->onChange((ActionHandler)&OptionsGeoscapeState::slrDogfightSpeedChange);
	_slrDogfightSpeed->setTooltip("STR_DOGFIGHT_SPEED_DESC");
	_slrDogfightSpeed->onMouseIn((ActionHandler)&OptionsGeoscapeState::txtTooltipIn);
	_slrDogfightSpeed->onMouseOut((ActionHandler)&OptionsGeoscapeState::txtTooltipOut);

	_txtClockSpeed->setText(tr("STR_CLOCK_SPEED"));

	_slrClockSpeed->setRange(250, 10);
	_slrClockSpeed->setValue(Options::geoClockSpeed);
	_slrClockSpeed->setTooltip("STR_CLOCK_SPEED_DESC");
	_slrClockSpeed->onChange((ActionHandler)&OptionsGeoscapeState::slrClockSpeedChange);
	_slrClockSpeed->onMouseIn((ActionHandler)&OptionsGeoscapeState::txtTooltipIn);
	_slrClockSpeed->onMouseOut((ActionHandler)&OptionsGeoscapeState::txtTooltipOut);

	_txtGlobeDetails->setText(tr("STR_GLOBE_DETAILS"));

	_btnGlobeCountries->setText(tr("STR_GLOBE_COUNTRIES"));
	_btnGlobeCountries->setPressed(Options::globeDetail);
	_btnGlobeCountries->onMouseClick((ActionHandler)&OptionsGeoscapeState::btnGlobeCountriesClick);
	_btnGlobeCountries->setTooltip("STR_GLOBE_COUNTRIES_DESC");
	_btnGlobeCountries->onMouseIn((ActionHandler)&OptionsGeoscapeState::txtTooltipIn);
	_btnGlobeCountries->onMouseOut((ActionHandler)&OptionsGeoscapeState::txtTooltipOut);

	_btnGlobeRadars->setText(tr("STR_GLOBE_RADARS"));
	_btnGlobeRadars->setPressed(Options::globeRadarLines);
	_btnGlobeRadars->onMouseClick((ActionHandler)&OptionsGeoscapeState::btnGlobeRadarsClick);
	_btnGlobeRadars->setTooltip("STR_GLOBE_RADARS_DESC");
	_btnGlobeRadars->onMouseIn((ActionHandler)&OptionsGeoscapeState::txtTooltipIn);
	_btnGlobeRadars->onMouseOut((ActionHandler)&OptionsGeoscapeState::txtTooltipOut);

	_btnGlobePaths->setText(tr("STR_GLOBE_FLIGHT_PATHS"));
	_btnGlobePaths->setPressed(Options::globeFlightPaths);
	_btnGlobePaths->onMouseClick((ActionHandler)&OptionsGeoscapeState::btnGlobePathsClick);
	_btnGlobePaths->setTooltip("STR_GLOBE_FLIGHT_PATHS_DESC");
	_btnGlobePaths->onMouseIn((ActionHandler)&OptionsGeoscapeState::txtTooltipIn);
	_btnGlobePaths->onMouseOut((ActionHandler)&OptionsGeoscapeState::txtTooltipOut);

	_txtOptions->setText(tr("STR_USER_INTERFACE_OPTIONS"));

	_btnShowFunds->setText(tr("STR_SHOW_FUNDS"));
	_btnShowFunds->setPressed(Options::showFundsOnGeoscape);
	_btnShowFunds->onMouseClick((ActionHandler)&OptionsGeoscapeState::btnShowFundsClick);
	_btnShowFunds->setTooltip("STR_SHOW_FUNDS_DESC");
	_btnShowFunds->onMouseIn((ActionHandler)&OptionsGeoscapeState::txtTooltipIn);
	_btnShowFunds->onMouseOut((ActionHandler)&OptionsGeoscapeState::txtTooltipOut);
}

/**
 *
 */
OptionsGeoscapeState::~OptionsGeoscapeState()
{

}

/**
 * Changes the Drag Scroll option.
 * @param action Pointer to an action.
 */
void OptionsGeoscapeState::cbxDragScrollChange(Action *)
{
	Options::geoDragScrollButton = _cbxDragScroll->getSelected();
}

/**
 * Updates the scroll speed.
 * @param action Pointer to an action.
 */
void OptionsGeoscapeState::slrScrollSpeedChange(Action *)
{
	Options::geoScrollSpeed = _slrScrollSpeed->getValue();
}

/**
 * Updates the dogfight speed.
 * @param action Pointer to an action.
 */
void OptionsGeoscapeState::slrDogfightSpeedChange(Action *)
{
	Options::dogfightSpeed = _slrDogfightSpeed->getValue();
}

/**
 * Updates the clock speed.
 * @param action Pointer to an action.
 */
void OptionsGeoscapeState::slrClockSpeedChange(Action *)
{
	Options::geoClockSpeed = _slrClockSpeed->getValue();
}

/**
 * Changes the Globe Country Borders option.
 * @param action Pointer to an action.
 */
void OptionsGeoscapeState::btnGlobeCountriesClick(Action *)
{
	Options::globeDetail = _btnGlobeCountries->getPressed();
}

/**
 * Changes the Globe Radar Ranges option.
 * @param action Pointer to an action.
 */
void OptionsGeoscapeState::btnGlobeRadarsClick(Action *)
{
	Options::globeRadarLines = _btnGlobeRadars->getPressed();
}

/**
 * Changes the Globe Flight Paths option.
 * @param action Pointer to an action.
 */
void OptionsGeoscapeState::btnGlobePathsClick(Action *)
{
	Options::globeFlightPaths = _btnGlobePaths->getPressed();
}

/**
 * Changes the Show Funds option.
 * @param action Pointer to an action.
 */
void OptionsGeoscapeState::btnShowFundsClick(Action *)
{
	Options::showFundsOnGeoscape = _btnShowFunds->getPressed();
}

}
