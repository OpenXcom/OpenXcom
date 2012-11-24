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
#include <string>

namespace OpenXcom
{

class Surface;
class InteractiveSurface;
class Text;
class TextList;

/**
 * Graphs screen for displaying graphs of various
 * monthly game data like activity and funding.
 */
class GraphsState : public State
{
private:
	Surface *_bg;
	InteractiveSurface *_btnGeoscape;
	InteractiveSurface *_btnXcomCountry, *_btnUfoCountry;
	InteractiveSurface *_btnXcomRegion, *_btnUfoRegion;
	InteractiveSurface *_btnIncome, *_btnFinance;
	Text *_txtTitle;
	TextList *_txtMonths, *_txtYears;
	Text *_txtScale0, *_txtScale1, *_txtScale2, *_txtScale3, *_txtScale4;
	Text *_txtScale5, *_txtScale6, *_txtScale7, *_txtScale8, *_txtScale9;
public:
	/// Creates the Graphs state.
	GraphsState(Game *game);
	/// Cleans up the Graphs state.
	~GraphsState();
	/// Handler for clicking the Geoscape icon.
	void btnGeoscapeClick(Action *action);
	void btnUfoRegionClick(Action *action);
	void btnUfoCountryClick(Action *action);
	void btnXcomRegionClick(Action *action);
	void btnXcomCountryClick(Action *action);
	void btnIncomeClick(Action *action);
	void btnFinanceClick(Action *action);
};

}

#endif
