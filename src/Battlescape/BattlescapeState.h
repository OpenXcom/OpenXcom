/*
 * Copyright 2010 OpenXcom Developers
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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#ifndef OPENXCOM_BATTLESCAPESTATE_H
#define OPENXCOM_BATTLESCAPESTATE_H

#include "../Engine/State.h"
#include <vector>

class Surface;
class Map;
class ImageButton;
class InteractiveSurface;
class Text;

/**
 * Battlescape screen which shows the tactical battle
 */
class BattlescapeState : public State
{
private:
	Surface *_icons;
	Map *_map;
	InteractiveSurface *_btnUnitUp, *_btnUnitDown, *_btnMapUp, *_btnMapDown, *_btnShowMap, *_btnKneel;
	InteractiveSurface *_btnSoldier, *_btnCenter, *_btnNextSoldier, *_btnNextStop, *_btnShowLayers, *_btnHelp;
	InteractiveSurface *_btnEndTurn, *_btnAbort;
	Text *_txtName;
	bool _pause;
	std::vector<State*> _popups;
public:
	/// Creates the Battlescape state.
	BattlescapeState(Game *game);
	/// Cleans up the Battlescape state.
	~BattlescapeState();
	/// Updates the palette and timer.
	void init();
	/// Runs the timer.
	void think();
	/// Gets the Battlescap map.
	Map *getMap();
	/// Handler for clicking the map.
	void mapClick(Action *action);
	/// Handler for clicking the UnitUp button.
	void btnUnitUpClick(Action *action);
	/// Handler for clicking the UnitDown button.
	void btnUnitDownClick(Action *action);
	/// Handler for clicking the MapUp button.
	void btnMapUpClick(Action *action);
	/// Handler for clicking the MapDown button.
	void btnMapDownClick(Action *action);
	/// Handler for clicking the ShowMap button.
	void btnShowMapClick(Action *action);
	/// Handler for clicking the Kneel button.
	void btnKneelClick(Action *action);
	/// Handler for clicking the Soldier button.
	void btnSoldierClick(Action *action);
	/// Handler for clicking the Center button.
	void btnCenterClick(Action *action);
	/// Handler for clicking the NextSoldier button.
	void btnNextSoldierClick(Action *action);
	/// Handler for clicking the NextStop button.
	void btnNextStopClick(Action *action);
	/// Handler for clicking the ShowLayers button.
	void btnShowLayersClick(Action *action);
	/// Handler for clicking the Help button.
	void btnHelpClick(Action *action);
	/// Handler for clicking the EndTurn button.
	void btnEndTurnClick(Action *action);
	/// Handler for clicking the EndGame button.
	void btnAbortClick(Action *action);
};

#endif
