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
#ifndef OPENXCOM_BASESCAPESTATE_H
#define OPENXCOM_BASESCAPESTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{

class BaseView;
class MiniBaseView;
class Text;
class TextButton;
class TextEdit;
class Base;
class Globe;

/**
 * Basescape screen that shows a base's layout
 * and lets the player manage their bases.
 */
class BasescapeState : public State
{
private:
	BaseView *_view;
	MiniBaseView *_mini;
	Text *_txtFacility, *_txtLocation, *_txtFunds;
	TextEdit *_edtBase;
	TextButton *_btnNewBase, *_btnBaseInfo, *_btnSoldiers, *_btnCrafts, *_btnFacilities, *_btnResearch, *_btnManufacture, *_btnTransfer, *_btnPurchase, *_btnSell, *_btnGeoscape;
	Base *_base;
	Globe *_globe;
public:
	/// Creates the Basescape state.
	BasescapeState(Game *game, Base *base, Globe *globe);
	/// Cleans up the Basescape state.
	~BasescapeState();
	/// Updates the base stats.
	void init();
	/// Sets a new base to display.
	void setBase(Base *base);
	/// Handler for clicking the Build New Base button.
	void btnNewBaseClick(Action *action);
	/// Handler for clicking the Base Information button.
	void btnBaseInfoClick(Action *action);
	/// Handler for clicking the Soldiers button.
	void btnSoldiersClick(Action *action);
	/// Handler for clicking the Equip Craft button.
	void btnCraftsClick(Action *action);
	/// Handler for clicking the Build Facilities button.
	void btnFacilitiesClick(Action *action);
	/// Handler for clicking the Research button.
	void btnResearchClick(Action *action);
	/// Handler for clicking the Manufacture button.
	void btnManufactureClick(Action *action);
	/// Handler for clicking the Purchase/Hire button.
	void btnPurchaseClick(Action *action);
	/// Handler for clicking the Sell/Sack button.
	void btnSellClick(Action *action);
	/// Handler for clicking the Transfer button.
	void btnTransferClick(Action *action);
	/// Handler for clicking the Geoscape button.
	void btnGeoscapeClick(Action *action);
	/// Handler for clicking the base view.
	void viewLeftClick(Action *action);
	/// Handler for right clicking the base view.
	void viewRightClick(Action *action);
	/// Handler for hovering the base view.
	void viewMouseOver(Action *action);
	/// Handler for hovering out of the base view.
	void viewMouseOut(Action *action);
	/// Handler for clicking the mini base view.
	void miniClick(Action *action);
	/// Handler for pressing a key on the Name edit.
	void edtBaseKeyPress(Action *action);
};

}

#endif
