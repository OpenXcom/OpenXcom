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
#ifndef OPENXCOM_GRAPHSSTATE_H
#define OPENXCOM_GRAPHSSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class InteractiveSurface;
class Window;
class TextList;
class Text;

/**
 * Graphs screen for displaying graphs of various
 * monthly game data like activity and funding.
 */
class GraphsState : public State
{
private:
	Window *_window;
	InteractiveSurface *_btnGeoscape;
	InteractiveSurface *_btnUFO_Activity_Areas;
	InteractiveSurface *_btnUFO_Activity_Countries;
	InteractiveSurface *_btnXCOM_Activity_Areas;
	InteractiveSurface *_btnXCOM_Activity_Countries;
	TextList *_lstResults;
	Text *_txtLabel;
public:
	/// Creates the Graphs state.
	GraphsState(Game *game);
	/// Cleans up the Graphs state.
	~GraphsState();
	/// Handler for clicking the Geoscape icon.
	void btnGeoscapeClick(Action *action);
	void btnUFO_Activity_AreasClick(Action *action);
	void btnUFO_Activity_CountriesClick(Action *action);
	void btnXCOM_Activity_AreasClick(Action *action);
	void btnXCOM_Activity_CountriesClick(Action *action);
};

}

#endif
