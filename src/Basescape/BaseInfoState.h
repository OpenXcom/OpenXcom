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
#ifndef OPENXCOM_BASEINFOSTATE_H
#define OPENXCOM_BASEINFOSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class BasescapeState;
class Base;
class Surface;
class MiniBaseView;
class TextButton;
class TextEdit;
class Text;
class Bar;

/**
 * Base Info screen that shows all the
 * stats of a base from the Basescape.
 */
class BaseInfoState : public State
{
private:
	Base *_base;
	BasescapeState *_state;

	Surface *_bg;
	MiniBaseView *_mini;
	TextButton *_btnOk, *_btnTransfers, *_btnStores, *_btnMonthlyCosts;
	TextEdit *_edtBase;

	Text *_txtPersonnel, *_txtSoldiers, *_txtEngineers, *_txtScientists;
	Text *_numSoldiers, *_numEngineers, *_numScientists;
	Bar *_barSoldiers, *_barEngineers, *_barScientists;

	Text *_txtSpace, *_txtQuarters, *_txtStores, *_txtLaboratories, *_txtWorkshops, *_txtContainment, *_txtHangars;
	Text *_numQuarters, *_numStores, *_numLaboratories, *_numWorkshops, *_numContainment, *_numHangars;
	Bar *_barQuarters, *_barStores, *_barLaboratories, *_barWorkshops, *_barContainment, *_barHangars;

	Text *_txtDefense, *_txtShortRange, *_txtLongRange;
	Text *_numDefense, *_numShortRange, *_numLongRange;
	Bar *_barDefense, *_barShortRange, *_barLongRange;
public:
	/// Creates the Base Info state.
	BaseInfoState(Game *game, Base *base, BasescapeState *state);
	/// Cleans up the Base Info state.
	~BaseInfoState();
	/// Updates the base stats.
	void init();
	/// Handler for pressing a key on the Name edit.
	void edtBaseKeyPress(Action *action);
	/// Handler for clicking the mini base view.
	void miniClick(Action *action);
	/// Handler for clicking the OK button.
	void btnOkClick(Action *action);
	/// Handler for clicking the Transfers button.
	void btnTransfersClick(Action *action);
	/// Handler for clicking the Stores button.
	void btnStoresClick(Action *action);
	/// Handler for clicking the Monthly Costs button.
	void btnMonthlyCostsClick(Action *action);
};

}

#endif
