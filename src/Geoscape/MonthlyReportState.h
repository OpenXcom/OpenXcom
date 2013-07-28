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
#ifndef OPENXCOM_MONTHLYREPORTSTATE_H
#define OPENXCOM_MONTHLYREPORTSTATE_H

#include "../Engine/State.h"
#include <string>

namespace OpenXcom
{

class TextButton;
class Window;
class Text;
class Globe;

/**
 * Report screen shown monthly to display
 * changes in the player's performance and funding.
 */
class MonthlyReportState : public State
{
private:
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtMonth, *_txtRating, *_txtChange, *_txtDesc, *_txtFailure;
	bool _psi, _gameOver;
	int _ratingTotal, _fundingDiff, _lastMonthsRating;
	std::vector<std::string> _happyList, _sadList, _pactList;
	Globe *_globe;
public:
	/// Creates the Monthly Report state.
	MonthlyReportState(Game *game, bool psi, Globe *globe);
	/// Cleans up the Monthly Report state.
	~MonthlyReportState();
	/// Updates the palette.
	void init();
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	void CalculateChanges();
};

}

#endif
